#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>

#include "exceptions.h"

namespace sjtu {
/**
 * @brief a container like std::priority_queue which is a heap internal.
 * **Exception Safety**: The `Compare` operation might throw exceptions for
 * certain data. In such cases, any ongoing operation should be terminated, and
 * the priority queue should be restored to its original state before the
 * operation began.
 */
template <typename T, class Compare = std::less<T> >
class priority_queue {
   public:
    struct Node {
        T value;
        Node *child = nullptr, *sibling = nullptr;

        Node(T val) : value(val) {
        }
    };

    /**
     * @brief default constructor
     */
    priority_queue() {
        queue_size = 0;
        root = nullptr;
    };

    /**
     * @brief copy constructor
     * @param other the priority_queue to be copied
     */

    void HeapCopy(Node *&a, Node *b) {
        if (b == nullptr) return;
        a = new Node(b->value);
        HeapCopy(a->child, b->child);
        HeapCopy(a->sibling, b->sibling);
    }

    priority_queue(const priority_queue &other) {
        queue_size = other.queue_size;
        HeapCopy(root, other.root);
    }

    /**
     * @brief deconstructor
     */

    void DeleteHeap(Node *a) {
        if (a == nullptr) return;
        DeleteHeap(a->child);
        DeleteHeap(a->sibling);
        delete a;
    }

    ~priority_queue() {
        DeleteHeap(root);
    }

    /**
     * @brief Assignment operator
     * @param other the priority_queue to be assigned from
     * @return a reference to this priority_queue after assignment
     */
    priority_queue &operator=(const priority_queue &other) {
        if (this == &other) {
            return *this;
        }
        DeleteHeap(root);
        root = nullptr;
        queue_size = other.queue_size;
        HeapCopy(root, other.root);
        return *this;
    }

    /**
     * @brief get the top element of the priority queue.
     * @return a reference of the top element.
     * @throws container_is_empty if empty() returns true
     */
    const T &top() const {
        if (empty()) throw container_is_empty();
        return root->value;
    }

    Node *Meld(Node *a, Node *b) {
        if (a == nullptr) return b;
        if (b == nullptr) return a;
        try {
            if (Compare{}(a->value, b->value)) std::swap(a, b);
            b->sibling = a->child;
            a->child = b;
            return a;
        } catch (...) {
            throw runtime_error();
        }
    }

    /**
     * @brief push new element to the priority queue.
     * @param e the element to be pushed
     */
    void push(const T &e) {
        auto tmp = root;
        Node *new_node = new Node(e);
        try {
            root = Meld(root, new_node);
            queue_size++;
        } catch (...) {
            root = tmp;
            delete new_node;
            throw runtime_error();
        }
    }

    Node *Merges(Node *x) {
        if (x == nullptr || x->sibling == nullptr) return x;
        Node *y = x->sibling;
        Node *z = y->sibling;
        auto tmp1 = x->sibling;
        auto tmp2 = y->sibling;
        x->sibling = y->sibling = nullptr;
        try {
            return Meld(Merges(z), Meld(x, y));
        } catch (...) {
            x->sibling = tmp1;
            x->sibling->sibling = tmp2;
            throw runtime_error();
        }
    }

    /**
     * @brief delete the top element from the priority queue.
     * @throws container_is_empty if empty() returns true
     */
    void pop() {
        if (empty()) throw container_is_empty();
        try {
            Node *temp = Merges(root->child);
            queue_size--;
            delete root;
            root = temp;
        } catch (...) {
            throw runtime_error();
        }
    }

    /**
     * @brief return the number of elements in the priority queue.
     * @return the number of elements.
     */
    size_t size() const {
        return queue_size;
    }

    /**
     * @brief check if the container is empty.
     * @return true if it is empty, false otherwise.
     */
    bool empty() const {
        return queue_size == 0;
    }

    /**
     * @brief merge another priority_queue into this one.
     * The other priority_queue will be cleared after merging.
     * The complexity is at most O(logn).
     * @param other the priority_queue to be merged.
     */
    void merge(priority_queue &other) {
        try {
            root = Meld(root, other.root);
            queue_size += other.queue_size;
            other.queue_size = 0;
            other.root = nullptr;
        } catch (...) {
            throw runtime_error();
        }
    }

   public:
    Node *root = nullptr;
    size_t queue_size = 0;
};
}  // namespace sjtu

#endif