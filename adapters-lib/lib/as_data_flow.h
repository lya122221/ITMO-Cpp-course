#include <utility>
#include <iterator>

template <typename Source>
class AsDataFlowAdapter {
public:
  AsDataFlowAdapter(Source& source) : source_(source) {}

  using iterator = decltype(std::begin(std::declval<Source&>()));
  using const_iterator = decltype(std::begin(std::declval<const Source&>()));
  using value_type = std::decay_t<decltype(*std::begin(std::declval<Source&>()))>;
  using reference = value_type&;
  using const_reference = const value_type&;
  using size_type = std::size_t;

  auto begin() const {
    return std::begin(source_);
  }
  auto end() const {
    return std::end(source_);
  }
private:
  Source& source_;
};

template <typename Source>
AsDataFlowAdapter<Source> AsDataFlow(Source& source) {
  return AsDataFlowAdapter<Source>(source);
}