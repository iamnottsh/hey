#include <boost/asio.hpp>
#include <iostream>

int main() {
    boost::asio::io_context io;
    io.run();
}
