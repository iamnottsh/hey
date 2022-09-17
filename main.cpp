#include <boost/asio.hpp>
#include <iostream>
#include "watch.h"

int main() {
    boost::asio::io_context io;
    boost::asio::stream_file file(
            io,
            "D:/123.txt",
            boost::asio::file_base::write_only | boost::asio::file_base::create);
    std::string b(1000000, {});
    std::generate(b.begin(), b.end(), []{
        static char p{};
        return ++p;
    });
    boost::asio::write(file, boost::asio::buffer(b), hey::watch([](std::size_t b){
        std::cout << b << std::endl;
    }));
    io.run();
}
