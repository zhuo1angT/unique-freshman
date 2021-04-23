#ifndef _UNIQUE_SET_H_
#define _UNIQUE_SET_H_

#include <iostream>

namespace unique {
typedef uint64_t size_type;

template <typename T> class set {
private:
  static const bool RED = 1;
  static const bool BLACK = 0;

  class node {
  public:
    T value;
    bool color;

    node *left, *right;
    node() {}
    node(T Value, bool Color) {
      value = Value;
      color = Color;
      left = nullptr;
      right = nullptr;
    }
    ~node() {}
  };

  size_type size_;

  node *root;
  node *leftRotate(node *h);
  node *rightRotate(node *h);
  node *colorFlip(node *h);
  node *insert(node *h, T Value); // the overload of public function
  node *fixUp(node *h);
  node *moveRedRight(node *h);
  node *moveRedLeft(node *h);
  T min(node *h);
  node *eraseMin(node *h);
  node *erase(node *h, T Value); // the overload of public function
  void freeSubtree(node *h);
  bool isRed(node *h);

public:
  set() {
    size_ = 0;
    root = nullptr;
  }
  ~set() {}

  bool empty() const;
  size_type size() const;
  void clear();
  void insert(const T &value);
  size_type erase(const T &value);
  size_type count(const T &value);
};

// private functions

template <typename T> typename set<T>::node *set<T>::leftRotate(node *h) {
  node *x = h->right;
  h->right = x->left;
  x->left = h;
  x->color = x->left->color;
  x->left->color = RED; // In llrb-tree, we only rotate **RED** links
  return x;
}

template <typename T> typename set<T>::node *set<T>::rightRotate(node *h) {
  node *x = h->left;
  h->left = x->right;
  x->right = h;
  x->color = x->right->color;
  x->right->color = RED; // In llrb-tree, we only rotate **RED** links
  return x;
}

template <typename T> typename set<T>::node *set<T>::colorFlip(node *h) {
  h->color = !h->color;
  if (h->left != nullptr)
    h->left->color = !h->left->color;
  if (h->right != nullptr)
    h->right->color = !h->right->color;
  return h;
}

template <typename T> typename set<T>::node *set<T>::insert(node *h, T value) {
  if (h == nullptr)
    return new node(value, RED);

  if (value < h->value) {
    h->left = insert(h->left, value);
  } else if (value > h->value) {
    h->right = insert(h->right, value);
  }

  if (h->right != nullptr)
    if (isRed(h->right))
      h = leftRotate(h);

  if (h->left != nullptr)
    if (isRed(h->left) && isRed(h->left->left))
      h = rightRotate(h);

  if (h->left != nullptr && h->right != nullptr)
    if (isRed(h->left) && isRed(h->right))
      colorFlip(h); // split 4-node on the way down

  return h;
}

template <typename T> typename set<T>::node *set<T>::fixUp(node *h) {
  if (isRed(h->right))
    h = leftRotate(h);

  if (isRed(h->left) && isRed(h->left->left))
    h = rightRotate(h);

  if (isRed(h->left) && isRed(h->right))
    h = colorFlip(h);

  return h;
}

template <typename T> typename set<T>::node *set<T>::moveRedRight(node *h) {
  colorFlip(h);
  if (h->left != nullptr) {
    if (isRed(h->left->left)) {
      h = rightRotate(h);
      colorFlip(h);
    }
  }
  return h;
}

template <typename T> typename set<T>::node *set<T>::moveRedLeft(node *h) {
  colorFlip(h);
  if (h->right != nullptr) {
    if (isRed(h->right->left)) {
      h->right = rightRotate(h->right);
      h = leftRotate(h);
      colorFlip(h);
    }
  }
  return h;
}

template <typename T> T set<T>::min(node *h) {
  node *x = h;
  while (x->left != nullptr) {
    x = x->left;
  }
  return x->value;
}

template <typename T> typename set<T>::node *set<T>::eraseMin(node *h) {
  if (h == nullptr)
    return nullptr;
  if (h->left == nullptr) {
    delete h;
    return nullptr;
  }
  if (h->left != nullptr)
    if (!isRed(h->left) && !isRed(h->left->left))
      h = moveRedLeft(h);
  h->left = eraseMin(h->left);

  return fixUp(h);
}

template <typename T> typename set<T>::node *set<T>::erase(node *h, T value) {
  if (value < h->value) {
    if (!isRed(h->left) && !isRed(h->left->left))
      h = moveRedLeft(h);
    h->left = erase(h->left, value);
  } else { // value >= h->value
    if (h->left != nullptr)
      if (isRed(h->left))
        h = rightRotate(h);
    if (value == h->value && h->right == nullptr)
      return nullptr;
    if (h->right != nullptr)
      if (!isRed(h->right) && !isRed(h->right->left))
        h = moveRedRight(h);
    if (value == h->value) {
      h->value = min(h->right);
      h->right = eraseMin(h->right);
    } else {
      h->right = erase(h->right, value);
    }
  }
  return fixUp(h);
}

template <typename T> bool set<T>::isRed(node *h) {
  if (h == nullptr)
    return false;
  return h->color == RED;
}

template <typename T> void set<T>::freeSubtree(node *h) {
  if (h == nullptr)
    return;
  if (h->left != nullptr)
    freeSubtree(h->left);
  if (h->right != nullptr)
    freeSubtree(h->right);
  delete h;
}

// public functions

template <typename T> bool set<T>::empty() const { return size_ == 0; }

template <typename T> size_type set<T>::size() const { return size_; }

template <typename T> void set<T>::clear() {
  size_ = (size_type)0;
  freeSubtree(root);
}

template <typename T> void set<T>::insert(const T &value) {
  size_ += (size_type)(count(value) == 0);
  root = insert(root, value);
  root->color = BLACK;
}

template <typename T> size_type set<T>::erase(const T &value) {
  size_type ret_value = count(value);
  size_ -= ret_value;
  if (ret_value != (size_type)0) {
    root = erase(root, value);
    if (root != nullptr)
      root->color = BLACK;
  }
  return ret_value;
}

template <typename T> size_type set<T>::count(const T &value) {
  node *cur = root;
  while (cur != nullptr) {
    if (value < cur->value) {
      cur = cur->left;
    } else if (value == cur->value) {
      return (size_type)1;
    } else if (value > cur->value) {
      cur = cur->right;
    }
  }
  return (size_type)0;
}

} // namespace unique

#endif