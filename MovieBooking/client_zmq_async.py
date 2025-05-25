# install using `pip install pyzmq transitions`

import zmq
import os
import time
import json
import threading
import uuid

import queue

from transitions import Machine


PID = os.getpid()
CLIENT_NAME = "client-{0}".format(PID)
print(f"CLIENT_NAME={CLIENT_NAME}")

receiver_stop_event = threading.Event()
zmqLock = threading.Lock()

responses: dict = {}

expect_response_event = threading.Event()

def exit_on_error(func):
    def wrapper(*args, **kwargs):
        try:
            return func(*args, **kwargs)
        except Exception as e:
            print(f"Function FAILED  with error {type(e).__name__}: \"{str(e)}\"=> exiting")
            exit(1)
    return wrapper


class ZmqClient:
    def __init__(self, request_id: str, socket: zmq.Socket):
        self.request_id = request_id
        self.socket = socket

    def request(self, cmd_name: str, *args) -> None:
        if args:
            message = json.dumps({"request_id": self.request_id, "cmd": cmd_name, "args": list(args)})
        else:
            message = json.dumps({"request_id": self.request_id, "cmd": cmd_name})

        # print("lock zmq...")
        with zmqLock:
            self.socket.send_string(message)
            # socket.send_multipart([request_id_a.encode("utf-8"), message.encode("utf-8")])
            expect_response_event.set()
        # print("... unlock zmq")

    def get_response(self, timeout=None) -> list:
        if timeout is None:
            timeout = 3

        if self.request_id not in responses:
            print(f"Could not find request id '{self.request_id}' key in responses. responses size={len(responses)}")
            return []

        if responses[self.request_id] is None:
            print("Trying to get a response from NONE queue...")
            return []

        return responses[self.request_id].get(block=True, timeout=timeout)


