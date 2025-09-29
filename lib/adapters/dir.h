#pragma once

#include <filesystem>
#include <variant>


class Dir {
public:
    using iterator_type = std::filesystem::directory_iterator;
    using recursive_iterator_type = std::filesystem::recursive_directory_iterator;

public:
    class iterator {
    public:
        using value_type = std::filesystem::path;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::input_iterator_tag;

    public:
        iterator() = default;

        iterator(const Dir& dir, bool is_end) {
            if (is_end) {
                InitializeEnd(dir.is_recursive_);
            } else {
                InitializeBegin(dir.file_path_, dir.is_recursive_);
            }
        }

        iterator(const iterator& other) : iterator_(other.iterator_),
                                            current_path_(other.current_path_) {
        }

        iterator(iterator&& other) noexcept : iterator_(std::move(other.iterator_)),
                                                current_path_(std::move(other.current_path_)){
        }

        iterator& operator=(const iterator& other) {
            if (this != &other) {
                iterator_ = other.iterator_;
                current_path_ = other.current_path_;
            }
            return *this;
        }

        iterator& operator=(iterator&& other) noexcept {
            if (this != &other) {
                iterator_ = std::move(other.iterator_);
                current_path_ = std::move(other.current_path_);
            }
            return *this;
        }

        ~iterator() = default;

    public:
        reference operator*() { return current_path_; }

        pointer operator->() { return &current_path_; }
        
        iterator& operator++() {
            std::visit([this](auto& iter) {
                ++iter;
                UpdateCurrentPath(iter);
            }, iterator_);
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const {
            return iterator_.index() == other.iterator_.index() && iterator_ == other.iterator_;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

    private:
        void InitializeBegin(const std::filesystem::path& pth, bool is_recursive) {
            if (is_recursive) {
                recursive_iterator_type it(pth);
                iterator_ = it;
                UpdateCurrentPath(it);
            } else {
                iterator_type it(pth);
                iterator_ = it;
                UpdateCurrentPath(it);
            }
        }

        void InitializeEnd(bool is_recursive) {
            if (is_recursive) {
                iterator_ = recursive_iterator_type();
            } else {
                iterator_ = iterator_type();
            }
            current_path_ = value_type();
        }

        template<typename iterator__>
        void UpdateCurrentPath(iterator__ iter) {

            if (iter != iterator__()) {
                current_path_ = iter->path();
            } else {
                current_path_ = value_type();
            }
        }

    private:
        std::variant<iterator_type, recursive_iterator_type> iterator_;
        value_type current_path_;
    };

public:
    Dir() = default;

    Dir(const std::filesystem::path& pth, bool recursivity) : file_path_(pth), 
                                                                is_recursive_(recursivity) {
    }

    Dir(const std::filesystem::path& pth) : file_path_(pth), 
                                                is_recursive_(false) {
    }

public:
    iterator begin() const { return iterator(*this, false); }

    iterator end() const { return iterator(*this, true); }

private:
    std::filesystem::path file_path_;
    bool is_recursive_ = false;
};