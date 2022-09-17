#ifndef HEY_WATCH_H
#define HEY_WATCH_H

#include <boost/asio/completion_condition.hpp>

namespace hey {
    template<class ProcessedSignal, class CompletionCondition>
    auto watch(ProcessedSignal &&signal, CompletionCondition &&condition) {
        return [signal = BOOST_ASIO_MOVE_CAST(ProcessedSignal)(signal),
                condition = BOOST_ASIO_MOVE_CAST(CompletionCondition)(condition)]
                (const auto &err, std::size_t bytes_transferred)mutable {
            signal(bytes_transferred);
            return condition(err, bytes_transferred);
        };
    }

    template<class ProcessedSignal>
    auto watch(ProcessedSignal &&signal) {
        return watch(BOOST_ASIO_MOVE_CAST(ProcessedSignal)(signal), boost::asio::transfer_all());
    }
}

#endif //HEY_WATCH_H
