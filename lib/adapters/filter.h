#pragma once


template<typename Predicate>
class Filter {
public:
	template<typename Range>
	class iterator {
	public:

		using Baseiterator = decltype(std::declval<Range>().begin());
		using value_type = typename std::iterator_traits<Baseiterator>::value_type;

		using difference_type = ptrdiff_t;
		using reference = const value_type&;
		using pointer = const value_type*;
		using iterator_category = std::input_iterator_tag;
	
	public:
		iterator() = default;

		iterator(Baseiterator current, Baseiterator end, Predicate predicate) : 
								current_(std::move(current)), 
								end_(std::move(end)), 
								predicate_(predicate) {
			ToNextValid();
		}

		iterator(const iterator& other) : current_(other.current_), 
										end_(other.end_), 
										predicate_(other.predicate_) {
		}

		iterator(iterator&& other) : 
				current_(std::move(other.current_)),
				end_(std::move(other.end_)),
				predicate_(std::move(other.predicate_)) {
		}

		iterator& operator=(const iterator& other) {
			if (this != &other) {
				current_ = other.current_;
				end_ = other.end_;
				predicate_ = other.predicate_;
			}
			return *this;
		}

		iterator& operator=(iterator&& other) {
			current_ = std::move(other.current_);
			end_ = std::move(other.end_);
			predicate_ = std::move(other.predicate_);
			return *this;
		}

		~iterator() = default;
		
	public:
		value_type operator*() { return *current_; }

		pointer operator->() { return &(*current_); } 		

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
			while (current_ != end_ && !predicate_(*current_)) {
				++current_;
			}
		}

	private:
		Baseiterator current_;
		Baseiterator end_;
		Predicate predicate_;
		
	};

public:
	Filter(Predicate predicate) : predicate_(predicate) {}

	template<typename Range>
	auto operator()(Range&& range) const {
		struct Wrapper {
			using iterator = typename Filter::iterator<Range>;
			
			Range range_;
			Predicate predicate_;
			
			auto begin() const { return iterator(range_.begin(), range_.end(), predicate_); }
			
			auto end() const { return iterator(range_.end(), range_.end(), predicate_); }
		};
		return Wrapper{std::forward<Range>(range), predicate_};
	}

private:
	Predicate predicate_;
};


template<typename Range, typename Predicate>
auto operator|(Range&& range, const Filter<Predicate>& filter) {
	return filter(std::forward<Range>(range));
}