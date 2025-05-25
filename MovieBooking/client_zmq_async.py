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

    def request(self, cmd_name: str) -> None:
        message = json.dumps({"request_id": self.request_id, "cmd": cmd_name})

        with zmqLock:
            self.socket.send_string(message)
            # socket.send_multipart([request_id_a.encode("utf-8"), message.encode("utf-8")])
            expect_response_event.set()

    def get_response(self) -> list:
        return responses[self.request_id].get(block=True, timeout=3)


class ClientModel:
    def __init__(self, client: ZmqClient):
        self.movies = []
        self.client = client

    def on_enter_want_movies_state(self):
        print("[enter:want_movies_state] Send request for list of movies...")
        self.client.request("getPlayingMovies")
        self.next_step()

    def on_exit_want_movies_state(self):
        print("[exit:want_movies_state] waiting...")
        self.movies = self.client.get_response()
        print("[exit:want_movies_state]: list of movies: ", self.movies)

    def haveMovies(self):
        print("Called haveMovies")
        return True
        return self.movies

    def fetch_movies_response(self):
        print("Called fetch_movies_response")
        return self.movies

    def error(self):
        print("SM failed with error!")
    pass

states = ['initial', 'want_movies_state', 'wait_for_list_of_movies', 'error', 'finished']

transitions = [
    {'trigger': 'request_playing_movies',  'source': 'initial', 'dest': 'want_movies_state'}
    {'trigger': 'next_step',  'source': 'want_movies_state', 'dest': 'finished'}
]


def wait_for_response(socket: zmq.Socket) -> bool:
    expect_response_event.wait()
    expect_response_event.clear()

    if receiver_stop_event.is_set():
        print("... waiting aborted.")
        return False

    for i in range(1, 30):
        with zmqLock:
            if socket.poll(timeout=100):
                # print("... found a response!")
                return True

    print("Waited too long... I suppose this means it's bad!")
    return False


def get_next_response(socket: zmq.Socket) -> [dict, None]:
    with zmqLock:
        reply_str = socket.recv_string()

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
            break

        response = get_next_response(socket)
        if not response:
            print("Bad or no response => leaving...")
            receiver_stop_event.set()
            break

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

print(f"request_id A: {request_id_a}")

responses[request_id_a] = queue.Queue()

zmqClient = ZmqClient(request_id_a, socket)
clientModel = ClientModel(zmqClient)
machine = Machine(model=clientModel, states=states, transitions=transitions, initial='initial')

receiver_thread = threading.Thread(target=receiver_thread_handler, args=(socket, receiver_stop_event,))
receiver_thread.start()

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
