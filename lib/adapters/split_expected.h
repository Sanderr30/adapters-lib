#pragma once

#include <expected>
#include <utility>
#include <iterator>
#include <vector>


template <typename Parser>
class SplitExpected {
public:
    SplitExpected(Parser parser) : parser_(std::move(parser)) {}

    template <typename Range>
    auto operator()(Range&& range) const {
        using iterator = typename std::decay_t<Range>::iterator;
        using ExpectedType = typename std::iterator_traits<iterator>::value_type;
        using ValueType = std::remove_cvref_t<decltype(std::declval<ExpectedType>().value())>;
        using ErrorType = std::remove_cvref_t<decltype(std::declval<ExpectedType>().error())>;

        std::vector<ValueType> values;
        std::vector<ErrorType> errors;

        for (auto&& element : range) {
            if (element.has_value()) {
                values.push_back(element.value());
            } else {
                errors.push_back(element.error());
            }
        }
        return std::make_pair(std::move(errors), std::move(values));
    }

private:
    Parser parser_;
};


template <typename Range, typename Parser>
auto operator|(Range&& range, const SplitExpected<Parser>& split_expected) {
    return split_expected(std::forward<Range>(range));
}