class ClientModel:
    def __init__(self, client: ZmqClient, client_name: str, count_seats_wanted: int):
        self.movies = []
        self.client = client
        self.movie = ""
        self.all_theaters = []
        self.theater = ""
        self.theater_idx = None
        self.all_seats = []
        self.count_seats_wanted = count_seats_wanted
        self.client_name = client_name
        self.booked_the_seats = False

    """
    get movies
    """
    def on_enter_get_movies_state(self):
        print("[enter:get_movies_state] requesting list of movies...")
        self.client.request("getPlayingMovies")
        # print("... requested list of movies (next, we'll choose a movie) ...")
        self.choose_movie()

    def on_exit_get_movies_state(self):
        # print("[exit:get_movies_state] waiting...")
        self.all_movies = self.client.get_response()
        print("[exit:get_movies_state]: list of movies: ", self.all_movies)

    """
    choose movie
    """
    def on_enter_choose_movie_state(self):
        if self.all_movies:
            self.movie = self.all_movies[0]
            print(f"[enter:choose_movie_state] choosing movie: '{self.movie}'")
            self.find_theaters()
        else:
            print("[enter:choose_movie_state] have no movies to choose from => leaving")
            self.leave()

    """
    get theaters
    """
    def on_enter_get_theaters_state(self):
        print("[enter:get_theaters_state] requesting list of theaters...")
        self.client.request("getTheaterNamesForMovie", self.movie)
        # print("... requested list of theaters (next, we'll choose a theater) ...")
        self.choose_theater()

    def on_exit_get_theaters_state(self):
        # print("[exit:get_theaters_state] waiting...")
        self.all_theaters = self.client.get_response()
        print("[exit:get_theaters_state]: list of theaters: ", self.all_theaters)

    """
    choose theater
    """
    def on_enter_choose_theater_state(self):
        if self.all_theaters:
            if self.theater_idx is None:
                self.theater_idx = 0
            else:
                self.theater_idx += 1

            if self.theater_idx >= len(self.all_theaters):
                print(f"Theater index {self.theater_idx} -- count teathers is {len(self.all_theaters)} => have no theater to choose from")
                self.leave()
                return

            self.theater = self.all_theaters[self.theater_idx]
            print(f"[enter:choose_theater_state] choosing theater {self.theater_idx}: '{self.theater}'")
            self.get_available_seats()
        else:
            print("[enter:choose_theater_state] have no theaters to choose from => leaving")
            self.leave()

    """
    get available seats
    """
    def on_enter_get_available_seats_state(self):
        if receiver_stop_event.is_set():
            print("[enter:get_available_seats_state] The client is coming down. Leaving SM")
            self.leave()
            return

        print("[enter:get_available_seats_state] requesting available seats...")
        self.client.request("getAvailableSeats", self.movie, self.theater)
        # print("... requested list of available seats (next, we'll book some seats) ...")
        if self.booked_the_seats:
            print("****** Already booked seats *********")
            self.leave()
        else:
            self.choose_seats()

    def on_exit_get_available_seats_state(self):
        if receiver_stop_event.is_set():
            return

        # print("[exit:get_available_seats] waiting...")
        try:
            self.all_seats = self.client.get_response()
            print("[exit:get_available_seats_state]: list of available seats: ", self.all_seats)
        except queue.Empty as e:
            print("Failed to get seats in a timely manner...")

    """
    choose seats
    """
    def on_enter_choose_seats_state(self):
        if self.all_seats:
            self.chosen_seats = self.all_seats[0]
            print(f"[enter:choose_seats_state] choosing seats: '{self.chosen_seats}'")
            self.book_seats()
        else:
            print("[enter:choose_seats_state] have no seats avaiable to choose from => choosing different theater")
            self.choose_theater()

    """
    book seats
    """
    def on_enter_book_seats_state(self):
        if receiver_stop_event.is_set():
            print("[enter:book_seats_state] The client is coming down. Leaving SM")
            self.leave()
            return

        if self.count_seats_wanted > len(self.all_seats):
            print(f"[enter:book_seats_state] client wants {self.count_seats_wanted} seats but only {len(self.all_seats)} are avaiable. Leaving...")
            self.leave()
            return

        seats_to_book = self.all_seats[:self.count_seats_wanted]
        print("we want to get these seats --- ", seats_to_book)

        print("[enter:book_seats_state] booking seats...")
        self.client.request("bookSeats", self.client_name, self.movie, self.theater, seats_to_book)
        # print("... requested book seats (next, we'll check again available seats) ...")
        self.check_available_seats()

    def on_exit_book_seats_state(self):
        if receiver_stop_event.is_set():
            return

        # print("[exit:book_seats_state] waiting...")
        try:
            booked_seats = self.client.get_response(10)

            print("[exit:book_seats_state]: booked seats: ", booked_seats)
            self.booked_the_seats = True
        except Exception as e:
            print(f"Failed to book seats. Error {type(e).__name__}: '{str(e)}'")

    """
    finished
    """
    def on_enter_finished(self):
        print("[finished]")

    """
    error
    """
    def on_enter_error(self):
        print("[enter:error]")


states = ['initial', 'get_movies_state', 'choose_movie_state', 'chooseMovie', 'get_theaters_state', 'choose_theater_state', 'get_available_seats_state', 'choose_seats_state', 'book_seats_state', 'error', 'finished']

