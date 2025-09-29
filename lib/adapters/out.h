#pragma once

#include <fstream>
#include <sstream>


class Out {
public:
    Out(std::ostream& output_stream) : output_stream_(output_stream) {}

    template<typename Range>
    void operator()(Range&& range) const {
        for (const auto& item : range) {
            output_stream_ << item << "\n";
        }
        output_stream_.flush();
    }

private:
    std::ostream& output_stream_;
};


template<typename Range>
auto operator|(Range&& range, const Out& out) {
    out(std::forward<Range>(range));
}