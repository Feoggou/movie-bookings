import sys
import uuid
import queue

import client_module

import client_fsm as fsm
from client_module import ZmqClient, responses
from zmq_network import create_socket
from client_receiver import start_receiver_thread, stop_receiver_thread, cancel_active_requests

def main(*args):
    print("************** Type 'bye' to stop!!! **************")
    print("\n\n")

    socket = create_socket()

    recv_thread = start_receiver_thread(socket)

    fsm.run_client_thread(socket)

    print("FSM finished")
    cancel_active_requests()
    stop_receiver_thread(recv_thread)

    try:
        while True:
            line = input().strip()
            if line.lower() == "bye":
                break
    except BaseException as e:
        print(f"\nError {type(e).__name__}: \"{str(e)}\"")

    print("Bye!")


if __name__ == "__main__":
    main(sys.argv[1:])
