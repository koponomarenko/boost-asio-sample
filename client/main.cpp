#include <iostream>

#include <boost/asio.hpp>

using namespace boost;

int main()
try {
    asio::ip::address server_ip_addr = asio::ip::address::from_string("127.0.0.1");
    unsigned short port_num = 33'333;
    asio::ip::tcp::endpoint server_ep {server_ip_addr, port_num};

    asio::io_context io_context;
    asio::ip::tcp::socket socket {io_context, server_ep.protocol()};
    // socket.bind(local_endpoint) is done implicitly by socket.connect()
    socket.connect(server_ep);
}
catch (std::exception& e) {
    std::cerr << "std::exception: " << e.what() << std::endl;
}
catch (...) {
    std::cerr << "Unknown exception" << std::endl;
}

// just a sample
auto resolve_dns_name()
{
    std::string hostname = "localhost";
    std::string port_num = "33333";
    asio::io_context io_context;
    asio::ip::tcp::resolver resolver {io_context};
    auto results = resolver.resolve(hostname, port_num);
    for (asio::ip::tcp::endpoint const& endpoint : results) {
        std::cout << endpoint << std::endl;
    }
}
