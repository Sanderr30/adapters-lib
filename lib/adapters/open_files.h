#pragma once

#include <fstream>
#include <filesystem>
#include <utility>


class OpenFiles {
public:
    template<typename Range>
    class iterator {
    public:
        using Baseiterator = decltype(std::declval<Range>().begin());
        using value_type = std::ifstream;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::input_iterator_tag;

    public:
        iterator(Baseiterator current, Baseiterator end) : current_(std::move(current)), 
                                                                end_(std::move(end)) {
            OpenNext();
        }

        iterator(const iterator& other) : current_(other.current_), 
                                                        end_(other.end_) {
            if (other.current_stream_.is_open()) {
                current_stream_.open((*current_).string());
            }
        }

        iterator(iterator&& other) noexcept : current_(std::move(other.current_)),
                                                end_(std::move(other.end_)),
                                                current_stream_(std::move(other.current_stream_)) {
        }

        iterator& operator=(const iterator& other) {
            if (this != &other) {
                current_ = other.current_;
                end_ = other.end_;
                if (other.current_stream_.is_open()) {
                    current_stream_.open((*current_).string());
                } else {
                    current_stream_.close();
                }
            }
            return *this;
        }

        iterator& operator=(iterator&& other) noexcept {
            current_ = std::move(other.current_);
            end_ = std::move(other.end_);
            current_stream_ = std::move(other.current_stream_);
            return *this;
        }

        ~iterator() = default;

    public:
        reference operator*() { return current_stream_; }

        pointer operator->() { return &current_stream_; }

        iterator& operator++() {
            ++current_;
            OpenNext();
            return *this;
        }

        iterator operator++(int) {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const iterator& other) const {
            return current_ == other.current_;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

    private:
        void OpenNext() {
            current_stream_.close();
            while (current_ != end_) {
                const std::filesystem::path& path = *current_;
                current_stream_.open(path.string());
                if (current_stream_.is_open()) {
                    break;
                } else {
                    ++current_;
                }
            }
            if (current_ == end_) {
                current_stream_.close();
            }
        }

    private:
        Baseiterator current_;
        Baseiterator end_;
        std::ifstream current_stream_;
    };

public:
    OpenFiles() = default;

    OpenFiles(const Dir& dir) : dir_(dir) {}

    template<typename Range>
    auto begin(const Range& range) const {
        return iterator<Range>(range.begin(), range.end());
    }

    template<typename Range>
    auto end(const Range& range) const {
        return iterator<Range>(range.end(), range.end());
    }

    template<typename Range>
    auto operator()(Range&& range) const {
        struct Wrapper {
            using iterator = typename OpenFiles::iterator<Range>;

            Range range_;

            auto begin() const { return iterator(range_.begin(), range_.end()); }

            auto end() const { return iterator(range_.end(), range_.end()); }
        };
        return Wrapper{std::forward<Range>(range)};
    }

private:
    Dir dir_;
};


template<typename Range>
auto operator|(Range&& range, const OpenFiles& open_files) {
    return open_files(std::forward<Range>(range));
}