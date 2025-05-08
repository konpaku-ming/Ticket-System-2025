#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include <climits>
#include <cstddef>
#include <iostream>
#include <memory>

#include "exceptions.h"

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template <typename T>
class vector {
   public:
    T *start = nullptr;
    T *finish = nullptr;
    T *storage_end = nullptr;
    std::allocator<T> alloc;
    using trait = std::allocator_traits<std::allocator<T>>;

    class const_iterator;

    class iterator {
       public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
        using iterator_category = std::output_iterator_tag;

        T *ptr = nullptr;

        iterator() = default;

        iterator(const iterator &other) {
            ptr = other.ptr;
        }

        iterator operator+(const int &n) const {
            iterator temp(*this);
            temp.ptr += n;
            return temp;
        }

        iterator operator-(const int &n) const {
            iterator temp(*this);
            temp.ptr -= n;
            return temp;
        }

        int operator-(const iterator &rhs) const {
            const int num = ptr - rhs.ptr;
            return num;
        }

        iterator &operator+=(const int &n) {
            ptr += n;
            return *this;
        }

        iterator &operator-=(const int &n) {
            ptr -= n;
            return *this;
        }

        iterator operator++(int) {
            iterator temp(*this);
            ++ptr;
            return temp;
        }

        iterator &operator++() {
            ++ptr;
            return *this;
        }

        iterator operator--(int) {
            iterator temp(*this);
            --ptr;
            return temp;
        }

        iterator &operator--() {
            --ptr;
            return *this;
        }

