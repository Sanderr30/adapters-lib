#pragma once


class DropNullopt {
public:
    template <typename Range>
    class iterator {
    public:
        using value_type = typename Range::iterator::value_type::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::input_iterator_tag;

        using Baseiterator = typename Range::iterator;

    public:
        iterator() = default;

        iterator(Baseiterator current, Baseiterator end) : current_(std::move(current)), 
                                                                end_(std::move(end)) {
            ToNextValid();
        }
        
        iterator(const iterator& other) : current_(other.current_),
                                            end_(other.end_) {
        }

        iterator(iterator&& other) noexcept : current_(std::move(other.current_)),
                                                end_(std::move(other.end_)) {
        }

        iterator& operator=(const iterator& other) {
            if (this != &other) {
                current_ = other.current_;
                end_ = other.end_;
            }
            return *this;
        }

        iterator& operator=(iterator&& other) noexcept {
            if (this != &other) {
                current_ = std::move(other.current_);
                end_ = std::move(other.end_);
            }
            return *this;
        }

        ~iterator() = default;

    public:

        reference operator*() const { return current_->value(); }

        pointer operator->() const { return &current_->value(); }

        iterator& operator++() {
            ++current_;
            ToNextValid();
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const { return current_ == other.current_; }

        bool operator!=(const iterator& other) const { return !(*this == other); }

    private:
        void ToNextValid() {
            while (current_ != end_ && !current_->has_value()) {
                ++current_;
            }
        }

    private:
        Baseiterator current_;
        Baseiterator end_;
    };

public:
    template <typename Range>
    auto operator()(Range&& range) const {
        struct Wrapper {
            using iterator = typename DropNullopt::iterator<Range>;

            Range range_;

            auto begin() const { return iterator(range_.begin(), range_.end()); }

            auto end() const { return iterator(range_.end(), range_.end()); }
        };

        return Wrapper{std::forward<Range>(range)};
    }
};


template <typename Range>
auto operator|(Range&& range, const DropNullopt& drop_nullopt) {
    return drop_nullopt(std::forward<Range>(range));
}