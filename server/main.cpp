#include <iostream>
#include <vector>

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
void async_read_handler(const boost::system::error_code& ec,
                        std::size_t bytes_transferred,
                        std::shared_ptr<Session> session)
{
    if (ec.value() != 0) {
        std::cerr << "ERROR: in async_read_handler. Error code " << ec.value()
                  << ". Message: " << ec.message() << std::endl;
        return;
    }

    std::string msg{session->buffer.begin(), session->buffer.end()};
    std::cout << "received msg: " << msg << std::endl;
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
    session->buffer.resize(msg_size, 0);

    log("do async_read");
    asio::async_read(
        *socket, asio::buffer(session->buffer, msg_size),
        std::bind(async_read_handler, std::placeholders::_1, std::placeholders::_2, session));

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