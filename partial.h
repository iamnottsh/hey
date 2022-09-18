#ifndef HEY_PARTIAL_H
#define HEY_PARTIAL_H

#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/stream_file.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <istream>

namespace hey {
    template<class T>
    concept partial_handler = requires(T &&x, const std::string &l, std::size_t n) {
        { x.line(l) };
        { x.header(l) };
        { x.read(n) };
        { x.write(n) };
    };

    template<class AsyncStream, partial_handler PartialHandler>
    boost::asio::awaitable<void> partial(auto &stream,
                                         const std::string &request,
                                         int64_t begin,
                                         std::size_t size,
                                         const std::string &path,
                                         PartialHandler &&handler) {
        co_await boost::asio::async_write(stream,
                                          boost::asio::buffer(request +
                                                              "Range: bytes=" +
                                                              std::to_string(begin) +
                                                              '-' +
                                                              std::to_string(begin + size - 1) +
                                                              "\r\n\r\n"
                                          ),
                                          boost::asio::use_awaitable);
        boost::asio::streambuf buf;
        auto bytes_remain = co_await boost::asio::async_read_until(stream,
                                                                   buf,
                                                                   "\r\n\r\n",
                                                                   boost::asio::use_awaitable);
        {
            std::istream is(&buf);
            std::string line;
            std::getline(is, line, '\r');
            is.get();
            bytes_remain -= line.size() + 2;
            handler.line(line);
            for (;;) {
                line.clear();
                std::getline(is, line, '\r');
                is.get();
                bytes_remain -= line.size() + 2;
                if (line.empty()) break;
                handler.header(line);
            }
        }
        size -= bytes_remain;
        handler.read(bytes_remain);
        boost::asio::stream_file file(stream.get_executor(),
                                      path,
                                      boost::asio::file_base::write_only | boost::asio::file_base::create);
        file.seek(begin, boost::asio::file_base::seek_set);
        while (handler.write(co_await boost::asio::async_write(stream, buf, boost::asio::use_awaitable)), size) {
            const auto bytes_transferred = co_await stream.async_read_some(buf.prepare(std::min(size,
                                                                                                std::max(512uz,
                                                                                                         buf.capacity() -
                                                                                                         buf.size()))),
                                                                           boost::asio::use_awaitable);
            buf.commit(bytes_transferred);
            size -= bytes_transferred;
            handler.read(bytes_transferred);
        }
    }
}

#endif //HEY_PARTIAL_H
