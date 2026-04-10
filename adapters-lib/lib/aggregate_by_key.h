#include <unordered_map>
#include <vector>

template <typename InitValue, typename Updater, typename KeyExtractor>
struct AggregateByKeyPart {
  InitValue init_value;
  Updater updater;
  KeyExtractor key_extractor;
};

template <typename InitValue, typename Updater, typename KeyExtractor>
auto AggregateByKey(InitValue init_val, Updater updater, KeyExtractor key_ext) {
  return AggregateByKeyPart<InitValue, Updater, KeyExtractor>{init_val, updater, key_ext};
}

template <typename Source, typename InitValue, typename Updater, typename KeyExtractor>
class AggregateByKeyAdapter {
public:
  using key_type = std::decay_t<decltype(std::declval<KeyExtractor>()(*std::declval<Source>().begin()))>;
  using updater_type = std::decay_t<InitValue>;
  using value_type = std::pair<key_type, updater_type>;
  using result_type = std::vector<value_type>;

  AggregateByKeyAdapter(Source source, InitValue init_val, Updater updater, KeyExtractor key_ext) : source_(source) , init_value_(init_val) , updater_(updater) , key_extractor_(key_ext) {}

  result_type operator()() {
    std::unordered_map<key_type, updater_type> groups;
    std::vector<key_type> order;

    for (auto it = source_.begin(); it != source_.end(); ++it) {
      auto& elem = *it;
      auto key = key_extractor_(elem);

      auto found = groups.find(key);
      if (found == groups.end()) {
        groups[key] = init_value_;
        order.push_back(key);
        updater_(elem, groups[key]);
      } else {
        updater_(elem, found->second);
      }
    }

    result_type result;
    result.reserve(order.size());
    for (auto& key : order) {
      result.emplace_back(key, groups[key]);
    }

    return result;
  }

private:
  Source source_;
  InitValue init_value_;
  Updater updater_;
  KeyExtractor key_extractor_;
};

template <typename Source, typename InitValue, typename Updater, typename KeyExtractor>
auto operator|(Source source, AggregateByKeyPart<InitValue, Updater, KeyExtractor> aggregate_by_key_part) {
  return AggregateByKeyAdapter(source, aggregate_by_key_part.init_value, aggregate_by_key_part.updater, aggregate_by_key_part.key_extractor)();
}