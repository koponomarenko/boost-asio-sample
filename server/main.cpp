#include <iostream>
#include <vector>

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
    std::vector<uint8_t> buffer;
};

// this is invoked when all data is writen or an error occurs
void async_read_handler(const boost::system::error_code& ec,
                        std::size_t bytes_transferred,
                        std::shared_ptr<Session> session)
{
    if (ec.value() != 0) {
        std::cerr << "ERROR: in async_read_handler. Error code " << ec.value()
                  << ". Message: " << ec.message() << std::endl;
        return;
    }

    std::string msg{session->buffer.begin() + msg_header_size, session->buffer.end()};
    std::cout << "received msg: " << msg << std::endl;
}

// this is invoked when all data is writen or an error occurs
void async_read_msg_header_handler(const boost::system::error_code& ec,
                                   std::size_t bytes_transferred,
                                   std::shared_ptr<Session> session)
{
    if (ec.value() != 0) {
        std::cerr << "ERROR: in async_read_handler. Error code " << ec.value()
                  << ". Message: " << ec.message() << std::endl;
        return;
    }

    uint16_t msg_size{};
    memcpy(&msg_size, session->buffer.data(), msg_header_size);
    endian::big_to_native_inplace(msg_size);

    std::cout << "session->buffer.size(): " << session->buffer.size() << std::endl;
    std::cout << "session->buffer[0]: " << static_cast<uint16_t>(session->buffer[0]) << std::endl;
    std::cout << "session->buffer[1]: " << static_cast<uint16_t>(session->buffer[1]) << std::endl;
    std::cout << "received msg size: " << msg_size << std::endl;

    session->buffer.resize(msg_header_size + msg_size);
    asio::async_read(
        *session->socket, asio::buffer(session->buffer),
        std::bind(async_read_handler, std::placeholders::_1, std::placeholders::_2, session));
}

int main()
try {
    asio::ip::address ip_addr{asio::ip::address_v4::any()};
    unsigned short port_num{33'333};

    asio::ip::tcp::endpoint ep{ip_addr, port_num};

    asio::io_context io_context;
    asio::ip::tcp::acceptor acceptor{io_context, ep.protocol()};
    acceptor.bind(ep);

    const int pending_requests_queue_size{50};
    acceptor.listen(pending_requests_queue_size);

    auto socket = std::make_shared<asio::ip::tcp::socket>(io_context);
    log("wait for the client");
    acceptor.accept(*socket);

    log("client connected");
    auto session{std::make_shared<Session>()};
    session->socket = socket;
    session->buffer.resize(msg_header_size);

    log("do async_read");
    asio::async_read(*socket, asio::buffer(session->buffer, msg_header_size),
                     std::bind(async_read_msg_header_handler, std::placeholders::_1,
                               std::placeholders::_2, session));

    log("wait for async_read finish");
    // all handlers are (waited for and) called in this call
    io_context.run();
}
catch (std::exception& e) {
    std::cerr << "std::exception: " << e.what() << std::endl;
}
catch (...) {
    std::cerr << "Unknown exception" << std::endl;
}