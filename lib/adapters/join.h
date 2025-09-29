#pragma once

#include <unordered_map>
#include <optional>
#include <vector>
#include <utility>
#include <type_traits>


struct Empty {};


template<typename Key, typename Value>
struct KV {
    Key key;
    Value value;
    bool operator==(const KV& other) const {
        return key == other.key && value == other.value;
    }
};


template<typename Base, typename Joined>
struct JoinResult {
    Base base;
    std::optional<Joined> joined;
    bool operator==(const JoinResult& other) const {
        return base == other.base && joined == other.joined;
    }
};


template<typename RightFlow, typename LKeyExtractor, typename RKeyExtractor>
class Join {
public:
    Join(RightFlow right_flow, LKeyExtractor l_key, RKeyExtractor r_key) : 
											right_flow_(std::move(right_flow)),
											left_key_extractor(std::move(l_key)),
												right_key_extractor(std::move(r_key)) {
	}

    template<typename Range>
    auto operator()(Range&& range) const {
        using LeftValue = typename std::decay_t<Range>::value_type;
        using RightValue = typename RightFlow::value_type;
        using KeyType = decltype(left_key_extractor(std::declval<LeftValue>()));

        std::unordered_map<KeyType, RightValue> right_map;
        
		for (const auto& item : right_flow_) {
            auto key = right_key_extractor(item);
            right_map[key] = item;
        }

        std::vector<JoinResult<LeftValue, RightValue>> result;

        for (const auto& element : range) {
            auto key = left_key_extractor(element);
            auto iter = right_map.find(key);
            if (iter == right_map.end()) {
                result.push_back({element, std::nullopt});
            } else {
                result.push_back({element, iter->second});
            }
        }
        return result;
    }

private:
    RightFlow right_flow_;
    LKeyExtractor left_key_extractor;
    RKeyExtractor right_key_extractor;
};


template<typename RightFlow>
class Join<RightFlow, Empty, Empty> {
public:
    Join(RightFlow& right_flow) : right_flow_(right_flow) {}

    template<typename Range>
    auto operator()(Range&& range) const {
        using LeftKV = typename std::decay_t<Range>::value_type;
        using RightKV = typename RightFlow::value_type;
        using BaseType = decltype(LeftKV::value);
        using JoinedType = decltype(RightKV::value);
        using ResultType = JoinResult<BaseType, JoinedType>;

        std::unordered_map<decltype(LeftKV::key), JoinedType> right_map;
        for (const auto& item : right_flow_) {
            right_map[item.key] = item.value;
        }

        std::vector<ResultType> result;
        for (const auto& element : range) {
            auto key = element.key;
            auto iter = right_map.find(key);
            if (iter == right_map.end()) {
                result.emplace_back(ResultType{element.value, std::nullopt});
            } else {
                result.emplace_back(ResultType{element.value, iter->second});
            }
        }
        return result;
    }

private:
    RightFlow& right_flow_;
};


template<typename Range, typename RightFlow, typename LKeyExtractor, typename RKeyExtractor>
auto operator|(Range&& range, const Join<RightFlow, LKeyExtractor, RKeyExtractor>& join) {
    return join(std::forward<Range>(range));
}


template<typename Range, typename RightFlow>
auto operator|(Range&& range, const Join<RightFlow, Empty, Empty>& join) {
    return join(std::forward<Range>(range));
}


template<typename RightFlow>
Join(RightFlow& right_flow) -> Join<RightFlow, Empty, Empty>;
