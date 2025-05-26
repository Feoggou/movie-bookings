import json
import threading
import queue

import zmq

from zmq_network import zmqLock


expect_response_event = threading.Event()
receiver_stop_event = threading.Event()
responses: dict = {}


def exit_on_error(func):
    def wrapper(*args, **kwargs):
        try:
            return func(*args, **kwargs)
        except Exception as e:
            print(f"Function FAILED  with error {type(e).__name__}: \"{str(e)}\"=> exiting")
            exit(1)
    return wrapper


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


def start_receiver_thread(socket: zmq.Socket):
    receiver_thread = threading.Thread(target=receiver_thread_handler, args=(socket, receiver_stop_event,))
    receiver_thread.start()
    return receiver_thread


def register_request_id(request_id: str):
    responses[request_id] = queue.Queue()


def stop_receiver_thread(receiver_thread: threading.Thread):
    receiver_stop_event.set()
    receiver_thread.join()


def cancel_active_requests():
    expect_response_event.set()
