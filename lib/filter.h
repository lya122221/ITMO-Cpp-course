#include <utility>
#include <iterator>

template <typename Predicate>
struct FilterPart {
  Predicate pred;
};

template <typename Predicate>
FilterPart<Predicate> Filter(Predicate pred) {
  return FilterPart<Predicate>{pred};
}

template <typename Source, typename Predicate>
class FilterAdapter {
public:
  FilterAdapter(Source src, Predicate pred) : source_(src), pred_(pred) {}

  using in_iter = decltype(std::declval<Source>().begin());
  using in_end_iter = decltype(std::declval<Source>().end());

  class iterator {
  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::decay_t<decltype(*std::declval<in_iter>())>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;
    
    iterator(in_iter in_it, in_end_iter end_it, Predicate pred) : curr_in_(in_it), end_(end_it), pred_(pred) {
      skip_();
    }

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
  private:
    in_iter curr_in_;
    in_end_iter end_;
    Predicate pred_;

    void skip_() {
      while (curr_in_ != end_ && !pred_(*curr_in_)) {
        ++curr_in_;
      }
    }
  };

  iterator begin() {
    return iterator(source_.begin(), source_.end(), pred_);
  }
  iterator end() {
    return iterator(source_.end(), source_.end(), pred_);
  }
private:
  Source source_;
  Predicate pred_;
};

template <typename Source, typename Predicate>
FilterAdapter<Source, Predicate> operator|(Source source, FilterPart<Predicate> filter_part) {
  return FilterAdapter<Source, Predicate>(source, filter_part.pred);
}