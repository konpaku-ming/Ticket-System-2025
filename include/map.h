/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include "exceptions.h"
#include "../include/utility.h"

namespace sjtu {
  template<class Key, class T, class Compare = std::less<Key> >
  class map {
  public:
    /**
     * the internal type of data.
     * it should have a default constructor, a copy constructor.
     * You can use sjtu::map as value_type by typedef.
     */
    typedef pair<const Key, T> value_type;
    /**
     * see BidirectionalIterator at CppReference for help.
     *
     * if there is anything wrong throw invalid_iterator.
     *     like it = map.begin(); --it;
     *       or it = map.end(); ++end();
     */

    struct Node {
      value_type value_;
      Node *parent_ = nullptr;
      Node *left_ = nullptr;
      Node *right_ = nullptr;
      int height_;

      Node(value_type value, int height = 1) : value_(value), height_(height) {
      }

      Node(Key k, T val, int height = 1)
        : value_(value_type(k, val)), height_(height) {
      }

      const Key &get_k() { return value_.first; }

      T &get_val() { return value_.second; }

      Node *find_k(const Key &k) {
        if (equal_k(k, get_k())) {
          return this;
        }
        if (Compare{}(k, get_k())) {
          if (left_ != nullptr) {
            return left_->find_k(k);
          }
          return nullptr;
        } else {
          if (right_ != nullptr) {
            return right_->find_k(k);
          }
          return nullptr;
        }
      }
    };

    static bool equal_k(const Key &x, const Key &y) {
      if (Compare{}(x, y))
        return false;
      if (Compare{}(y, x))
        return false;
      return true;
    }

    // 找到当前节点最往左的孩子
    static Node *get_left(Node *x) {
      if (x->left_ == nullptr)
        return x;
      return get_left(x->left_);
    }

    // 找到当前节点最往右的孩子
    static Node *get_right(Node *x) {
      if (x->right_ == nullptr)
        return x;
      return get_right(x->right_);
    }

    void update_height(Node *&x) {
      x->height_ = std::max((x->left_ != nullptr ? x->left_->height_ : 0),
                            (x->right_ != nullptr ? x->right_->height_ : 0)) +
                   1;
    }

    // 将当前节点左旋
    Node *left_spin(Node *&x) {
      auto tmp = x->right_;
      x->right_ = tmp->left_;
      if (tmp->left_ != nullptr)
        tmp->left_->parent_ = x;
      tmp->left_ = x;
      tmp->parent_ = x->parent_;
      x->parent_ = tmp;
      update_height(x);
      update_height(tmp);
      return tmp; // 新的parent
    }

    Node *right_spin(Node *&x) {
      auto tmp = x->left_;
      x->left_ = tmp->right_;
      if (tmp->right_ != nullptr)
        tmp->right_->parent_ = x;
      tmp->right_ = x;
      tmp->parent_ = x->parent_;
      x->parent_ = tmp;
      update_height(x);
      update_height(tmp);
      return tmp; // 新的parent
    }

    Node *maintain(Node *&x) {
      if (x == nullptr) {
        return nullptr;
      }
      int lh = x->left_ != nullptr ? x->left_->height_ : 0;
      int rh = x->right_ != nullptr ? x->right_->height_ : 0;
      if (abs(lh - rh) > 1) {
        if (lh > rh) {
          // 左树高
          int llh = x->left_ != nullptr && x->left_->left_ != nullptr
                      ? x->left_->left_->height_
                      : 0;
          int lrh = x->left_ != nullptr && x->left_->right_ != nullptr
                      ? x->left_->right_->height_
                      : 0;
          if (llh >= lrh) {
            x = right_spin(x);
          } else {
            x->left_ = left_spin(x->left_);
            x = right_spin(x);
          }
        } else {
          // 右树高
          int rlh = x->right_ != nullptr && x->right_->left_ != nullptr
                      ? x->right_->left_->height_
                      : 0;
          int rrh = x->right_ != nullptr && x->right_->right_ != nullptr
                      ? x->right_->right_->height_
                      : 0;
          if (rrh >= rlh) {
            x = left_spin(x);
          } else {
            x->right_ = right_spin(x->right_);
            x = left_spin(x);
          }
        }
      }
      return x;
    }

