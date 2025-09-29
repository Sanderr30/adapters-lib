#pragma once 

#include <utility>
#include <string>


class Split {
public:
    template<typename Range>
    class iterator {
    public:
        using value_type = std::string;
        using difference_type = ptrdiff_t;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::input_iterator_tag;

        using Fileiterator = typename Range::iterator;

    public:
        iterator() = default;
        
        iterator(Fileiterator current, Fileiterator end, const std::string& delimiters)
                                            : current_file_(std::move(current)),
                                                end_file_(std::move(end)),
                                                delimiters_(delimiters),
                                                buffer_position_(0) {
            ReadNextFile();
            ToNextToken();
        }


        iterator(const iterator& other) : current_file_(other.current_file_),
                                        end_file_(other.end_file_),
                                        delimiters_(other.delimiters_),
                                        buffer_(other.buffer_),
                                        buffer_view_(other.buffer_view_),
                                        current_token_(other.current_token_),
                                        buffer_position_(other.buffer_position_) {

        }

        iterator(iterator&& other) : current_file_(std::move(other.current_file_)),
                                    end_file_(std::move(other.end_file_)),
                                    delimiters_(std::move(other.delimiters_)),
                                    buffer_(std::move(other.buffer_)),
                                    buffer_view_(std::move(other.buffer_view_)),
                                    current_token_(std::move(other.current_token_)),
                                    buffer_position_(std::move(other.buffer_position_)) { 
        }

        iterator& operator=(const iterator& other) {
            if (this != &other) {
                current_file_ = other.current_file_;
                end_file_ = other.end_file_;
                delimiters_ = other.delimiters_;
                buffer_ = other.buffer_;
                buffer_view_ = other.buffer_view_;
                current_token_ = other.current_token_;
                buffer_position_ = other.buffer_position_;
            }
            return *this;
        }

        iterator& operator=(iterator&& other) {
            if (this != &other) {
                current_file_ = std::move(other.current_file_);
                end_file_ = std::move(other.end_file_);
                delimiters_ = std::move(other.delimiters_);
                buffer_ = std::move(other.buffer_);
                buffer_view_ = std::move(other.buffer_view_);
                current_token_ = std::move(other.current_token_);
                buffer_position_ = other.buffer_position_;
            }
            return *this;
        }

        ~iterator() = default;

    public:
        reference operator*() const { return current_token_; }

        pointer operator->() const { return &current_token_; }

        iterator& operator++() {
            ToNextToken();
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = std::move(*this);
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const {
            return current_file_ == other.current_file_ &&
                    buffer_position_ == other.buffer_position_ &&
                    buffer_ == other.buffer_;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

    private:
        void ReadNextFile() {
            using buffer_iterator = std::istreambuf_iterator<char>;
        
            if (buffer_position_ > 0) {
                buffer_.erase(0, buffer_position_);
                buffer_position_ = 0;
            }	
            current_token_.clear();
        
            while (current_file_ != end_file_) {
                auto& file = *current_file_;
        
                using v_type = std::decay_t<decltype(file)>;
        
                if constexpr (std::is_same_v<v_type, std::ifstream>) {
                    const size_t chunk_size = 4096;
                    char buffer[chunk_size];
                    file.read(buffer, chunk_size);
                    size_t read = file.gcount();
                    if (read > 0) {
                        buffer_.append(buffer, read);
                        break;
                    } else {
                        ++current_file_;
                        if (!buffer_.empty()) {
                            break;
                        }
                    }
                } else if constexpr (std::is_same_v<v_type, std::string>) {
                    buffer_view_ = std::string_view(file);
                    ++current_file_;
                    if (!buffer_view_.empty()) {
                        break;
                    }
                } else {
                    buffer_ = file.str();
                    ++current_file_;
                    if (!buffer_.empty()) {
                        break;
                    }
                }
            }
        }

        bool RefillBuffer() {
            if (buffer_.empty()) {
                ReadNextFile();
                if (buffer_.empty()) {
                    current_token_.clear();
                    return true;
                }
            }
            return false;
        }

        void ToNextToken() {
            if (RefillBuffer()) {
                return;
            }
        
            size_t start = buffer_position_;
            size_t end = buffer_.find_first_of(delimiters_, start);
        
            if (end == std::string::npos) {
                current_token_ = buffer_.substr(start);
                buffer_position_ = buffer_.size();
                buffer_.clear();
            } else {
                current_token_ = buffer_.substr(start, end - start);
                buffer_position_ = end + 1;
            }
        }
        

    private:
        Fileiterator current_file_;
        Fileiterator end_file_;
        std::string delimiters_;
        std::string buffer_;
        std::string_view buffer_view_;
        std::string current_token_;
        size_t buffer_position_;
    };

public:
    Split(const std::string& delimiters) : delimiters_(delimiters) {}

    template<typename Range>
    auto operator()(Range&& range) const {
        struct Wrapper {
            using iterator = typename Split::iterator<Range>;

            Range range;
            const Split split;

            iterator begin() const { return iterator(range.begin(), range.end(), split.delimiters_); }

            iterator end() const { return iterator(range.end(), range.end(), split.delimiters_); }
        };

        return Wrapper{std::forward<Range>(range), *this};
    }

private:
    std::string delimiters_;
};


template<typename Range>
auto operator|(Range&& range, const Split& split) {
    return split(std::forward<Range>(range));
}