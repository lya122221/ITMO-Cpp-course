#include <string>


struct SplitPart {
  std::string delimiters;
};

inline SplitPart Split(std::string delimiters) {
  return SplitPart{delimiters};
}

template <typename Source, typename Iterator>
class SplitAdapter {
public:
  SplitAdapter(Source source, std::string delimiters) : source_(source), delimiters_(delimiters) {}

  using in_iter = decltype(std::declval<Source>().begin());

  class iterator {
  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::string;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type;
  
    iterator() : done_(true) {}

    iterator(in_iter curr, in_iter end, const std::string* delimiters) : curr_flow_(curr) , end_flow_(end) , delimiters_(delimiters) , done_(false) {
      if (curr_flow_ != end_flow_) {
        init_iterator_();
        advance_to_next_token_();
      } else {
        done_ = true;
      }
    }

    value_type operator*() {
      return curr_str_;
    }

    iterator& operator++() {
      advance_to_next_token_();
      return *this;
    }

    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const iterator& other) const {
      return done_ == other.done_;
    }

    bool operator!=(const iterator& other) const {
      return !(*this == other);
    }
  private:
    Iterator curr_char_;
    Iterator end_char_;
    in_iter curr_flow_;
    in_iter end_flow_;
    std::string curr_str_;
    const std::string* delimiters_;
    bool done_;
    
    bool is_delimiter_(char c) const {
      return delimiters_->find(c) != std::string::npos;
    }

    void init_iterator_() {
      auto& flow = *curr_flow_;
      using elem_type = std::decay_t<decltype(flow)>;

      if constexpr (std::is_convertible_v<elem_type, std::string>) {
        const std::string& s = flow;
        curr_char_ = Iterator(s.begin());
        end_char_ = Iterator(s.end());
      } else {
        curr_char_ = Iterator(flow);
        end_char_ = Iterator();
      }
    }

    void advance_to_next_token_() {
      while (true) {
        if (curr_char_ == end_char_) {
          ++curr_flow_;
          if (curr_flow_ == end_flow_) {
            done_ = true;
            return;
          }
          init_iterator_();
          continue;
        }

        curr_str_.clear();
        while (curr_char_ != end_char_ && !is_delimiter_(*curr_char_)) {
          curr_str_ += *curr_char_;
          ++curr_char_;
        }

        if (curr_char_ != end_char_) {
          ++curr_char_;
        }

        return;
      }
    }
  };

  iterator begin() {
    return iterator(source_.begin(), source_.end(), &delimiters_);
  }
  iterator end() {
    return iterator();
  }

private:
  Source source_;
  std::string delimiters_;
};

template <typename Source>
auto operator|(Source source, SplitPart split_part) {
  using elem_type = std::decay_t<decltype(*source.begin())>;

  if constexpr (std::is_convertible_v<elem_type, std::string>) {
    return SplitAdapter<Source, std::string::iterator>(source, split_part.delimiters);
  } else {
    return SplitAdapter<Source, std::istreambuf_iterator<char>>(source, split_part.delimiters);
  }
}