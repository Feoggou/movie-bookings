import json

import zmq

from zmq_network import zmqLock
from client_receiver import responses, expect_response_event


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
            timeout = 6

        if self.request_id not in responses:
            print(f"Could not find request id '{self.request_id}' key in responses. responses size={len(responses)}")
            return []

        if responses[self.request_id] is None:
            print("Trying to get a response from NONE queue...")
            return []

        return responses[self.request_id].get(block=True, timeout=timeout)
