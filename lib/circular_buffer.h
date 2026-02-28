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

    circular_buffer(size_type capacity, const Allocator& alloc = Allocator());
    circular_buffer(const circular_buffer& other);
    circular_buffer& operator=(const circular_buffer& other);
    ~circular_buffer();

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
};