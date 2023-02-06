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
std::map<std::string,std::string> name_to_endpoint;
std::mutex name_to_endpoint_lock;

void register_func(std::string endpoint)
{   
    std::cout << "binding welcome socket to address: " << endpoint << std::endl;

    
    zmq::socket_t sock(ctx, zmq::socket_type::pull);
    sock.bind(endpoint);

    
    std::cout << "awaiting registration of chat clients" << std::endl;
    while (true)
    {
        zmq::message_t request;

        // receive a request from client
        auto res = sock.recv(request, zmq::recv_flags::none);

        // split string into name and endpoint, comma is used as delimiter
        // Jane,127.0.0.1:6000
        std::string s = request.to_string();
        std::string delimiter = ",";
        auto split = s.find(delimiter);
        std::string name = s.substr(0, split);
        std::string endpoint = s.substr(split + 1);

        std::cout << "received handshake from: '" << name << "', with endpoint: '" << endpoint << "'" << std::endl;

        // store name and associated endpoint
        // here it is necessary to lock
        {
            std::scoped_lock sl(name_to_endpoint_lock);
            name_to_endpoint[name] = endpoint;
        }
        
        // req/rep pattern requires that a reply is every time an request is made
        // in this case we simply tell the client that everything is "ok"
        // std::string_view msg = "ok";
        // sock.send(zmq::buffer(msg),zmq::send_flags::dontwait);
    }
}

// function run by thread responsible for resolving the address of chat clients
void whereis_func(std::string endpoint)
{
    zmq::socket_t sock(ctx, zmq::socket_type::rep);
    sock.bind(endpoint);

    while(true)
    {
        zmq::message_t request;
        auto res = sock.recv(request);

        auto recipient_name = request.to_string();
        auto maybe_recipient_address = name_to_endpoint.find(recipient_name);

        if (maybe_recipient_address != name_to_endpoint.end())
        {
            auto address = maybe_recipient_address->second;
            std::cout << "resolved address of: '" << request.to_string() << "', address is: '" << address << std::endl;
            sock.send(zmq::buffer(address));
        }
        else
        {
            std::cout << "unable to find client: '" << request.to_string() <<"'" << std::endl;
            std::string empty_msg = "";
            sock.send(zmq::buffer(empty_msg)); // indicate address not found
        }
        

    }
}

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        std::cerr << "incorrect number of arguments specified when launching server. Ensure that you specifiy the servers endpoint, for example: 127.0.0.1:5000" << std::endl;
        exit(1);
    }
    std::string ip = std::string(argv[1]);
    std::string register_endpoint = "tcp://" + ip + ":5000";
    std::string whereis_endpoint = "tcp://" + ip + ":5001";

    // this thread handles new clients joining
    std::thread register_worker(register_func, register_endpoint);

    // this thread allows clients to look up the address of other clients
    std::thread whereis_worker(whereis_func, whereis_endpoint);

    register_worker.join();
    whereis_worker.join();
}