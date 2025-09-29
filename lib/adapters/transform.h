#pragma once

#include <type_traits>


template<typename Function>
class Transform {
public:
	template<typename Range>
	class iterator {
	public:
		using value_type = decltype(std::declval<Function>()(std::declval<typename Range::iterator::reference>()));
		using Baseiterator = decltype(std::begin(std::declval<Range&>()));
		using difference_type = ptrdiff_t;
		using pointer = value_type*;
		using reference = value_type&;
		using iterator_category = std::input_iterator_tag;

	public:
		iterator() = default;

		iterator(auto&& current, auto&& end, const Function& function) : 
								current_(std::forward<decltype(current)>(current)),
								end_(std::forward<decltype(current)>(end)), 
								function_(function) {
		}

		iterator(const iterator& other) : current_(other.current_), 
										end_(other.end_), 
										function_(other.function_) {
		}


		iterator(iterator&& other) : current_(std::move(other.current_)),
										end_(std::move(other.end_)),
										function_(std::move(other.function_)) {
		}

		iterator& operator=(const iterator& other) {
			if (this != &other) {
				current_ = other.current_;
				end_ = other.end_;
				function_ = other.function_;
			}
			return *this;
		}

		iterator& operator=(iterator&& other) {
			current_ = std::move(other.current_);
			end_ = std::move(other.end_);
			function_ = std::move(other.function_);
			return *this;
		}

		~iterator() = default;

	public:
		decltype(auto) operator*() {
			auto value =  *current_;
    		return function_(value);
		}

		pointer operator->() const { return &function_(*current_); }

		iterator& operator++() {
			++current_;
			return *this;
		}

		iterator operator++(int) {
			iterator temp = *this;
			++(*this);
			return temp;
		}

		bool operator==(const iterator& other) const { return current_ == other.current_; }

		bool operator!=(const iterator& other) const { return !(*this == other); }

	private:
		Baseiterator current_;
		Baseiterator end_;
		Function function_;
	};

	template<typename Range>
	auto operator()(Range&& range) const {
		struct Wrapper {
			using iterator = typename Transform<Function>::template iterator<Range>;
			
			mutable Range range_;
			Function function_;
			
			auto begin() const { return iterator(range_.begin(), range_.end(), function_); }
			
			auto end() const { return iterator(range_.end(), range_.end(), function_); }
		};

		return Wrapper{std::forward<Range>(range), function_};
	}

public:
	Transform(Function function) : function_(function) {}

private:
	Function function_;
};


template<typename Range, typename Function>
auto operator|(Range&& range, const Transform<Function>& transform) {
	return transform(std::forward<Range>(range));
}