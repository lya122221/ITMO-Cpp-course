#include <optional>
#include <vector>
#include <unordered_map>

template <typename Key, typename Value>
struct KV {
    Key key;
    Value value;
};

template <typename Base, typename Joined>
struct JoinResult {
    Base base;
    std::optional<Joined> joined;
};

template <typename Flow>
struct JoinKVPart {
    Flow& flow;
};

template <typename Flow>
auto Join(Flow& flow) {
    return JoinKVPart<Flow>{flow};
}

template <typename Flow, typename LeftKey, typename RightKey>
struct JoinPart {
    Flow& flow;
    LeftKey left_key;
    RightKey right_key;
};

template <typename Flow, typename LeftKey, typename RightKey>
auto Join(Flow& flow, LeftKey left_key, RightKey right_key) {
    return JoinPart<Flow, LeftKey, RightKey>{flow, left_key, right_key};
}

template <typename Source, typename Flow>
class JoinKVAdapter {
public:
    JoinKVAdapter(Source source, JoinKVPart<Flow> join_part) : source_(source), right_flow_(join_part.flow) {}

    using left_kv = std::decay_t<decltype(*std::declval<Source>().begin())>;
    using key_type = decltype(left_kv::key);
    using left_val = decltype(left_kv::value);
    using right_kv = std::decay_t<decltype(*std::declval<Flow>().begin())>;
    using right_val = decltype(right_kv::value);

    std::vector<JoinResult<left_val, right_val>> operator()() {
        std::unordered_map<key_type, right_val> right_map;

        for (auto it = right_flow_.begin(); it != right_flow_.end(); ++it) {
            auto& kv = *it;
            right_map[kv.key] = kv.value;
        }

        std::vector<JoinResult<left_val, right_val>> result;
        for (auto it = source_.begin(); it != source_.end(); ++it) {
            auto& kv = *it;
            auto found = right_map.find(kv.key);
            if (found != right_map.end()) {
                result.push_back({kv.value, found->second});
            } else {
                result.push_back({kv.value, std::nullopt});
            }
        }
        return result;
    }

private:
    Source source_;
    Flow& right_flow_;
};

template <typename Source, typename Flow, typename LeftKey, typename RightKey>
class JoinAdapter {
public:
    JoinAdapter(Source source, JoinPart<Flow, LeftKey, RightKey> join_part) : source_(source), right_flow_(join_part.flow), left_key_(join_part.left_key), right_key_(join_part.right_key) {}

    using left_elem = std::decay_t<decltype(*std::declval<Source>().begin())>;
    using right_elem = std::decay_t<decltype(*std::declval<Flow>().begin())>;
    using key_type = std::decay_t<decltype(std::declval<LeftKey>()(std::declval<left_elem>()))>;

    std::vector<JoinResult<left_elem, right_elem>> operator()() {
        std::unordered_map<key_type, right_elem> right_map;

        for (auto it = right_flow_.begin(); it != right_flow_.end(); ++it) {
            auto elem = *it;
            right_map[right_key_(elem)] = elem;
        }

        std::vector<JoinResult<left_elem, right_elem>> result;
        for (auto it = source_.begin(); it != source_.end(); ++it) {
            auto elem = *it;
            auto key = left_key_(elem);
            auto found = right_map.find(key);
            if (found != right_map.end()) {
                result.push_back({elem, found->second});
            } else {
                result.push_back({elem, std::nullopt});
            }
        }
        return result;
    }

private:
    Source source_;
    Flow& right_flow_;
    LeftKey left_key_;
    RightKey right_key_;
};

template <typename Source, typename Flow, typename LeftKey, typename RightKey>
auto operator|(Source source, JoinPart<Flow, LeftKey, RightKey> join_part) {
    return JoinAdapter(source, join_part)();
}

template <typename Source, typename Flow>
auto operator|(Source source, JoinKVPart<Flow> join_kv_part) {
    return JoinKVAdapter(source, join_kv_part)();
}