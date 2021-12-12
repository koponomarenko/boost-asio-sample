#include <algorithm>
#include <iostream>
#include <memory>
#include <string>

#include <boost/asio.hpp>

using namespace boost;

// hardcode fixed msg size for simplicity
constexpr int msg_size = 100;

void log(const std::string& s)
{
    std::cout << "log: " << s << std::endl;
}

struct Session {
    std::shared_ptr<asio::ip::tcp::socket> socket;
    std::vector<char> buffer;
};

// this is invoked when all data is writen or an error occurs
void async_write_handler(const boost::system::error_code& ec,
                         std::size_t bytes_transferred,
                         std::shared_ptr<Session> session)
{
    if (ec.value() != 0) {
        std::cerr << "ERROR: in async_write_handler. Error code " << ec.value()
                  << ". Message: " << ec.message() << std::endl;
        return;
    }
    log("msg was sent");
}

std::vector<char> transform_to_msg(const std::string& s)
{
    std::vector<char> msg;
    msg.resize(msg_size, '\0');
    int len = (s.size() < msg_size) ? s.size() : msg_size;
    std::copy(s.begin(), s.begin() + len, msg.begin());
    return msg;
}

int main()
try {
    asio::ip::address server_ip_addr{asio::ip::address::from_string("127.0.0.1")};
    unsigned short port_num{33'333};

    asio::ip::tcp::endpoint server_ep{server_ip_addr, port_num};

    asio::io_context io_context;
    auto socket{std::make_shared<asio::ip::tcp::socket>(io_context, server_ep.protocol())};
    // socket.bind(local_endpoint) is done implicitly by socket.connect()
    socket->connect(server_ep);

    std::string text{"Hello there! It actually works! =)"};
    auto session{std::make_shared<Session>()};
    session->socket = socket;
    session->buffer = transform_to_msg(text);

    log("do async_write");
    asio::async_write(
        *socket, asio::buffer(session->buffer, msg_size),
        std::bind(async_write_handler, std::placeholders::_1, std::placeholders::_2, session));

    log("wait for async_write finish");
    // all handlers are (waited for and) called in this call
    io_context.run();
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
    asio::ip::tcp::resolver resolver{io_context};
    auto results = resolver.resolve(hostname, port_num);
    for (asio::ip::tcp::endpoint const& endpoint : results) {
        std::cout << endpoint << std::endl;
    }
}
