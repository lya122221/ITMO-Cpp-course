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
    using pointer = T*;

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

    class const_iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using reference = const T&;
        using size_type = size_t;
        using pointer = const T*;

        const_iterator() : owner_(nullptr), index_(0) {}

        const_iterator(const circular_buffer* owner, size_type index) 
               : owner_(owner),  
                 index_(index) {}

        const_iterator& operator++() {
            ++index_;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++index_;
            return tmp;
        }

        const_iterator& operator--() {
            --index_;
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator tmp = *this;
            --index_;
            return tmp;
        }

        const_iterator& operator+=(difference_type n) {
            index_ += n;
            return *this;
        }

        const_iterator operator+(difference_type n) const {
            const_iterator tmp = *this;
            tmp += n;
            return tmp;
        }

        friend const_iterator operator+(difference_type n, const const_iterator& it) {
            return it + n;
        }

        const_iterator& operator-=(difference_type n) {
            index_ -= n;
            return *this;
        } 

        const_iterator operator-(difference_type n) const {
            const_iterator tmp = *this;
            tmp -= n;
            return tmp;
        }

        difference_type operator-(const const_iterator& other) const {
            return static_cast<difference_type>(index_) - static_cast<difference_type>(other.index_);
        }

        reference operator[](difference_type n) const {
            return owner_->data_[(owner_->head_ + index_ + n) % owner_->capacity_];
        }

        reference operator*() const {
            return owner_->data_[real_index_()];
        }

        pointer operator->() const {
            return owner_->data_ + real_index_();
        }

        bool operator==(const const_iterator& other) const { return index_ == other.index_; }
        bool operator!=(const const_iterator& other) const { return index_ != other.index_; }
        bool operator<(const const_iterator& other) const { return index_ < other.index_; }
        bool operator>(const const_iterator& other) const { return index_ > other.index_; }
        bool operator<=(const const_iterator& other) const { return index_ <= other.index_; }
        bool operator>=(const const_iterator& other) const { return index_ >= other.index_; }

    private:
        const circular_buffer* owner_;
        size_type index_;
        
        size_type real_index_() const {
            return (owner_->head_ + index_) % owner_->capacity_;
        }
    };
    class iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using reference = T&;
        using size_type = size_t;
        using pointer = T*;

        iterator() : owner_(nullptr), index_(0) {}

        iterator(circular_buffer* owner, size_type index) 
               : owner_(owner),  
                 index_(index) {}

        iterator& operator++() {
            ++index_;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++index_;
            return tmp;
        }

        iterator& operator--() {
            --index_;
            return *this;
        }

        iterator operator--(int) {
            iterator tmp = *this;
            --index_;
            return tmp;
        }

        iterator& operator+=(difference_type n) {
            index_ += n;
            return *this;
        }

        iterator operator+(difference_type n) const {
            iterator tmp = *this;
            tmp += n;
            return tmp;
        }

        friend iterator operator+(difference_type n, const iterator& it) {
            return it + n;
        }

        iterator& operator-=(difference_type n) {
            index_ -= n;
            return *this;
        } 

        iterator operator-(difference_type n) const {
            iterator tmp = *this;
            tmp -= n;
            return tmp;
        }

        difference_type operator-(const iterator& other) const {
            return static_cast<difference_type>(index_) - static_cast<difference_type>(other.index_);
        }

        reference operator[](difference_type n) const {
            return owner_->data_[(owner_->head_ + index_ + n) % owner_->capacity_];
        }

        reference operator*() const {
            return owner_->data_[real_index_()];
        }

        pointer operator->() const {
            return owner_->data_ + real_index_();
        }

        bool operator==(const iterator& other) const { return index_ == other.index_; }
        bool operator!=(const iterator& other) const { return index_ != other.index_; }
        bool operator<(const iterator& other) const { return index_ < other.index_; }
        bool operator>(const iterator& other) const { return index_ > other.index_; }
        bool operator<=(const iterator& other) const { return index_ <= other.index_; }
        bool operator>=(const iterator& other) const { return index_ >= other.index_; }

    private:
        circular_buffer* owner_;
        size_type index_;
        
        size_type real_index_() const {
            return (owner_->head_ + index_) % owner_->capacity_;
        }
    };
    // TODO
    class reverse_iterator;
    class const_reverse_iterator;

    iterator begin() {
        return iterator(this, 0);
    }
    const_iterator begin() const {
        return const_iterator(this, 0);
    }
    const_iterator cbegin() const {
        return const_iterator(this, 0);
    }

    // TODO
    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const;

    iterator end() {
        return iterator(this, size_);
    }
    const_iterator end() const {
        return const_iterator(this, size_);
    }
    const_iterator cend() const {
        return const_iterator(this, size_);
    }

    // TODO
    reverse_iterator rend();
    const_reverse_iterator rend() const;
    const_reverse_iterator crend() const;

    void swap(circular_buffer& other);
    
    size_type size() const {
        return size_;
    }
    size_type max_size() const {
        return alloc_traits::max_size(alloc_);
    }
    bool empty() const {
        return size_ == 0;
    }

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

    reference front() {
        return data_[head_];
    }
    const_reference front() const {
        return data_[head_];
    }
    reference back() {
        return data_[(head_ + size_ - 1) % capacity_];
    }
    const_reference back() const {
        return data_[(head_ + size_ - 1) % capacity_];
    }
    reference operator[](size_type pos) {
        return data_[(head_ + pos) % capacity_];
    }
    const_reference operator[](size_type pos) const {
        return data_[(head_ + pos) % capacity_];
    }
    reference at(size_type pos) {
        if (pos >= size_) {
            throw std::out_of_range("index out of range");
        }
        return data_[(head_ + pos) % capacity_];
    }
    const_reference at(size_type pos) const {
        if (pos >= size_) {
            throw std::out_of_range("index out of range");
        }
        return data_[(head_ + pos) % capacity_];
    }

    void push_back(const T& value);
    void pop_back();
    void push_front(const T& value);
    void pop_front();

private:
    pointer data_;

    size_type capacity_;
    size_type size_;
    size_type head_;

    Allocator alloc_;
};