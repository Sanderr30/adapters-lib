#pragma once 

#include <unordered_map>
#include <vector>
#include <utility>
#include <iostream>


template<typename Accumulator, typename Aggregator, typename KeyExtractor>
class AggregateByKey {
public:
    AggregateByKey(Accumulator accumulator, Aggregator aggregator, KeyExtractor key_extractor) : 
                                                        accumulator_(std::move(accumulator)),
                                                            aggregator_(std::move(aggregator)),
                                                            key_extractor_(std::move(key_extractor)) {
    }

    template<typename Range>
    auto operator()(Range&& range) const {
        using KeyType = decltype(key_extractor_(*std::begin(range)));
        using AccumulatorType = std::decay_t<Accumulator>;

        std::unordered_map<KeyType, AccumulatorType> res_map;
        std::vector<KeyType> key_order;

        for (const auto& item : range) {
            auto key = key_extractor_(item);
            if (res_map.find(key) == res_map.end()) {
                res_map[key] = accumulator_;
                key_order.push_back(key);
            }
            aggregator_(item, res_map[key]);
        }

        std::vector<std::pair<KeyType, AccumulatorType>> result;
        for (const auto& key : key_order) {
            result.emplace_back(key, std::move(res_map[key]));
        }

        return result;
    }

private:
    Accumulator accumulator_;
    Aggregator aggregator_;
    KeyExtractor key_extractor_;
};


template<typename Accumulator, typename Aggregator, typename KeyExtractor, typename Range>
auto operator|(Range&& range, const AggregateByKey<Accumulator, Aggregator, KeyExtractor>& aggregate_by_key) {
    return aggregate_by_key(std::forward<Range>(range));
}