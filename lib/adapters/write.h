#pragma once

#include <fstream>


class Write {
public:
    Write(std::ostream& output_stream, char delimiter) : 
            output_stream_(output_stream), delimiter_(delimiter) {	
    }

    template<typename Range>
    auto operator()(Range&& range) const {
        for (const auto& item : range) {
            output_stream_ << item << delimiter_;
        }
        output_stream_.flush();
        return std::forward<Range>(range);
    }

private:
    std::ostream& output_stream_;
    char delimiter_;
};


template<typename Range>
auto operator|(Range&& range, const Write& write) {
    return write(std::forward<Range>(range));
}   