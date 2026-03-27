#include <utility>
#include <iterator>

template <typename Source>
class AsDataFlowAdapter {
public:
  AsDataFlowAdapter(Source& source) : source_(source) {}

  auto begin() {
    return std::begin(source_);
  }
  auto end() {
    return std::end(source_);
  }
private:
  Source& source_;
};

template <typename Source>
AsDataFlowAdapter<Source> AsDataFlow(Source& source) {
  return AsDataFlowAdapter<Source>(source);
}