    Node *insert_node(Node *&x, Node *fa, const Key &k, const T &val) {
      if (x == nullptr) {
        x = new Node(k, val);
        x->parent_ = fa;
        return x;
      }
      if (Compare{}(x->get_k(), k)) {
        x->right_ = insert_node(x->right_, x, k, val);
      } else {
        x->left_ = insert_node(x->left_, x, k, val);
      }
      update_height(x);
      x->parent_ = fa;
      return maintain(x);
    }

    Node *exchange_node(Node *x, Node *new_x) {
      Node *fa = x->parent_;
      Node *l = x->left_;
      Node *r = x->right_;
      Node *nfa = new_x->parent_;
      Node *nl = new_x->left_;
      Node *nr = new_x->right_;
      if (new_x->parent_ != x) {
        // 没有重叠
        x->parent_ = nfa;
        x->left_ = nl;
        x->right_ = nr;
        new_x->parent_ = fa;
        new_x->left_ = l;
        new_x->right_ = r;
        if (fa != nullptr) {
          if (fa->left_ == x) {
            fa->left_ = new_x;
          } else {
            fa->right_ = new_x;
          }
        }
        if (l != nullptr) {
          l->parent_ = new_x;
        }
        if (r != nullptr) {
          r->parent_ = new_x;
        }
        if (nfa != nullptr) {
          if (nfa->left_ == new_x) {
            nfa->left_ = x;
          } else {
            nfa->right_ = x;
          }
        }
        if (nl != nullptr) {
          nl->parent_ = x;
        }
        if (nr != nullptr) {
          nr->parent_ = x;
        }
      } else {
        if (nl != nullptr) {
          nl->parent_ = x;
        }
        if (nr != nullptr) {
          nr->parent_ = x;
        }
        x->left_ = nl;
        x->right_ = nr;
        new_x->parent_ = x->parent_;
        if (fa != nullptr) {
          if (fa->left_ == x) {
            fa->left_ = new_x;
          } else {
            fa->right_ = new_x;
          }
        }
        x->parent_ = new_x;
        if (l == new_x) {
          new_x->left_ = x;
          new_x->right_ = r;
          if (r != nullptr) {
            r->parent_ = new_x;
          }
        } else {
          new_x->right_ = x;
          new_x->left_ = l;
          if (l != nullptr) {
            l->parent_ = new_x;
          }
        }
      }
      if (x == root) {
        root = new_x;
      }
      update_height(x);
      update_height(new_x);
      return new_x;
    }

    Node *remove_node(Node *&x, Node *fa, const Key &k) {
      if (Compare{}(x->get_k(), k)) {
        x->right_ = remove_node(x->right_, x, k);
      } else if (Compare{}(k, x->get_k())) {
        x->left_ = remove_node(x->left_, x, k);
      } else {
        if (x->left_ == nullptr && x->right_ == nullptr) {
          delete x;
          x = nullptr;
          return nullptr;
        } else if (x->left_ == nullptr && x->right_ != nullptr) {
          auto tmp_r = x->right_;
          delete x;
          x = tmp_r;
        } else if (x->left_ != nullptr && x->right_ == nullptr) {
          auto tmp_l = x->left_;
          delete x;
          x = tmp_l;
        } else {
          auto tmp = get_left(x->right_); // 右子树的最左节点
          x = exchange_node(x, tmp);
          // x = tmp;
          x->right_ = remove_node(x->right_, x, k);
        }
      }
      if (x != nullptr) {
        update_height(x);
        x->parent_ = fa;
      }
      return maintain(x);
    }

    class const_iterator;

    class iterator {
      friend map;
      friend const_iterator;

