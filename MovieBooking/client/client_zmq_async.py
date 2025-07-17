# install using `pip install pyzmq transitions`

import zmq
import os
import time
import json
import threading

import queue

from transitions import Machine


# PID = os.getpid()
# CLIENT_NAME = "client-{0}".format(PID)
# print(f"CLIENT_NAME={CLIENT_NAME}")

# receiver_stop_event = threading.Event()
# zmqLock = threading.Lock()

# responses: dict = {}

# expect_response_event = threading.Event()



# print("************** Type 'bye' to stop!!! **************")

# context = zmq.Context()

# socket = context.socket(zmq.DEALER)

# socket.setsockopt(zmq.RCVTIMEO, 5000) 
# socket.setsockopt(zmq.SNDTIMEO, 3000) 
# socket.setsockopt_string(zmq.IDENTITY, CLIENT_NAME)

# socket.connect("tcp://localhost:52345")

# print(f"request_id B: {request_id_b}")

# responses[request_id_a] = queue.Queue()

# zmqClient = ZmqClient(request_id_a, socket)
# clientModel = ClientModel(zmqClient, "Mark", 2)
# machine = Machine(model=clientModel, states=states, transitions=transitions, initial='initial')

# receiver_thread = threading.Thread(target=receiver_thread_handler, args=(socket, receiver_stop_event,))
# receiver_thread.start()

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
