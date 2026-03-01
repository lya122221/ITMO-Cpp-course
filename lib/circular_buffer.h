#include <memory>

template<typename T, bool Extendable = false, typename Allocator = std::allocator<T>>
class circular_buffer {
public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using difference_type = std::ptrdiff_t;
    using size_type = size_t;
    using allocator_type = Allocator;

    circular_buffer(size_type capacity, const Allocator& alloc = Allocator()) 
                  : data_(nullptr), 
                    capacity_(capacity), 
                    size_(0), head_(0), 
                    alloc_(alloc)  
    {
        if (capacity > 0) {
            data_ = std::allocator_traits<Allocator>::allocate(alloc_, capacity_);
        }
    }
    circular_buffer(const circular_buffer& other)
                  : data_(nullptr), 
                    capacity_(other.capacity_), 
                    size_(0), 
                    head_(0), 
                    alloc_(std::allocator_traits<Allocator>::select_on_container_copy_construction(other.alloc_))
    {
        if (capacity_ > 0) {
            data_ = std::allocator_traits<Allocator>::allocate(alloc_, capacity_);

            for (size_type index = 0; index < other.size_; index++) {
                size_type other_index = (other.head_ + i) % other.capacity_;
                std::allocator_traits<Allocator>::construct(alloc_, data_ + i, other.data_[other_index]);
                size_++;
            }
        }
    }
    circular_buffer& operator=(const circular_buffer& other) {
        if (this != &other) {
            circular_buffer tmp(other);
            swap(tmp);
        }

        return *this;
    }
    ~circular_buffer() {
        for (size_type i = 0; i < size_; i++) {
            size_type index = (head_ + i) % capacity_;
            std::allocator_traits<Allocator>::destroy(alloc_, data_ + index);
        }

        if (data_ != nullptr) {
            std::allocator_traits<Allocator>::deallocate(alloc_, data_, capacity_);
        }
    }

    class const_iterator;
    class iterator;
    class reverse_iterator;
    class const_reverse_iterator;

    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;
    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const;

    iterator end();
    const_iterator end() const;
    const_iterator cend() const;
    reverse_iterator rend();
    const_reverse_iterator rend() const;
    const_reverse_iterator crend() const;

    void swap(circular_buffer& other);
    
    size_type size() const;
    size_type max_size() const;
    bool empty() const;

    iterator insert(const_iterator pos, const T& value);
    iterator insert(const_iterator pos, size_type count, const T& value);
    template<typename InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last);

    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    void clear();
    void assign(size_type count, const T& value);
    template<typename InputIt>
    void assign(InputIt first, InputIt last);
    void assign(std::initializer_list<T> ilist);

    allocator_type get_allocator() const;

    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;
    reference operator[](size_type pos);
    const_reference operator[](size_type pos) const;
    reference at(size_type pos);
    const_reference at(size_type pos) const;

    void push_back(const T& value);
    void pop_back();
    void push_front(const T& value);
    void pop_front();

private:
    T* data_;

    size_type capacity_;
    size_type size_;
    size_type head_;

    Allocator alloc_;
};