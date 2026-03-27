#include <filesystem>

template <typename Directory>
class DirAdapter {
public:
  DirAdapter(Directory root, bool recursive) : root_(root), recursive_(recursive) {} 

  class iterator {
  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::filesystem::path;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type;
    
    iterator() = default;

    iterator(const std::filesystem::path& root, bool recursive) : recursive_(recursive) {
      std::error_code error_code;
      curr_dir_ = std::filesystem::recursive_directory_iterator(root, error_code);

      if (error_code) {
        curr_dir_ = std::filesystem::recursive_directory_iterator();
        return;
      }

      if (!recursive_) {
        curr_dir_.disable_recursion_pending();
      }
    }

    value_type operator*() {
      return curr_dir_->path();
    }

    iterator& operator++() {
      std::error_code error_code;
      curr_dir_.increment(error_code);
      
      if (error_code) {
        curr_dir_ = std::filesystem::recursive_directory_iterator();
      }
      
      return *this;
    }

    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const iterator& other) const {
      return curr_dir_ == other.curr_dir_;
    }

    bool operator!=(const iterator& other) const {
      return !(*this == other);
    }
  private:
    std::filesystem::recursive_directory_iterator curr_dir_;
    bool recursive_;
  };

  iterator begin() {
    return iterator(root_, recursive_);
  }

  iterator end() {
    return iterator();
  }

private:
  std::filesystem::path root_;
  bool recursive_;
};

template <typename Directory>
DirAdapter<Directory> Dir(Directory root, bool recursive) {
  return DirAdapter<Directory>(root, recursive);
}