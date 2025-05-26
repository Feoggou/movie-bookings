import os
import threading

import zmq


PID = os.getpid()
CLIENT_NAME = "client-{0}".format(PID)
print(f"CLIENT_NAME={CLIENT_NAME}")

zmqLock = threading.Lock()

context = zmq.Context()


def create_socket():
    socket = context.socket(zmq.DEALER)

    socket.setsockopt(zmq.RCVTIMEO, 5000) 
    socket.setsockopt(zmq.SNDTIMEO, 3000) 
    socket.setsockopt_string(zmq.IDENTITY, CLIENT_NAME)

    socket.connect("tcp://localhost:52345")

    return socket
