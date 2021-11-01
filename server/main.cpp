#include <iostream>

#include <boost/asio.hpp>

using namespace boost;

int main()
try {
    asio::ip::address ip_addr = asio::ip::address_v4::any();
    unsigned short port_num = 33'333;
    asio::ip::tcp::endpoint ep {ip_addr, port_num};

    asio::io_context io_context;
    asio::ip::tcp::acceptor acceptor {io_context, ep.protocol()};
    acceptor.bind(ep);

    const int pending_requests_queue_size = 50;
    acceptor.listen(pending_requests_queue_size);

    asio::ip::tcp::socket socket {io_context};
    acceptor.accept(socket);
}
catch (std::exception& e) {
    std::cerr << "std::exception: " << e.what() << std::endl;
}
catch (...) {
    std::cerr << "Unknown exception" << std::endl;
}