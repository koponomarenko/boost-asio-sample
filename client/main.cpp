#include <iostream>

#include <boost/asio.hpp>

using namespace boost;

int main()
{
    std::string raw_ip_addr = "127.0.0.1";
    unsigned short port_num = 3333;

    boost::system::error_code ec;
    asio::ip::address ip_addr = asio::ip::address::from_string(raw_ip_addr, ec);
    if (ec.value() != 0) {
        std::cout << "Failed to parse the IP address. Error code = " << ec.value()
                  << ". Message: " << ec.message();
        return ec.value();
    }

    asio::ip::tcp::endpoint ep {ip_addr, port_num};
}
