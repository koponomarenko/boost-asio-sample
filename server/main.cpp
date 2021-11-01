#include <iostream>

#include <boost/asio.hpp>

using namespace boost;

int main()
{
    asio::ip::address ip_addr = asio::ip::address_v6::any();
    unsigned short port_num = 3333;

    asio::ip::tcp::endpoint ep {ip_addr, port_num};
}
