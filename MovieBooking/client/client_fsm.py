import zmq
import threading

from transitions import Machine

from client_module import ZmqClient
from client_receiver import receiver_stop_event, register_request_id

thread_a_started = threading.Event()


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
        self.rq_id = client.request_id

    """
    get movies
    """
    def on_enter_get_movies_state(self):
        print(f"{self.rq_id}[enter:get_movies_state] requesting list of movies...")
        self.client.request("getPlayingMovies")
        # print("{self.rq_id}... requested list of movies (next, we'll choose a movie) ...")
        self.choose_movie()

    def on_exit_get_movies_state(self):
        # print("{self.rq_id}[exit:get_movies_state] waiting...")
        self.all_movies = self.client.get_response()
        print(f"{self.rq_id}[exit:get_movies_state]: list of movies: ", self.all_movies)

    """
    choose movie
    """
    def on_enter_choose_movie_state(self):
        if self.all_movies:
            self.movie = self.all_movies[0]
            print(f"{self.rq_id}[enter:choose_movie_state] choosing movie: '{self.movie}'")
            self.find_theaters()
        else:
            print(f"{self.rq_id}[enter:choose_movie_state] have no movies to choose from => leaving")
            self.leave()

    """
    get theaters
    """
    def on_enter_get_theaters_state(self):
        print("{self.rq_id}[enter:get_theaters_state] requesting list of theaters...")
        self.client.request("getTheaterNamesForMovie", self.movie)
        # print("{self.rq_id}... requested list of theaters (next, we'll choose a theater) ...")
        self.choose_theater()

    def on_exit_get_theaters_state(self):
        # print("{self.rq_id}[exit:get_theaters_state] waiting...")
        self.all_theaters = self.client.get_response()
        print(f"{self.rq_id}[exit:get_theaters_state]: list of theaters: ", self.all_theaters)

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
                print(f"{self.rq_id} -- Theater index {self.theater_idx} -- count teathers is {len(self.all_theaters)} => have no theater to choose from")
                self.leave()
                return

            self.theater = self.all_theaters[self.theater_idx]
            print(f"{self.rq_id}[enter:choose_theater_state] choosing theater {self.theater_idx}: '{self.theater}'")
            self.get_available_seats()
        else:
            print("{self.rq_id}[enter:choose_theater_state] have no theaters to choose from => leaving")
            self.leave()

    """
    get available seats
    """
    def on_enter_get_available_seats_state(self):
        if receiver_stop_event.is_set():
            print(f"{self.rq_id}[enter:get_available_seats_state] The client is coming down. Leaving SM")
            self.leave()
            return

        print(f"{self.rq_id}[enter:get_available_seats_state] requesting available seats...")
        self.client.request("getAvailableSeats", self.movie, self.theater)
        # print("... requested list of available seats (next, we'll book some seats) ...")
        if self.booked_the_seats:
            print(f"{self.rq_id}[enter:get_available_seats_state] already booked seats")
            self.leave()
        else:
            self.choose_seats()

    def on_exit_get_available_seats_state(self):
        if receiver_stop_event.is_set():
            return

        # print("[exit:get_available_seats] waiting...")
        try:
            self.all_seats = self.client.get_response()
            print(f"{self.rq_id}[exit:get_available_seats_state]: list of available seats: ", self.all_seats)
        except queue.Empty as e:
            print(f"{self.rq_id} -- Failed to get seats in a timely manner...")

    """
    choose seats
    """
    def on_enter_choose_seats_state(self):
        if self.all_seats:
            self.chosen_seats = self.all_seats[0]
            print(f"{self.rq_id}[enter:choose_seats_state] choosing seats: '{self.chosen_seats}'")
            self.book_seats()
        else:
            print(f"{self.rq_id}[enter:choose_seats_state] have no seats avaiable to choose from => choosing different theater")
            self.choose_theater()

    """
    book seats
    """
    def on_enter_book_seats_state(self):
        if receiver_stop_event.is_set():
            print(f"{self.rq_id}[enter:book_seats_state] The client is coming down. Leaving SM")
            self.leave()
            return

        if self.count_seats_wanted > len(self.all_seats):
            print(f"{self.rq_id}[enter:book_seats_state] client wants {self.count_seats_wanted} seats but only {len(self.all_seats)} are avaiable. Leaving...")
            self.leave()
            return

        seats_to_book = self.all_seats[:self.count_seats_wanted]
        print(f"{self.rq_id} -- we want to get these seats --- ", seats_to_book)

        print(f"{self.rq_id}[enter:book_seats_state] booking seats...")
        self.client.request("bookSeats", self.client_name, self.movie, self.theater, seats_to_book)
        # print("... requested book seats (next, we'll check again available seats) ...")
        self.check_available_seats()

    def on_exit_book_seats_state(self):
        if receiver_stop_event.is_set():
            return

        # print("[exit:book_seats_state] waiting...")
        try:
            booked_seats = self.client.get_response(10)

            print(f"{self.rq_id}[exit:book_seats_state]: booked seats: ", booked_seats)
            self.booked_the_seats = True
        except Exception as e:
            print(f"{self.rq_id} -- Failed to book seats. Error {type(e).__name__}: '{str(e)}'")

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


def make_and_run_state_machine(zmq_client: ZmqClient, client_name: str, seats_wanted: int):
    clientModel = ClientModel(zmq_client, client_name, seats_wanted)
    machine = Machine(model=clientModel, states=states, transitions=transitions, initial='initial')
    
    clientModel.request_playing_movies()


def client_thread_handler(zmq_client: ZmqClient, client_name: str, seats_wanted: int, signal: bool):
    print(f"** start {zmq_client.request_id} = {client_name} **")

    if signal:
        thread_a_started.set()

    make_and_run_state_machine(zmq_client, client_name, seats_wanted)


def start_client_thread(socket: zmq.Socket, request_id: str, client_name: str, seats_wanted: int, signal: bool):
    zmq_client = ZmqClient(request_id, socket)

    client_thread = threading.Thread(target=client_thread_handler, args=(zmq_client, client_name, seats_wanted, signal))
    client_thread.start()
    return client_thread


def run_client_thread(socket: zmq.Socket):
    # request_id_a = str(uuid.uuid4())
    # request_id_b = str(uuid.uuid4())
    request_id_a = "[*A*]"
    request_id_b = "[*B*]"
    print(f"request_id A: {request_id_a}")
    print(f"request_id B: {request_id_b}")

    register_request_id(request_id_a)
    register_request_id(request_id_b)

    client_thread_a = start_client_thread(socket, request_id_a, "Mark", seats_wanted=1, signal=True)

    thread_a_started.wait()
    client_thread_b = start_client_thread(socket, request_id_b, "Joe", seats_wanted=1, signal=False)

    client_thread_a.join()
    print("Client A finished.")
    client_thread_b.join()
    print("Client B finished.")