        T &operator*() const {
            return *ptr;
        }

        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }

        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.c_ptr;
        }

        bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }

        bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.c_ptr;
        }
    };

    class const_iterator {
       public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
        using iterator_category = std::output_iterator_tag;

        T *c_ptr = nullptr;

        const_iterator() = default;

        const_iterator(const const_iterator &other) {
            c_ptr = other.c_ptr;
        }

        const_iterator operator+(const int &n) const {
            c_ptr += n;
            const_iterator temp(*this);
            return temp;
        }

        const_iterator operator-(const int &n) const {
            c_ptr -= n;
            const_iterator temp(*this);
            return temp;
        }

        int operator-(const const_iterator &rhs) const {
            const int num = c_ptr - rhs.c_ptr;
            return num;
        }

        const_iterator &operator+=(const int &n) {
            c_ptr += n;
            return *this;
        }

        const_iterator &operator-=(const int &n) {
            c_ptr -= n;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator temp(*this);
            ++c_ptr;
            return temp;
        }

        const_iterator &operator++() {
            ++c_ptr;
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator temp(*this);
            --c_ptr;
            return temp;
        }

        const_iterator &operator--() {
            --c_ptr;
            return *this;
        }

        const T &operator*() const {
            return *c_ptr;
        }

        bool operator==(const iterator &rhs) const {
            return c_ptr == rhs.ptr;
        }

        bool operator==(const const_iterator &rhs) const {
            return c_ptr == rhs.c_ptr;
        }

        bool operator!=(const iterator &rhs) const {
            return c_ptr != rhs.ptr;
        }

        bool operator!=(const const_iterator &rhs) const {
            return c_ptr != rhs.c_ptr;
        }
    };

   public:
    vector() {
        start = nullptr;
        finish = nullptr;
        storage_end = nullptr;
    }

    vector(const vector &other) {
        size_t _capacity = other.storage_end - other.start;
        size_t _size = other.size();
        start = trait::allocate(alloc, _capacity);
        finish = start + _size;
        storage_end = start + _capacity;
        for (size_t i = 0; i < _size; i++) {
            trait::construct(alloc, start + i, other.start[i]);
        }
    }

    ~vector() {
        for (size_t i = 0; i < size(); i++) {
            trait::destroy(alloc, start + i);
        }
        trait::deallocate(alloc, start, storage_end - start);
        start = nullptr;
        finish = nullptr;
        storage_end = nullptr;
    }

    vector &operator=(const vector &other) {
        if (this == &other) {
            return *this;
        }
        for (size_t i = 0; i < size(); i++) {
            trait::destroy(alloc, start + i);
        }
        trait::deallocate(alloc, start, storage_end - start);
        start = nullptr;
        size_t _capacity = other.storage_end - other.start;
        size_t _size = other.size();
        start = trait::allocate(alloc, _capacity);
        finish = start + _size;
        storage_end = start + _capacity;
        for (size_t i = 0; i < _size; i++) {
            trait::construct(alloc, start + i, other.start[i]);
        }
        return *this;
    }

    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     */
    T &at(const size_t &pos) {
        if (pos >= size() || pos < 0) {
            throw index_out_of_bound();
        }
        return start[pos];
    }

    const T &at(const size_t &pos) const {
        if (pos >= size() || pos < 0) {
            throw index_out_of_bound();
        }
        return start[pos];
    }

    T &operator[](const size_t &pos) {
        if (pos >= size() || pos < 0) {
            throw index_out_of_bound();
        }
        return start[pos];
    }

    const T &operator[](const size_t &pos) const {
        if (pos >= size() || pos < 0) {
            throw index_out_of_bound();
        }
        return start[pos];
    }

    const T &front() const {
        return *start;
    }

    const T &back() const {
        return *(finish - 1);
    }

    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
        iterator begin_it;
        begin_it.ptr = start;
        return begin_it;
    }

    const_iterator begin() const {
        const_iterator begin_it;
        begin_it.c_ptr = start;
        return begin_it;
    }

    const_iterator cbegin() const {
        const_iterator cbegin_it;
        cbegin_it.c_ptr = start;
        return cbegin_it;
    }

    iterator end() {
        iterator end_it;
        end_it.ptr = finish;
        return end_it;
    }

    const_iterator end() const {
        const_iterator end_it;
        end_it.c_ptr = finish;
        return end_it;
    }

    const_iterator cend() const {
        const_iterator end_it;
        end_it.c_ptr = finish;
        return end_it;
    }

    /**
     * checks whether the container is empty
     */
    bool empty() const {
        return start == finish;
    }

    /**
     * returns the number of elements
     */
    size_t size() const {
        return finish - start;
    }

   private:
    void doubleSpace() {
        size_t _capacity = storage_end - start;
        size_t _size = size();
        size_t new_capacity = (_capacity + 1) * 2;
        T *temp = trait::allocate(alloc, new_capacity);
        for (size_t i = 0; i < _size; i++) {
            trait::construct(alloc, temp + i, start[i]);
            trait::destroy(alloc, start + i);
        }
        trait::deallocate(alloc, start, _capacity);
        start = temp;
        finish = start + _size;
        storage_end = start + new_capacity;
    }

   public:
    void clear() {
        for (size_t i = 0; i < size(); i++) {
            trait::destroy(alloc, start + i);
        }
        finish = start;
    }

    /**
     * inserts value before pos
     * returns an iterator pointing to the inserted value.
     */
    iterator insert(iterator pos, const T &value) {
        if (finish == storage_end) {
            size_t ind = pos.ptr - start;
            doubleSpace();
            pos.ptr = start + ind;
        }
        if (finish != start) trait::construct(alloc, finish, *(finish - 1));
        for (auto it = finish - 1; it != pos.ptr; --it) {
            *it = *(it - 1);
        }
        *pos = value;
        ++finish;
        return pos;
    }

    iterator insert(const size_t &ind, const T &value) {
        if (ind > size()) {
            throw index_out_of_bound();
        }
        if (finish == storage_end) {
            doubleSpace();
        }
        iterator pos;
        pos.ptr = start + ind;
        trait::construct(alloc, finish, *(finish - 1));
        for (auto it = finish - 1; it != pos.ptr; --it) {
            *it = *(it - 1);
        }
        *pos = value;
        ++finish;
        return pos;
    }

    iterator erase(iterator pos) {
        --finish;
        for (auto it = pos.ptr; it != finish; ++it) {
            *it = *(it + 1);
        }
        trait::destroy(alloc, finish);
        return pos;
    }

    /**
     * removes the element with index ind.
     * return an iterator pointing to the following element.
     * throw index_out_of_bound if ind >= size
     */
    iterator erase(const size_t &ind) {
        if (ind >= size()) {
            throw index_out_of_bound();
        }
        iterator pos;
        pos.ptr = start + ind;
        --finish;
        for (auto it = pos.ptr; it != finish; ++it) {
            *it = *(it + 1);
        }
        trait::destroy(alloc, finish);
        return pos;
    }

    /**
     * adds an element to the end.
     */
    void push_back(const T &value) {
        if (finish == storage_end) {
            doubleSpace();
        }
        trait::construct(alloc, finish, value);
        ++finish;
    }

    /**
     * remove the last element from the end.
     * throw container_is_empty if size() == 0
     */
    void pop_back() {
        if (empty()) {
            throw container_is_empty();
        }
        --finish;
        trait::destroy(alloc, finish);
    }
};
}  // namespace sjtu

#endif