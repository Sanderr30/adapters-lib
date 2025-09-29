#pragma once

#include <vector>


class AsVector {
public:
    template<typename Range>
    auto operator()(Range&& range) const {
        using value_type = std::decay_t<decltype(*std::begin(range))>;
        std::vector<value_type> result;
        for (const auto& elem : range) {
            result.push_back(elem);
        }
        return result;
    }
};


template<typename Range>
auto operator|(Range&& range, const AsVector& as_vector) {
    return as_vector(std::forward<Range>(range));
}