    private:
      Node *ptr = nullptr;
      map *m = nullptr;
      // Node *rt = nullptr;

    public:
      iterator() {
        ptr = nullptr;
        m = nullptr;
        // rt = nullptr;
      }

      iterator(Node *ptr_, map *m_) {
        ptr = ptr_;
        m = m_;
      }

      iterator(const iterator &other) {
        ptr = other.ptr;
        m = other.m;
      }

      iterator operator++(int) {
        if (ptr == nullptr) {
          throw invalid_iterator();
        }
        iterator tmp(*this);
        if (ptr->right_ != nullptr) {
          ptr = get_left(ptr->right_);
          return tmp;
        } else {
          while (ptr->parent_ != nullptr && ptr->parent_->right_ == ptr) {
            ptr = ptr->parent_;
          }
          ptr = ptr->parent_; // 把nullptr作为end()
        }
        return tmp;
      }

      iterator &operator++() {
        if (ptr == nullptr) {
          throw invalid_iterator();
        }
        if (ptr->right_ != nullptr) {
          ptr = get_left(ptr->right_);
          return *this;
        } else {
          while (ptr->parent_ != nullptr && ptr->parent_->right_ == ptr) {
            ptr = ptr->parent_;
          }
          ptr = ptr->parent_; // 把nullptr作为end()
        }
        return *this;
      }

      iterator operator--(int) {
        iterator tmp(*this);
        if (ptr == nullptr) {
          // 检查是否为end()
          if (m->root != nullptr) {
            ptr = get_right(m->root);
            return tmp;
          } else {
            throw invalid_iterator();
          }
        } else if (ptr->left_ != nullptr) {
          ptr = get_right(ptr->left_);
          return tmp;
        } else {
          while (ptr->parent_ != nullptr && ptr->parent_->left_ == ptr) {
            ptr = ptr->parent_;
          }
          if (ptr == m->root) {
            // begin()--
            throw invalid_iterator();
          } else {
            ptr = ptr->parent_;
          }
        }
        return tmp;
      }

      iterator &operator--() {
        if (ptr == nullptr) {
          // 检查是否为end()
          if (m->root != nullptr) {
            ptr = get_right(m->root);
            return *this;
          } else {
            throw invalid_iterator();
          }
        } else if (ptr->left_ != nullptr) {
          ptr = get_right(ptr->left_);
          return *this;
        } else {
          while (ptr->parent_ != nullptr && ptr->parent_->left_ == ptr) {
            ptr = ptr->parent_;
          }
          if (ptr == m->root) {
            // begin()--
            throw invalid_iterator();
          } else {
            ptr = ptr->parent_;
          }
        }
        return *this;
      }

      value_type &operator*() const { return ptr->value_; }

      bool operator==(const iterator &rhs) const {
        return ptr == rhs.ptr && m == rhs.m;
      }

      bool operator==(const const_iterator &rhs) const {
        return ptr == rhs.ptr && m == rhs.m;
      }

      /**
       * some other operator for iterator.
       */
      bool operator!=(const iterator &rhs) const {
        return ptr != rhs.ptr || m != rhs.m;
      }

      bool operator!=(const const_iterator &rhs) const {
        return ptr != rhs.ptr || m != rhs.m;
      }

      value_type *operator->() const noexcept {
        if (ptr == nullptr) {
          throw invalid_iterator();
        }
        return &(ptr->value_);
      }
    };

    class const_iterator {
      friend map;
      friend iterator;

    private:
      Node *ptr = nullptr;
      const map *m = nullptr;
      // Node *rt = nullptr;

    public:
      const_iterator() {
        ptr = nullptr;
        m = nullptr;
      }

      const_iterator(Node *ptr_, const map *m_) {
        ptr = ptr_;
        m = m_;
        // rt = rt_;
      }

      const_iterator(const const_iterator &other) {
        ptr = other.ptr;
        m = other.m;
        // rt = other.rt;
      }

