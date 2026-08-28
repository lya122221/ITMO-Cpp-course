#include <iostream>
#include <fstream>
#include <memory>
#include <utility>

struct OpenFilesPart {};

inline OpenFilesPart OpenFiles() {
  return {};
}

template <typename Source>
class OpenFilesAdapter {
public:
  OpenFilesAdapter(Source source) : source_(source) {}

  using in_iter = decltype(std::declval<Source>().begin());
  using in_end_iter = decltype(std::declval<Source>().end());

  class iterator {
  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::ifstream;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    iterator(in_iter in_it, in_end_iter end_it) : curr_in_(in_it), end_(end_it) {
      skip_();
    }

    reference operator*() {
      return *curr_file_;
    }

    pointer operator->() {
      return curr_file_.get();
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
    std::shared_ptr<std::ifstream> curr_file_;

    bool try_open_() {
      curr_file_ = std::make_shared<std::ifstream>(
          static_cast<std::string>(*curr_in_)
      );
      return curr_file_->is_open() && curr_file_->good();
    }

    void skip_() {
      while (curr_in_ != end_ && !try_open_()) {
        ++curr_in_;
      }
      if (curr_in_ == end_) {
        curr_file_.reset();
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
auto operator|(Source source, OpenFilesPart open_file_part) {
  return OpenFilesAdapter<Source>(source);
}