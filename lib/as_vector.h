#include <vector>

struct AsVectorPart {};

inline AsVectorPart AsVector() {
  return AsVectorPart{};
}

template <typename Source>
auto operator|(Source source, AsVectorPart) {
  using value_type = std::decay_t<decltype(*source.begin())>;
  return std::vector<value_type>(source.begin(), source.end());
}