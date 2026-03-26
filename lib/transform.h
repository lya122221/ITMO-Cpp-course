#include <utility>
#include <iterator>

template <typename Predicate>
struct TransformPart {
  Predicate pred;
};

template <typename Predicate>
TransformPart<Predicate> Transform(Predicate pred) {
  return TransformPart<Predicate>(pred);
}

template <typename Source, typename Predicate>
class TransformAdapter {
public:
  TransformAdapter(Source src, Predicate pred) : source_(src), pred_(pred) {}

  using in_iter = decltype(std::declval<Source>().begin());

  class iterator {
  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = decltype(std::declval<Predicate>()(*std::declval<in_iter>()));
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type;
  
    iterator(in_iter in_it, Predicate pred) : curr_in_(in_it), pred_(pred) {}

    value_type operator*() {
      return pred_(*curr_in_);
    }

    iterator& operator++() {
      ++curr_in_;
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
    Predicate* pred_;
  };

  iterator begin() {
    return iterator(source_.begin(), &pred_);
  }
  iterator end() {
    return iterator(source_.end(), &pred_);
  }
private:
  Source source_;
  Predicate pred_;
};

template <typename Source, typename Predicate>
TransformAdapter<Source, Predicate> operator|(Source source, TransformPart<Predicate> transform_part) {
  return TransformAdapter<Source, Predicate>(source, transform_part.pred);
}