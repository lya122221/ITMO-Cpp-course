#include <memory>
#include <stdexcept>
#include <algorithm>

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

    circular_buffer() : circular_buffer(0) {}
    explicit circular_buffer(const Allocator& alloc) : circular_buffer(0, alloc) {}
    circular_buffer(size_type capacity, const Allocator& alloc = Allocator()) 
                  : data_(nullptr), 
                    capacity_(capacity), 
                    size_(0), head_(0), 
                    alloc_(alloc)  
    {
        if (capacity > 0) {
            data_ = alloc_traits::allocate(alloc_, capacity_);
        }
    }
    circular_buffer(const circular_buffer& other)
                  : data_(nullptr), 
                    capacity_(other.capacity_), 
                    size_(0), 
                    head_(0), 
                    alloc_(alloc_traits::select_on_container_copy_construction(other.alloc_))
    {
        if (capacity_ > 0) {
            data_ = alloc_traits::allocate(alloc_, capacity_);

            for (size_type index = 0; index < other.size_; index++) {
                size_type other_index = (other.head_ + index) % other.capacity_;
                alloc_traits::construct(alloc_, data_ + index, other.data_[other_index]);
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
            alloc_traits::destroy(alloc_, data_ + index);
        }

        if (data_ != nullptr) {
            alloc_traits::deallocate(alloc_, data_, capacity_);
        }
    }
    circular_buffer(std::initializer_list<T> ilist, const Allocator& alloc = Allocator()) : circular_buffer(ilist.size(), alloc) {
        assign(ilist.begin(), ilist.end());
    }
    circular_buffer(size_type n, const T& value, const Allocator& alloc = Allocator()) : circular_buffer(n, alloc) {
        assign(n, value);
    }

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

        friend class circular_buffer;
        friend class const_iterator;
    private:
        circular_buffer* owner_;
        size_type index_;
        
        size_type real_index_() const {
            return (owner_->head_ + index_) % owner_->capacity_;
        }
    };
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

        const_iterator(const iterator& it)
                     : owner_(it.owner_), 
                       index_(it.index_) {}

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

        friend class circular_buffer;
    private:
        const circular_buffer* owner_;
        size_type index_;
        
        size_type real_index_() const {
            return (owner_->head_ + index_) % owner_->capacity_;
        }
    };
    class reverse_iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using reference = T&;
        using pointer = T*;

        reverse_iterator() : owner_(nullptr), index_(0) {}

        reverse_iterator(circular_buffer* owner, size_type index) : owner_(owner), index_(index) {}

        reverse_iterator(const iterator& it) : owner_(it.owner_), index_(it.index_) {}

        iterator base() const {
            return iterator(owner_, index_);
        }

        reference operator*() const {
            return owner_->data_[real_index_()];
        }

        pointer operator->() const {
            return owner_->data_ + real_index_();
        }

        reference operator[](difference_type n) const {
            return owner_->data_[
                (owner_->head_ + index_ - 1 - n) % owner_->capacity_
            ];
        }

        reverse_iterator& operator++() {
            --index_;
            return *this;
        }

        reverse_iterator operator++(int) {
            reverse_iterator tmp = *this;
            --index_;
            return tmp;
        }

        reverse_iterator& operator--() {
            ++index_;
            return *this;
        }

        reverse_iterator operator--(int) {
            reverse_iterator tmp = *this;
            ++index_;
            return tmp;
        }

        reverse_iterator& operator+=(difference_type n) {
            index_ -= n;
            return *this;
        }

        reverse_iterator& operator-=(difference_type n) {
            index_ += n;
            return *this;
        }

        reverse_iterator operator+(difference_type n) const {
            reverse_iterator tmp = *this;
            tmp += n;
            return tmp;
        }

        reverse_iterator operator-(difference_type n) const {
            reverse_iterator tmp = *this;
            tmp -= n;
            return tmp;
        }

        difference_type operator-(const reverse_iterator& other) const {
            return static_cast<difference_type>(other.index_) - static_cast<difference_type>(index_);
        }

        friend reverse_iterator operator+(difference_type n, const reverse_iterator& it) {
            return it + n;
        }

        bool operator==(const reverse_iterator& other) const { return index_ == other.index_; }
        bool operator!=(const reverse_iterator& other) const { return index_ != other.index_; }
        bool operator<(const reverse_iterator& other) const { return index_ > other.index_; }
        bool operator>(const reverse_iterator& other) const { return index_ < other.index_; }
        bool operator<=(const reverse_iterator& other) const { return index_ >= other.index_; }
        bool operator>=(const reverse_iterator& other) const { return index_ <= other.index_; }

        friend class circular_buffer;
        friend class const_reverse_iterator;

    private:
        circular_buffer* owner_;
        size_type index_;

        size_type real_index_() const {
            return (owner_->head_ + index_ - 1) % owner_->capacity_;
        }
    };
    class const_reverse_iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using reference = const T&;
        using pointer = const T*;

        const_reverse_iterator() : owner_(nullptr), index_(0) {}

        const_reverse_iterator(const circular_buffer* owner, size_type index) : owner_(owner), index_(index) {}

        const_reverse_iterator(const reverse_iterator& it) : owner_(it.owner_), index_(it.index_) {}

        const_reverse_iterator(const const_iterator& it) : owner_(it.owner_), index_(it.index_) {}

        const_iterator base() const {
            return const_iterator(owner_, index_);
        }

        reference operator*() const {
            return owner_->data_[real_index_()];
        }

        pointer operator->() const {
            return owner_->data_ + real_index_();
        }

        reference operator[](difference_type n) const {
            return owner_->data_[(owner_->head_ + index_ - 1 - n) % owner_->capacity_];
        }

        const_reverse_iterator& operator++() {
            --index_;
            return *this;
        }

        const_reverse_iterator operator++(int) {
            const_reverse_iterator tmp = *this;
            --index_;
            return tmp;
        }

        const_reverse_iterator& operator--() {
            ++index_;
            return *this;
        }

        const_reverse_iterator operator--(int) {
            const_reverse_iterator tmp = *this;
            ++index_;
            return tmp;
        }

        const_reverse_iterator& operator+=(difference_type n) {
            index_ -= n;
            return *this;
        }

        const_reverse_iterator& operator-=(difference_type n) {
            index_ += n;
            return *this;
        }

        const_reverse_iterator operator+(difference_type n) const {
            const_reverse_iterator tmp = *this;
            tmp += n;
            return tmp;
        }

        const_reverse_iterator operator-(difference_type n) const {
            const_reverse_iterator tmp = *this;
            tmp -= n;
            return tmp;
        }

        difference_type operator-(const const_reverse_iterator& other) const {
            return static_cast<difference_type>(other.index_) - static_cast<difference_type>(index_);
        }

        friend const_reverse_iterator operator+(difference_type n, const const_reverse_iterator& it) {
            return it + n;
        }

        bool operator==(const const_reverse_iterator& other) const { return index_ == other.index_; }
        bool operator!=(const const_reverse_iterator& other) const { return index_ != other.index_; }
        bool operator<(const const_reverse_iterator& other) const { return index_ > other.index_; }
        bool operator>(const const_reverse_iterator& other) const { return index_ < other.index_; }
        bool operator<=(const const_reverse_iterator& other) const { return index_ >= other.index_; }
        bool operator>=(const const_reverse_iterator& other) const { return index_ <= other.index_; }

        friend class circular_buffer;

    private:
        const circular_buffer* owner_;
        size_type index_;

        size_type real_index_() const {
            return (owner_->head_ + index_ - 1) % owner_->capacity_;
        }
    };

    iterator begin() {
        return iterator(this, 0);
    }
    const_iterator begin() const {
        return const_iterator(this, 0);
    }
    const_iterator cbegin() const {
        return const_iterator(this, 0);
    }
    reverse_iterator rbegin() {
        return reverse_iterator(this, size_);
    }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(this, size_);
    }
    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(this, size_);
    }
    iterator end() {
        return iterator(this, size_);
    }
    const_iterator end() const {
        return const_iterator(this, size_);
    }
    const_iterator cend() const {
        return const_iterator(this, size_);
    }
    reverse_iterator rend() {
        return reverse_iterator(this, 0);
    }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(this, 0);
    }
    const_reverse_iterator crend() const {
        return const_reverse_iterator(this, 0);
    }
    void swap(circular_buffer& other) {
        if constexpr (alloc_traits::propagate_on_container_swap::value) {
            std::swap(alloc_, other.alloc_);
        }
        std::swap(data_, other.data_);
        std::swap(capacity_, other.capacity_);
        std::swap(size_, other.size_);
        std::swap(head_, other.head_);
    }
    
    size_type size() const {
        return size_;
    }
    size_type max_size() const {
        return alloc_traits::max_size(alloc_);
    }
    bool empty() const {
        return size_ == 0;
    }

    iterator insert(const_iterator pos, const T& value) {
        return insert(pos, size_type(1), value);
    }
    iterator insert(const_iterator pos, size_type count, const T& value) {
        size_type pos_idx = pos.index_;

        if (count == 0) {
            return iterator(this, pos_idx);
        }

        if constexpr (Extendable) {
            size_type saved_pos = pos_idx;
            while (size_ + count > capacity_) {
                grow();
            }
            pos_idx = saved_pos;
        } else {
            if (size_ + count > capacity_) {
                if (capacity_ == 0) {
                    return iterator(this, 0);
                }

                size_type overflow = size_ + count - capacity_;

                size_type front_drop = std::min(overflow, pos_idx);
                for (size_type i = 0; i < front_drop; i++) {
                    alloc_traits::destroy(alloc_, data_ + head_);
                    head_ = (head_ + 1) % capacity_;
                    size_--;
                }

                pos_idx -= front_drop;
                overflow -= front_drop;

                count -= overflow;

                if (count == 0) {
                    return iterator(this, pos_idx);
                }
            }
        }

        size_type elements_after = size_ - pos_idx;
        shift_right(pos_idx, count);

        for (size_type i = 0; i < count; i++) {
            size_type idx = (head_ + pos_idx + i) % capacity_;
            if (i < elements_after) {
                data_[idx] = value;
            } else {
                alloc_traits::construct(alloc_, data_ + idx, value);
            }
        }

        size_ += count;
        return iterator(this, pos_idx);
    }
    template<typename InputIt, typename = std::void_t<typename std::iterator_traits<InputIt>::iterator_category>>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        size_type count = static_cast<size_type>(std::distance(first, last));
        size_type pos_idx = pos.index_;

        if (count == 0) {
            return iterator(this, pos_idx);
        }

        if constexpr (Extendable) {
            size_type saved_pos = pos_idx;
            while (size_ + count > capacity_) {
                grow();
            }
            pos_idx = saved_pos;
        } else {
            if (size_ + count > capacity_) {
                if (capacity_ == 0) {
                    return iterator(this, 0);
                }

                size_type overflow = size_ + count - capacity_;

                size_type front_drop = std::min(overflow, pos_idx);
                for (size_type i = 0; i < front_drop; i++) {
                    alloc_traits::destroy(alloc_, data_ + head_);
                    head_ = (head_ + 1) % capacity_;
                    size_--;
                }
                pos_idx -= front_drop;
                overflow -= front_drop;

                for (size_type i = 0; i < overflow; i++) {
                    first++;
                }
                count -= overflow;

                if (count == 0) {
                    return iterator(this, pos_idx);
                }
            }
        }

        size_type elements_after = size_ - pos_idx;
        shift_right(pos_idx, count);

        auto it = first;
        for (size_type i = 0; i < count; i++) {
            size_type idx = (head_ + pos_idx + i) % capacity_;
            if (i < elements_after) {
                data_[idx] = *it;
            } else {
                alloc_traits::construct(alloc_, data_ + idx, *it);
            }
            it++;
        }

        size_ += count;
        return iterator(this, pos_idx);
    }
    iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
        return insert(pos, ilist.begin(), ilist.end());
    }

    iterator erase(const_iterator pos) {
        return erase(pos, pos + 1);
    }
    iterator erase(const_iterator first, const_iterator last) {
        if (first == last) {
            return iterator(this, first.index_);
        }

        size_type first_idx = first.index_;
        size_type last_idx = last.index_;
        size_type count = last_idx - first_idx;

        size_type elements_after = size_ - last_idx;
        for (size_type i = 0; i < elements_after; i++) {
            size_type dst = (head_ + first_idx + i) % capacity_;
            size_type src = (head_ + last_idx + i) % capacity_;
            data_[dst] = data_[src];
        }

        for (size_type i = 0; i < count; i++) {
            size_type idx = (head_ + size_ - 1 - i) % capacity_;
            alloc_traits::destroy(alloc_, data_ + idx);
        }

        size_ -= count;

        return iterator(this, first_idx);
    }

    void clear() {
        for (size_type i = 0; i < size_; i++) {
            size_type idx = (head_ + i) % capacity_;
            alloc_traits::destroy(alloc_, data_ + idx);
        }
        size_ = 0;
        head_ = 0;
    }

    void resize(size_type new_size) {
        if (new_size < size_) {
            for (size_type i = new_size; i < size_; i++) {
                size_type idx = (head_ + i) % capacity_;
                alloc_traits::destroy(alloc_, data_ + idx);
            }

            size_ = new_size;
        } else if (new_size > size_) {
            if (new_size > capacity_) {
                if constexpr (Extendable) {
                    while (new_size > capacity_) {
                        grow();
                    }
                } else {
                    throw std::out_of_range("resizing a non-expandable buffer");
                }
            }

            size_type constructed = size_;
            for (size_type i = size_; i < new_size; i++) {
                size_type idx = (head_ + i) % capacity_;
                alloc_traits::construct(alloc_, data_ + idx);
                constructed++;
            }

            size_ = new_size;
        }
    }

    void resize(size_type new_size, const T& value) {
        if (new_size < size_) {
            for (size_type i = new_size; i < size_; i++) {
                size_type idx = (head_ + i) % capacity_;
                alloc_traits::destroy(alloc_, data_ + idx);
            }

            size_ = new_size;
        } else if (new_size > size_) {
            if (new_size > capacity_) {
                if constexpr (Extendable) {
                    while (new_size > capacity_) {
                        grow();
                    }
                } else {
                    throw std::out_of_range("resizing a non-expandable buffer");
                }
            }

            size_type constructed = size_;
            for (size_type i = size_; i < new_size; i++) {
                size_type idx = (head_ + i) % capacity_;
                alloc_traits::construct(alloc_, data_ + idx, value);
                constructed++;
            }

            size_ = new_size;
        }
    }

    void assign(size_type count, const T& value) {
        if constexpr (Extendable) {
            if (count > capacity_) {
                reallocate(count);
            } else {
                destroy_all();
            }
        } else {
            destroy_all();
            count = std::min(count, capacity_);
        }

        size_type constructed = 0;
        for (size_type i = 0; i < count; i++) {
            alloc_traits::construct(alloc_, data_ + i, value);
            constructed++;
        }

        size_ = count;
    }
    template<typename InputIt>
    void assign(InputIt first, InputIt last) {
        size_type count = static_cast<size_type>(std::distance(first, last));

        if constexpr (Extendable) {
            if (count > capacity_) {
                reallocate(count);
            } else {
                destroy_all();
            }

            size_type constructed = 0;

            for (auto it = first; it != last; it++) {
                alloc_traits::construct(alloc_, data_ + constructed, *it);
                constructed++;
            }

            size_ = count;
        } else {
            destroy_all();

            if (count > capacity_) {
                size_type skip = count - capacity_;
                auto it = first;
                for (size_type i = 0; i < skip; i++) {
                    it++;
                }

                size_type constructed = 0;
                while (it != last) {
                    alloc_traits::construct(alloc_, data_ + constructed, *it);
                    constructed++;
                    it++;
                }

                size_ = capacity_;
            } else {
                size_type constructed = 0;
                for (auto it = first; it != last; it++) {
                    alloc_traits::construct(alloc_, data_ + constructed, *it);
                    constructed++;
                }

                size_ = count;
            }
        }
    }
    void assign(std::initializer_list<T> ilist) {
        assign(ilist.begin(), ilist.end());
    }

    allocator_type get_allocator() const {
        return alloc_;
    }

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

    void push_back(const T& value) {
        if (size_ == capacity_) {
            if constexpr (Extendable) {
                grow();
            } else {
                alloc_traits::destroy(alloc_, data_ + head_);
                alloc_traits::construct(alloc_, data_ + head_, value);
                head_ = (head_ + 1) % capacity_;
                return;
            }
        }

        size_type tail = (head_ + size_) % capacity_;
        alloc_traits::construct(alloc_, data_ + tail, value);
        size_++;
    }
    void pop_back() {
        if (empty()) {
            throw std::out_of_range("pop_front on empty circular_buffer");
        }

        size_type back_idx = (head_ + size_ - 1) % capacity_;
        alloc_traits::destroy(alloc_, data_ + back_idx);
        size_--;
    }
    void push_front(const T& value) {
        if (size_ == capacity_) {
            if constexpr (Extendable) {
                grow();
            } else {
                size_type back_idx = (head_ + size_ - 1) % capacity_;
                alloc_traits::destroy(alloc_, data_ + back_idx);
                head_ = (head_ == 0) ? capacity_ - 1 : head_ - 1;
                alloc_traits::construct(alloc_, data_ + head_, value);

                return;
            }
        }

        head_ = (head_ == 0) ? capacity_ - 1 : head_ - 1;
        alloc_traits::construct(alloc_, data_ + head_, value);
        size_++;
    }
    void pop_front() {
        if (empty()) {
            throw std::out_of_range("pop_back on empty circular_buffer");
        }

        alloc_traits::destroy(alloc_, data_ + head_);
        head_ = (head_ + 1) % capacity_;
        size_--;
    }

    friend bool operator==(const circular_buffer& lhs, const circular_buffer& rhs) {
        if (lhs.size_ != rhs.size_) {
            return false;
        }

        for (size_type i = 0; i < lhs.size_; i++) {
            if (!(lhs[i] == rhs[i])) {
                return false;
            }
        }

        return true;
    }

    friend bool operator!=(const circular_buffer& lhs, const circular_buffer& rhs) {
        return !(lhs == rhs);
    }

