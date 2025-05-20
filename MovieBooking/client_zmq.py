# install using `pip install pyzmq`

import zmq

# 1. Create a ZeroMQ context
context = zmq.Context()

# 2. Create a REQ (Request) socket
socket = context.socket(zmq.REQ)

# 3. Connect to the server
socket.connect("tcp://localhost:52345")

# 4. Send a request and wait for reply
for i in range(3):
    message = "Hello from Python client"
    print(f"Sending request {i+1}: {message}")
    socket.send_string(message)

    # 5. Wait for reply
    reply = socket.recv_string()
    print(f"Received reply {i+1}: {reply}")

    