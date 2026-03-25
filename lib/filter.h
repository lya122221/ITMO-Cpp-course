#include <utility>
#include <iterator>

template <typename Predicate>
struct FilterPart {
  Predicate pred;
};

template <typename Predicate>
FilterPart<Predicate> Filter(Predicate pred) {
  return FilterPart<Predicate>(pred);
}

template <typename Source, typename Predicate>
class FilterAdapter {
public:
  FilterAdapter(Source src, Predicate pred) : source_(src), pred_(pred) {}

  using in_iter = decltype(std::declval<Source>().begin());

  class iterator {
  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = decltype(*std::declval<in_iter>());
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type;
  
    iterator(in_iter in_it, Predicate pred) : curr_in_(in_it), pred_(pred), end_(source_.end()) {}

    value_type operator*() {
      return *curr_in_;
    }

    iterator& operator++() {
      ++curr_in_;
      skip_();
      return *this;
    }

    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const iterator& other) const {
      return curr_in_ == other.curr_in_;
    }

    bool operator!=(const iterator& other) const {
      return !(*this == other);
    }

    void skip() {
      while (curr_in_ != end_ && !pred_(*curr_in_)) {
        ++curr_in_;
      }
    }
  private:
    in_iter curr_in_;
    in_iter end_;
    Predicate pred_;
  };

  iterator begin() {
    auto it = iterator(source_.begin(), pred_);
    it.skip();
    return it;
  }
  iterator end() {
    return iterator(source_.end(), pred_);
  }
private:
  Source source_;
  Predicate pred_;
};

template <typename Source, typename Predicate>
FilterAdapter<Source, Predicate> operator|(Source source, FilterPart<Predicate> filter_part) {
  return FilterAdapter<Source, Predicate>(source, filter_part.pred);
}