      const_iterator(const iterator &other) {
        ptr = other.ptr;
        m = other.m;
      }

      const_iterator operator++(int) {
        if (ptr == nullptr) {
          throw invalid_iterator();
        }
        const_iterator tmp(*this);
        if (ptr->right_ != nullptr) {
          ptr = get_left(ptr->right_);
        } else {
          while (ptr->parent_ != nullptr && ptr->parent_->right_ == ptr) {
            ptr = ptr->parent_;
          }
          ptr = ptr->parent_; // 把nullptr作为end()
        }
        return tmp;
      }

      const_iterator &operator++() {
        if (ptr == nullptr) {
          throw invalid_iterator();
        }
        if (ptr->right_ != nullptr) {
          ptr = get_left(ptr->right_);
        } else {
          while (ptr->parent_ != nullptr && ptr->parent_->right_ == ptr) {
            ptr = ptr->parent_;
          }
          ptr = ptr->parent_; // 把nullptr作为end()
        }
        return *this;
      }

      const_iterator operator--(int) {
        const_iterator tmp(*this);
        if (ptr == nullptr) {
          // 检查是否为end()
          if (m->root != nullptr) {
            ptr = get_right(m->root);
          } else {
            throw invalid_iterator();
          }
        } else if (ptr->left_ != nullptr) {
          ptr = get_right(ptr->left_);
        } else {
          while (ptr->parent_ != nullptr && ptr->parent_->left_ == ptr) {
            ptr = ptr->parent_;
          }
          if (ptr == m->root) {
            // begin()--
            throw invalid_iterator();
          } else {
            ptr = ptr->parent_;
          }
        }
        return tmp;
      }

      const_iterator &operator--() {
        if (ptr == nullptr) {
          // 检查是否为end()
          if (m->root != nullptr) {
            ptr = get_right(m->root);
          } else {
            throw invalid_iterator();
          }
        } else if (ptr->left_ != nullptr) {
          ptr = get_right(ptr->left_);
        } else {
          while (ptr->parent_ != nullptr && ptr->parent_->left_ == ptr) {
            ptr = ptr->parent_;
          }
          if (ptr == m->root) {
            // begin()--
            throw invalid_iterator();
          } else {
            ptr = ptr->parent_;
          }
        }
        return *this;
      }

      value_type &operator*() const { return ptr->value_; }

      bool operator==(const iterator &rhs) const {
        return ptr == rhs.ptr && m == rhs.m;
      }

      bool operator==(const const_iterator &rhs) const {
        return ptr == rhs.ptr && m == rhs.m;
      }

      bool operator!=(const iterator &rhs) const {
        return ptr != rhs.ptr || m != rhs.m;
      }

      bool operator!=(const const_iterator &rhs) const {
        return ptr != rhs.ptr || m != rhs.m;
      }

      value_type *operator->() const noexcept {
        if (ptr == nullptr) {
          throw invalid_iterator();
        }
        return &(ptr->value_);
      }
    };

    map() = default;

    Node *copy_node(Node *&a, Node *b) {
      if (b == nullptr) {
        return nullptr;
      }
      a = new Node(b->value_, b->height_);
      Node *l_tmp = copy_node(a->left_, b->left_);
      Node *r_tmp = copy_node(a->right_, b->right_);
      a->left_ = l_tmp;
      a->right_ = r_tmp;
      if (l_tmp != nullptr)
        l_tmp->parent_ = a;
      if (r_tmp != nullptr)
        r_tmp->parent_ = a;
      return a;
    }

    void delete_node(Node *&x) {
      if (x == nullptr) {
        return;
      }
      delete_node(x->left_);
      delete_node(x->right_);
      delete x;
      x = nullptr;
    }

    map(const map &other) {
      map_size = other.map_size;
      copy_node(root, other.root);
    }

    map &operator=(const map &other) {
      if (this == &other) {
        return *this;
      }
      delete_node(root);
      map_size = other.map_size;
      copy_node(root, other.root);
      return *this;
    }

