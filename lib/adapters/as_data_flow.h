#pragma once

#include <optional>
#include <fstream>


template<typename Container>
class AsDataFlow {
public:
	using value_type = typename Container::value_type;

public:
    class iterator {
    public:
        using value_type = typename Container::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::input_iterator_tag;

        using DataFlowiterator = typename Container::const_iterator;

    public:
        iterator() = default;

        iterator(DataFlowiterator current, DataFlowiterator end) : current_(current), 
                                                                    end_(end) {
        }

        iterator(const iterator& other) : current_(other.current_), 
                                            end_(other.end_) { 
        }

        iterator(iterator&& other) noexcept : current_(std::move(other.current_)),
                                                end_(std::move(other.end_)) { 
        }

        iterator& operator=(const iterator& other) {
            if (this == &other) {
                return *this;
            }
            current_ = other.current_;
            end_ = other.end_;
            return *this;
        }

        iterator& operator=(iterator&& other) noexcept {
            if (this == &other) {
                return *this;
            }
            current_ = std::move(other.current_);
            end_ = std::move(other.end_);
            return *this;
        }

        ~iterator() = default;

    public:
        reference operator*() const { return *current_; }

        pointer operator->() const { return &(*current_); }

        iterator& operator++() {
            ++current_;
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
        DataFlowiterator current_;
        DataFlowiterator end_;
    };

public:
    AsDataFlow(Container& container) : container_(container) {}

    auto begin() const { return iterator(container_.begin(), container_.end()); }

    auto end() const { return iterator(container_.end(), container_.end()); }

private:
    Container& container_;
};