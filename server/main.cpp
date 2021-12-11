#include <iostream>
#include <vector>

#include <boost/asio.hpp>

using namespace boost;

struct Session {
    std::shared_ptr<asio::ip::tcp::socket> sock;
    std::vector<char> buf;
    const int buf_size {100};
};

// this is invoked when all data is writen or an error occurred.
void handler(const boost::system::error_code& ec,
             std::size_t bytes_transferred,
             std::shared_ptr<Session> session)
{
    if (ec.value() != 0) {
        std::cerr << "Error in callback. Error code value = " << ec.value()
                  << ". Message: " << ec.message() << std::endl;
        return;
    }

    std::string msg {session->buf.begin(), session->buf.begin() + bytes_transferred};
    std::cout << "received msg: " << msg << std::endl;
}

void log(const std::string& s) {
    std::cout << "log: " << s << std::endl;
}

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

    asio::io_context io_context2; // tmp
    auto socket = std::make_shared<asio::ip::tcp::socket>(io_context);
    log("wait for the client");
    acceptor.accept(*socket);
    log("client connected");

    auto session {std::make_shared<Session>()};
    session->sock = socket;
    session->buf.resize(session->buf_size, '\0');

    log("do async_read");
    asio::async_read(*socket, asio::buffer(session->buf, 11),
                     std::bind(handler, std::placeholders::_1, std::placeholders::_2, session));

    //    char buf2[100] = {};
    //    socket.read_some(asio::buffer(buf2, 20));
    //    std::cout << "received msg: " << buf2 << std::endl;
    io_context.run();
    log("wait for async_read");
    std::this_thread::sleep_for(std::chrono::seconds {10});
}
catch (std::exception& e) {
    std::cerr << "std::exception: " << e.what() << std::endl;
}
catch (...) {
    std::cerr << "Unknown exception" << std::endl;
}