#include <utility>
#include <vector>

struct SplitExpectedPart {};

inline SplitExpectedPart SplitExpected() {
  return SplitExpectedPart{};
}

template <typename Source>
class ExpectedFlow {
public:
  ExpectedFlow(Source source) : source_(source) {}

  using in_iter = decltype(std::declval<Source>().begin());

  class iterator {
  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = typename std::decay_t<decltype(*std::declval<in_iter>())>::value_type;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type;

    iterator(in_iter curr, in_iter end) : curr_(curr), end_(end) {
      skip_();
    }

    value_type operator*() {
      return (*curr_).value();
    }

    iterator& operator++() {
      ++curr_;
      skip_();
      return *this;
    }

    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const iterator& other) const {
      return curr_ == other.curr_;
    }

    bool operator!=(const iterator& other) const {
      return !(*this == other);
    }

  private:
    in_iter curr_;
    in_iter end_;

    void skip_() {
      while (curr_ != end_ && !(*curr_).has_value()) {
        ++curr_;
      }
    }
  };

  iterator begin() {
    return iterator(source_.begin(), source_.end());
  }

  iterator end() {
    return iterator(source_.end(), source_.end());
  }

private:
  Source source_;
};

template <typename Source>
class UnexpectedFlow {
public:
  UnexpectedFlow(Source source) : source_(source) {}

  using in_iter = decltype(std::declval<Source>().begin());

  class iterator {
  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = typename std::decay_t<decltype(*std::declval<in_iter>())>::error_type;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type;

    iterator(in_iter curr, in_iter end) : curr_(curr), end_(end) {
      skip_();
    }

    value_type operator*() {
      return (*curr_).error();
    }

    iterator& operator++() {
      ++curr_;
      skip_();
      return *this;
    }

    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const iterator& other) const {
      return curr_ == other.curr_;
    }

    bool operator!=(const iterator& other) const {
      return !(*this == other);
    }

  private:
    in_iter curr_;
    in_iter end_;

    void skip_() {
      while (curr_ != end_ && (*curr_).has_value()) {
        ++curr_;
      }
    }
  };

  iterator begin() {
    return iterator(source_.begin(), source_.end());
  }

  iterator end() {
    return iterator(source_.end(), source_.end());
  }

private:
  Source source_;
};

template <typename Source>
auto operator|(Source source, SplitExpectedPart) {
  using value_type = std::decay_t<decltype(*source.begin())>;
  std::vector<value_type> data(source.begin(), source.end());

  return std::pair{ UnexpectedFlow<std::vector<value_type>>(data), ExpectedFlow<std::vector<value_type>>(data) };
}