private:
    using alloc_traits = std::allocator_traits<Allocator>;
    pointer data_;

    size_type capacity_;
    size_type size_;
    size_type head_;

    Allocator alloc_;

    void destroy_all() {
        for (size_type i = 0; i < size_; ++i) {
            size_type idx = (head_ + i) % capacity_;
            alloc_traits::destroy(alloc_, data_ + idx);
        }
        size_ = 0;
        head_ = 0;
    }

    void reallocate(size_type new_cap) {
        destroy_all();
        if (data_) {
            alloc_traits::deallocate(alloc_, data_, capacity_);
            data_ = nullptr;
        }
        capacity_ = new_cap;
        if (capacity_ > 0) {
            data_ = alloc_traits::allocate(alloc_, capacity_);
        }
    }

    void grow() {
        size_type new_cap = (capacity_ == 0) ? 1 : capacity_ * 2;
        pointer new_data = alloc_traits::allocate(alloc_, new_cap);

        size_type moved = 0;
        for (size_type i = 0; i < size_; i++) {
            size_type old_idx = (head_ + i) % capacity_;
            alloc_traits::construct(alloc_, new_data + i, data_[old_idx]);
            moved++;
        }

        destroy_all();
        if (data_) {
            alloc_traits::deallocate(alloc_, data_, capacity_);
        }

        data_ = new_data;
        capacity_ = new_cap;
        head_ = 0;
        size_ = moved;
    }

    void shift_right(size_type pos_idx, size_type count) {
        if (count == 0 || pos_idx == size_) {
            return;
        }

        size_type elements_to_move = size_ - pos_idx;

        for (size_type i = elements_to_move; i > 0; i--) {
            size_type src_logical = pos_idx + i - 1;
            size_type dst_logical = pos_idx + count + i - 1;
            size_type src = (head_ + src_logical) % capacity_;
            size_type dst = (head_ + dst_logical) % capacity_;

            if (dst_logical >= size_) {
                alloc_traits::construct(alloc_, data_ + dst, data_[src]);
            } else {
                data_[dst] = data_[src];
            }
        }
    }
};