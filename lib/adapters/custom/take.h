#pragma once


class Take {
public:
    Take(size_t count) : count_(count) {}

    template <typename Range>
    auto operator()(Range&& range) const {
        auto start = begin(range);
        auto finish = end(range);
        
        size_t taken = 0;
        while (start != finish && taken < count_) {
            ++start;
            ++taken;
        }

        struct Wrapper {
            decltype(start) begin_;
            decltype(finish) end_;

            auto begin() const { return begin_; }
            auto end() const { return end_; }
        };

        return Wrapper{begin(range), start};
    }

private:
    size_t count_;
};


template <typename Range>
auto operator|(Range&& range, const Take& take) {
    return take(std::forward<Range>(range));
}
