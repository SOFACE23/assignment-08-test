#include <zmq.hpp>
#include <string>
#include <map>
#include <iostream>
#include <mutex>
#include <thread>

zmq::context_t ctx;

// store mapping from name to endpoint, e.g.
// Jane -> 127.0.0.1:1245
// Josh -> 127.0.0.1:5678
std::map<std::string,zmq::socket_t> name_to_socket;
std::mutex name_to_socket_lock;

void send_func(std::string whereis_endpoint)
{   

    zmq::socket_t sock(ctx,zmq::socket_type::req);
    zmq::message_t request;

    while(true)
    {
        std::string msg;
        std::getline(std::cin,msg);

        auto split = msg.find(",");
        std::string recipient = msg.substr(0, split);
        std::string text = msg.substr(split + 1);
        bool recipient_located = true;

        // check if we already have an connection to the recipient
        // if not inquire the server for the address of the recipient
        auto maybe_socket = name_to_socket.find(recipient);
        if (maybe_socket == name_to_socket.end())
        {
            std::cout << "address of recipent: '" << recipient << "' is not known to the client, asking server" << std::endl;
            
            sock.connect(whereis_endpoint);
            sock.send(zmq::buffer(recipient));
            auto _ = sock.recv(request);

            auto recipient_endpoint = request.to_string();

            // if successfull store the client name and the now connected socket
            if (recipient_endpoint == "")
            {
                std::cerr << "unable to send message to recipient, server was unable to find address. Ensure that client is already started" << std::endl;
                recipient_located = false;

            }
            else
            {
                std::cout << "server resolved address of: '" << recipient << ", address is: '" << recipient_endpoint << "' " << std::endl;
                zmq::socket_t sock(ctx,zmq::socket_type::push);
                sock.connect(recipient_endpoint);
                std::scoped_lock(name_to_socket_lock);
                name_to_socket.emplace(recipient,std::move(sock));
                
            }
        }
        
        if (recipient_located)
        {
            maybe_socket = name_to_socket.find(recipient);
            std::cout << "sending message: '" << text << "' to: '" << recipient << "'" << std::endl; 
            maybe_socket->second.send(zmq::buffer(text));
        }
    }
    
}

void recv_func(std::string endpoint)
{
    zmq::socket_t sock(ctx,zmq::socket_type::pull);
    sock.bind(endpoint);
    zmq::message_t msg;
    

    while(true)
    {
        auto _ = sock.recv(msg);
        std::cout << "recieved message: '" << msg.to_string() << "'" << std::endl;
    }
}

int main(int argc, char **argv)
{

    if (argc != 4)
    {
        std::cerr << "incorrect number of arguments specified when launching server. Ensure that you specifiy the servers endpoint and the name of the client, for example: 127.0.0.1 jane 8000" << std::endl;
        exit(1);
    }
    std::string ip = std::string(argv[1]);
    std::string name = std::string(argv[2]);
    std::string recv_port = std::string(argv[3]);
    std::string server_register_client_endpoint = "tcp://" + ip + ":5000";
    std::string server_whereis_client_endpoint = "tcp://" + ip + ":5001";

    std::string recv_endpoint = "tcp://" + ip + ":" + recv_port;

    // register the client with the server, such that other clients can get in touch
    std::cout << "attempting to connect to server with endpoint: " << server_register_client_endpoint << std::endl;
    zmq::socket_t sock(ctx, zmq::socket_type::push);

    sock.connect(server_register_client_endpoint);
    std::cout << "registering client with server, using name: '" << name << "' and endpoint: '" << recv_endpoint << "'" << std::endl; 

    std::string msg = name + "," + recv_endpoint;
    sock.send(zmq::buffer(msg), zmq::send_flags::dontwait);
    std::cout << "client successfully registered" << std::endl;
    std::cout << "to send a message type a message of the form: 'recipient,message' and then press enter" << std::endl;

    std::thread send_thread(send_func,server_whereis_client_endpoint);
    std::thread recv_thread(recv_func,recv_endpoint);

    send_thread.join();
    recv_thread.join();
}