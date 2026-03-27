#include <iostream>
#include <fstream>
#include <memory>
#include <utility>

struct OpenFilesPart {};

OpenFilesPart OpenFiles() {
  return {};
}

template <typename Source>
class OpenFilesAdapter {
public:
  OpenFilesAdapter(Source source) : source_(source) {}

  using in_iter = decltype(std::declval<Source>().begin());

  class iterator {
  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::ifstream;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type;
  
    iterator(in_iter in_it) : curr_in_(in_it), end_(source_.end()) {
      skip_();
    }

    value_type& operator*() {
      return curr_file_;
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
    in_iter end_;
    value_type* curr_file_;

    bool try_open_() {
      curr_file_ = std::ifstream(*curr_in_);
      if (!curr_file_) {
        return false;
      } else {
        return true;
      }
    }

    void skip_() {
      while (curr_in_ != end_ && !try_open_()) {
        ++curr_in_;
      }
    }
  };

  iterator begin() {
    auto it = iterator(source_.begin());
    return it;
  }
  iterator end() {
    return iterator(source_.end());
  }
private:
  Source source_;
};

template <typename Source>
auto operator|(Source source, OpenFilesPart open_file_part) {
  return OpenFilesAdapter<Source>(source);
}