transitions = [
    {'source': 'initial',                       'dest': 'get_movies_state',             'trigger': 'request_playing_movies'},
    {'source': 'get_movies_state',              'dest': 'choose_movie_state',           'trigger': 'choose_movie'},
    {'source': 'choose_movie_state',            'dest': 'finished',                     'trigger': 'leave'},
    {'source': 'choose_movie_state',            'dest': 'get_theaters_state',           'trigger': 'find_theaters'},
    {'source': 'get_theaters_state',            'dest': 'choose_theater_state',         'trigger': 'choose_theater'},
    {'source': 'choose_theater_state',          'dest': 'finished',                     'trigger': 'leave'},
    {'source': 'choose_theater_state',          'dest': 'get_available_seats_state',    'trigger': 'get_available_seats'},
    {'source': 'get_available_seats_state',     'dest': 'choose_seats_state',           'trigger': 'choose_seats'},
    {'source': 'get_available_seats_state',     'dest': 'finished',                     'trigger': 'leave'},
    {'source': 'choose_seats_state',            'dest': 'choose_theater_state',         'trigger': 'choose_theater'},
    {'source': 'choose_seats_state',            'dest': 'book_seats_state',             'trigger': 'book_seats'},
    {'source': 'book_seats_state',              'dest': 'finished',                     'trigger': 'leave'},
    {'source': 'book_seats_state',              'dest': 'get_available_seats_state',    'trigger': 'check_available_seats'},
    {'source': 'book_seats_state',              'dest': 'error',                        'trigger': 'fail'},
]


def wait_for_response(socket: zmq.Socket) -> bool:
    # print("Waiting for request to be sent...")
    expect_response_event.wait()
    expect_response_event.clear()

    if receiver_stop_event.is_set():
        print("... waiting stopped (receiver probably completed).")
        return False

    # print("... expecting RESPONSE ...")

    for i in range(1, 100):
        with zmqLock:
            if socket.poll(timeout=100):
                # print("... found a response!")
                return True

    print("Waited too long... I suppose this means it's bad!")
    return False


def get_next_response(socket: zmq.Socket) -> [dict, None]:
    # print("ZMQ lock...")
    with zmqLock:
        reply_str = socket.recv_string()
    # print("... ZMQ unlock")

    # reply_str = socket.recv_string()
    return json.loads(reply_str)


def enqueue_response(resp: dict):
    print("Received RESPONSE:",)
    print("    ID=", resp["request_id"])
    print("    RESPONSE=", resp["response"])

    key = resp["request_id"]

    responses[key].put(resp["response"])
    print(f"Responses in QUEUE: {responses[key].qsize()}")


@exit_on_error
def receiver_thread_handler(socket: zmq.Socket, stop_event: threading.Event):
    print("Started receiver_thread_handler...")
    while not stop_event.is_set():
        if not wait_for_response(socket):
            receiver_stop_event.set()
            break

        # print("Retrieving response...")
        response = get_next_response(socket)
        # print("... Response retrieved: ", response)
        if not response:
            print("Bad or no response => leaving...")
            receiver_stop_event.set()
            break

        # print("******** Got response: ", response)

        enqueue_response(response)

    print("Worker exited. Please say bye!")



print("************** Type 'bye' to stop!!! **************")

context = zmq.Context()

socket = context.socket(zmq.DEALER)

socket.setsockopt(zmq.RCVTIMEO, 5000) 
socket.setsockopt(zmq.SNDTIMEO, 3000) 
socket.setsockopt_string(zmq.IDENTITY, CLIENT_NAME)

socket.connect("tcp://localhost:52345")

request_id_a = str(uuid.uuid4())
# request_id_b = str(uuid.uuid4())

print(f"request_id A: {request_id_a}")
# print(f"request_id B: {request_id_b}")

responses[request_id_a] = queue.Queue()
# responses[request_id_b] = []

zmqClient = ZmqClient(request_id_a, socket)
clientModel = ClientModel(zmqClient, "Mark", 2)
machine = Machine(model=clientModel, states=states, transitions=transitions, initial='initial')

receiver_thread = threading.Thread(target=receiver_thread_handler, args=(socket, receiver_stop_event,))
receiver_thread.start()

print("\n\n")
clientModel.request_playing_movies()

print("FSM finished")
receiver_stop_event.set()
expect_response_event.set()

try:
    while True:
        line = input().strip()
        if line.lower() == "bye":
            break
except BaseException as e:
    print(f"\nError {type(e).__name__}: \"{str(e)}\"")

receiver_thread.join()


print("Bye!")