    ~map() { delete_node(root); }

    T &at(const Key &key) {
      if (empty())
        throw index_out_of_bound();
      Node *tmp = root->find_k(key);
      if (tmp == nullptr)
        throw index_out_of_bound();
      return tmp->get_val();
    }

    const T &at(const Key &key) const {
      if (empty())
        throw index_out_of_bound();
      Node *tmp = root->find_k(key);
      if (tmp == nullptr)
        throw index_out_of_bound();
      return tmp->get_val();
    }

    /**
     * access specified element
     * Returns a reference to the value that is mapped to a key equivalent to
     * key, performing an insertion if such key does not already exist.
     */
    T &operator[](const Key &key) {
      auto tmp = find(key);
      if (tmp == end()) {
        map_size++;
        root = insert_node(root, nullptr, key, T());
        return find(key)->second;
      } else {
        return tmp->second;
      }
    }

    /**
     * behave like at() throw index_out_of_bound if such key does not exist.
     */
    const T &operator[](const Key &key) const { return at(key); }

    /**
     * return a iterator to the beginning
     */
    iterator begin() {
      if (root == nullptr)
        return end();
      auto tmp = get_left(root);
      return iterator(tmp, this);
    }

    const_iterator cbegin() const {
      if (root == nullptr)
        return cend();
      auto tmp = get_left(root);
      return const_iterator(tmp, this);
    }

    /**
     * return a iterator to the end
     * in fact, it returns past-the-end.
     */
    iterator end() { return iterator(nullptr, this); }

    const_iterator cend() const { return const_iterator(nullptr, this); }

    /**
     * checks whether the container is empty
     * return true if empty, otherwise false.
     */
    bool empty() const { return map_size == 0; }

    /**
     * returns the number of elements.
     */
    size_t size() const { return map_size; }

    /**
     * clears the contents
     */
    void clear() {
      delete_node(root);
      root = nullptr;
      map_size = 0;
    }

    /**
     * insert an element.
     * return a pair, the first of the pair is
     *   the iterator to the new element (or the element that prevented the
     * insertion), the second one is true if insert successfully, or false.
     */
    pair<iterator, bool> insert(const value_type &value) {
      auto it = find(value.first);
      if (it != end()) {
        return pair<iterator, bool>(it, false);
      } else {
        map_size++;
        root = insert_node(root, nullptr, value.first, value.second);
        auto new_it = find(value.first);
        return pair<iterator, bool>(new_it, true);
      }
    }

    /**
     * erase the element at pos.
     *
     * throw if pos pointed to a bad element (pos == this->end() || pos points
     * an element out of this)
     */
    void erase(iterator pos) {
      if (pos.m->root != root || pos.ptr == nullptr) {
        throw invalid_iterator();
      }
      auto k = pos->first;
      map_size--;
      root = remove_node(root, nullptr, k);
    }

    /**
     * Returns the number of elements with key
     *   that compares equivalent to the specified argument,
     *   which is either 1 or 0
     *     since this container does not allow duplicates.
     * The default method of check the equivalence is !(a < b || b > a)
     */
    size_t count(const Key &key) const {
      if (root == nullptr) {
        return 0;
      }
      if (root->find_k(key) == nullptr) {
        return 0;
      }
      return 1;
    }

    /**
     * Finds an element with key equivalent to key.
     * key value of the element to search for.
     * Iterator to an element with key equivalent to key.
     *   If no such element is found, past-the-end (see end()) iterator is
     * returned.
     */
    iterator find(const Key &key) {
      if (root == nullptr) {
        return end();
      }
      auto tmp = root->find_k(key);
      return iterator(tmp, this);
    }

    const_iterator find(const Key &key) const {
      if (root == nullptr) {
        return cend();
      }
      auto tmp = root->find_k(key);
      return const_iterator(tmp, this);
    }

  private:
    Node *root = nullptr;
    size_t map_size = 0;
  };
}

#endif
