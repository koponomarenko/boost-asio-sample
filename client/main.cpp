#include <iostream>
#include <memory>
#include <string>

#include <boost/asio.hpp>

using namespace boost;

struct Session {
    std::shared_ptr<asio::ip::tcp::socket> sock;
    std::string buf;
};

// this is invoked when all data is writen or an error occurred.
void callback(const boost::system::error_code& ec,
              std::size_t bytes_transferred,
              std::shared_ptr<Session> session)
{
    if (ec.value() != 0) {
        std::cerr << "Error in callback. Error code value = " << ec.value()
                  << ". Message: " << ec.message() << std::endl;
        return;
    }
}

int main()
try {
    asio::ip::address server_ip_addr = asio::ip::address::from_string("127.0.0.1");
    unsigned short port_num = 33'333;
    asio::ip::tcp::endpoint server_ep {server_ip_addr, port_num};

    asio::io_context io_context;
    std::shared_ptr<asio::ip::tcp::socket> socket {
        std::make_shared<asio::ip::tcp::socket>(io_context, server_ep.protocol())};
    // socket.bind(local_endpoint) is done implicitly by socket.connect()
    socket->connect(server_ep);

    std::string msg {"Hello there!"};
    std::shared_ptr<Session> session {std::make_shared<Session>()};
    session->sock = socket;
    session->buf = msg;

    asio::async_write(*socket, asio::buffer(session->buf),
                      std::bind(callback, std::placeholders::_1, std::placeholders::_2, session));
}
catch (std::exception& e) {
    std::cerr << "std::exception: " << e.what() << std::endl;
}
catch (...) {
    std::cerr << "Unknown exception" << std::endl;
}

// =====================================================================
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
