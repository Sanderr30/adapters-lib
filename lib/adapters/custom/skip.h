#pragma once


class Skip {
public:
    Skip(size_t count) : count_(count) {}

    template <typename Range>
    auto operator()(Range&& range) const {
        auto start = begin(range);
        auto finish = end(range);

        size_t skipped = 0;
        while (start != finish && skipped < count_) {
            ++start;
            ++skipped;
        }

        struct Wrapper {
            decltype(start) begin_;
            decltype(finish) end_;

            auto begin() const { return begin_; }
            auto end() const { return end_; }
        };

        return Wrapper{start, finish};
    }

private:
    size_t count_;
};


template <typename Range>
auto operator|(Range&& range, const Skip& skip) {
    return skip(std::forward<Range>(range));
}