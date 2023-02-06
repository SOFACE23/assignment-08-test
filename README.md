# SOFACE-assignment-8

**It is strongly recommended to use the VM for this assignment, due to the difficulty of installing third-party packages on non-unix systems.**

During this exercise you will implement a simple peer-to-peer chat program that allows you to send simple text messages to other clients on your network.
The exercise is split into two parts:

1. warmup questions
2. modifying existing program.

Note that both of these are mandatory parts of the assignment.

## Installing ZMQ

There are several approaches to install C/C++ packages on an operating system.
One of these is downloading the source files and building compiling the project yourself.
Two advantages of installing from source are:

1.  You get the most recent version of the library
2.  It does not rely on package managers that may break

To install zmq from source run the following commands:
(May vary with newer versions, check the github for updated install commands)

```bash
# install libzmq from source
git clone https://github.com/zeromq/libzmq.git
cd libzmq
mkdir build
cd build
cmake ..
cmake --build .
sudo cmake install .

# install cppzmq, (C++ bindings to libzmq)
git clone https://github.com/zeromq/cppzmq
cd cppzmq
mkdir build
cd build
cmake ..
cmake --build .
sudo cmake install .
```

After running these commands both zmq and cppzmq should be installed on your system.

## Exercise

### Discussion: Network Architectures

Create a file `discussion.md` in the repo and answer the following questions:

1. What is an IP address? (100 words)

   - Why do we need IP addresses?
   - What is the relation between TCP and IP?
   - What is a socket? Have you heard of different types of sockets?
   - What is up with the address: 127.0.0.1?

2. What does a client-server architecture mean? (150 words)

   - Give an concrete example of where you would encounter this in real life.
   - How do you decide who is the server and who is the client?

3. What is the difference between client-server architecture and the broker architecture? (100 words)

   - What are the benefits to the broker pattern?
   - What could be potential downsides?

4. What does Peer-to-Peer mean? (150 words)
   - What are the main characteristics of this architecture?
   - Have you encountered it anywhere online?
   - Does Peer-to-Peer applications mean that there are no servers and only clients?
   - What benefitits could there be to using this pattern?
   - What downsides could there be to this pattern?

### Programming: Chat Program

<img src="docs/peer_to_peer_chat.svg" width=60%>

1. Inspect the following files `chat_client.cpp` and `chat_server.cpp`.
   Try to relate the concepts to the image above.
   Focus on the high level structure like where data structures are initialized and where the reading and writing to the socket occurs.

2. Compile and programs and start 1 server and two clients, as shown below:

   ```bash
   ./chat_server 127.0.0.1
   ```

   Then start the first client in a new terminal:

   ```bash
   ./chat_client 127.0.0.1 tarzan 8000
   ```

   Start the second client in a new terminal:

   ```bash
   ./chat_client 127.0.0.1 clayton 9000
   ```

   The first argument `127.0.0.1` is the ip address of the server.
   The client program accepts two additional arguments, the first being the name of the client and the second being the port number used by the specific client.

   The output in the server terminal should be:

   ```bash
   binding welcome socket to address: tcp://127.0.0.1:5000
   awaiting registration of chat clients
   received handshake from: 'clayton', with endpoint: 'tcp://127.0.0.1:8000'
   received handshake from: 'tarzan', with endpoint: 'tcp://127.0.0.1:9000'
   ```

   Note: To enable chat between multiple computers you could start the server on a computer that is accessible by all computers on the network and chage the argument appropriately **(you dont have to do this)**.

3. Try to send a message from Clayton to Tarzan by running typing the following into Clayton's terminal:

   ```bash
   tarzan,Hi meet me in the dark woods <3 - Jane!
   ```

   Clayton's terminal:

   ```bash
   tarzan,hi meet me in the dark woods <3 - Jane
   address of recipent: 'tarzan' is not known to the client, asking server
   server resolved address of: 'tarzan, address is: 'tcp://127.0.0.1:9000'
   sending message: 'hi meet me in the dark woods <3 - Jane' to: 'tarzan'
   ```

   Tarzan's terminal should show:

   ```bash
   ...
   recieved message: 'hi meet me in the dark woods <3 - Jane'
   ```

4. Clearly, Clayton is trying to lure Tarzan into a trap. Modifiy the program `chat_client.cpp` program such that the name of the sender is shown when recieving the message.
   After modifying the program the message recieved by Tarzan should look like:
   ```bash
   ...
   recieved message from 'clayton': 'hi meet me in the dark woods <3 - Jane'
   ```
