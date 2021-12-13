#include <algorithm>
#include <iostream>
#include <memory>
#include <string>

#include <boost/asio.hpp>
#include <boost/endian/conversion.hpp>

using namespace boost;

constexpr int msg_header_size = 2;

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

    // insert msg header (msg size info)
    uint16_t msg_size{static_cast<uint16_t>(s.size())};
    std::cout << "msg_size: " << msg_size << std::endl;
    endian::native_to_big_inplace(msg_size);

    char* tmp = reinterpret_cast<char*>(&msg_size);
    std::cout << "tmp[0]: " << static_cast<uint16_t>(tmp[0]) << std::endl;
    std::cout << "tmp[1]: " << static_cast<uint16_t>(tmp[1]) << std::endl;

    std::copy(reinterpret_cast<char*>(&msg_size),
              reinterpret_cast<char*>(&msg_size + sizeof(msg_size)), std::back_inserter(msg));

    std::copy(s.begin(), s.end(), std::back_inserter(msg));

    std::cout << "msg[0]: " << static_cast<uint16_t>(msg[0]) << std::endl;
    std::cout << "msg[1]: " << static_cast<uint16_t>(msg[1]) << std::endl;

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
        *socket, asio::buffer(session->buffer),
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
