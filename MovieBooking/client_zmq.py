# install using `pip install pyzmq`

import zmq
import os
import time
import json

PID = os.getpid()
print(f"PID={PID}")


def execute_command(socket, cmd: str, args:list=None) -> dict:
    message = json.dumps({"pid": PID, "cmd": cmd}) if args is None else json.dumps({"pid": PID, "cmd": cmd, "args": args})
    # print(f"request {i+1}: {message}")
    socket.send_string(message)

    # 5. Wait for reply
    reply = socket.recv_string()
    print(f"[{PID}] --- reply: {reply}")
    time.sleep(1)

    return json.loads(reply)

def run_all(socket):
    reply = execute_command(socket, "getPlayingMovies")
    movie = reply[0]
    print("****** Chosen movie: {0} **** ".format(movie))

    reply = execute_command(socket, "getTheaterNamesForMovie", [movie])
    theater = reply[1]
    print("****** Chosen theater: {0} **** ".format(theater))

    reply = execute_command(socket, "getAvailableSeats", [movie, theater])
    if not reply:
        return

    seats = reply
    print("****** ALL seats: {0} **** ".format(seats))
    print("****** Reserving first seats {0} **** ".format(seats[0]))
    reply = execute_command(socket, "bookSeats", ["mark", movie, theater, [seats[0],]])
    print("****** Reserved seats: {0} **** ".format(reply))

    reply = execute_command(socket, "getAvailableSeats", [movie, theater])
    if not reply:
        print("NO REPLY")
        return
    else:
        print("GOT REPLY: {0}".format(reply))



# 1. Create a ZeroMQ context
context = zmq.Context()

# 2. Create a REQ (Request) socket
socket = context.socket(zmq.REQ)

# 3. Connect to the server
socket.connect("tcp://localhost:52345")


# for i in range(3):
run_all(socket)


print("Bye!")
