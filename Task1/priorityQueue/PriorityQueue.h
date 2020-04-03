#ifndef _UNIQUE_PRIORITY_QUEUE_H_
#define _UNIQUE_PRIORITY_QUEUE_H

#include <cstring>
#include <iostream>

using std::log;

namespace unique {

typedef uint64_t size_type;

template <typename T>
class priority_queue {
   private:
    class node {
       public:
        T value;
        int degree;
        node *left, *right, *parent, *child;

        node(T Value) {
            value = Value;
            left = right = nullptr;
            parent = child = nullptr;
            degree = 1;
        }
        node(node *ptr) {
            value = ptr->value;
            child = ptr->child;
            degree = ptr->degree;
            parent = left = right = nullptr;
        }
    };

    node *min_node;
    size_type size_;

    void nodeSwap(node *a, node *b) {
        if (a->left != nullptr && a->left != b) a->left->right = b;
        if (a->right != nullptr && a->right != b) a->right->left = b;
        if (b->left != nullptr && b->left != a) b->left->right = a;
        if (b->right != nullptr && b->right != a) b->right->left = a;

        if (a->left != nullptr && a->left == b) {
            node *temp = a->right;
            a->right = b;
            a->left = b->left;
            b->left = a;
            b->right = temp;
        } else if (a->right != nullptr && a->right == b) {
            node *temp = b->right;
            b->right = a;
            b->left = a->left;
            a->left = b;
            a->right = temp;
        } else {
            std::swap(a->left, b->left);
            std::swap(a->right, b->right);
        }
    }

    void insertRight(node *org, node *cur) {
        if (org != nullptr && cur != nullptr) {
            if (org->right == nullptr) {
                org->right = cur;
                cur->left = org;
            } else {
                cur->left = org;
                cur->right = org->right;
                org->right->left = cur;
                org->right = cur;
            }
        }
    }

    void deleteMinNode() {
        if (min_node->left != nullptr) min_node->left->right = min_node->right;
        if (min_node->right != nullptr) min_node->right->left = min_node->left;
        if (min_node->right == nullptr && min_node->left == nullptr) {
            delete min_node;
            min_node = nullptr;
        } else {
            node *temp = min_node->right;
            delete min_node;
            min_node = temp;
        }
    }
    void maintainMin(node *left_most_node);
    void consolidate();

   public:
    priority_queue() {
        min_node = nullptr;
        size_ = 0;
    }
    ~priority_queue() {}

    const T &top() const;
    bool empty() const;
    size_type size() const;
    void push(const T &value);
    void pop();
};

// private functions

template <typename T>
void priority_queue<T>::consolidate() {
    int tot_log_degree = (int)(1 + (1 + log(size_) / log(2)));

    node **tree_size = new node *[tot_log_degree];
    memset(tree_size, 0, sizeof(node *) * tot_log_degree);

    unique::priority_queue<T> consol;
    node *cur_node = min_node;  // actually the left-most node

    while (cur_node != nullptr) {
        node *_old = new node(cur_node);

        int log_degree = (int)(1 + log(cur_node->degree) / log(2));

        if (tree_size[log_degree] == nullptr) {
            if (consol.min_node == nullptr) {
                consol.min_node = _old;
                consol.min_node->left = nullptr;
                consol.min_node->right = nullptr;
            } else {
                consol.insertRight(consol.min_node, _old);

                if (_old->value < consol.min_node->value)
                    consol.min_node = _old;
            }
            tree_size[log_degree] = _old;
        } else {
            node *_new0 = tree_size[log_degree];

            if (_old->value < _new0->value) {
                if (_old->child != nullptr) _old->child->parent = _new0;
                if (_new0->child != nullptr) _new0->child->parent = _old;
                std::swap(_old->value, _new0->value);
                std::swap(_old->child, _new0->child);
                std::swap(_old->parent, _new0->parent);
            }

            if (_old->value < consol.min_node->value) consol.min_node = _old;
            if (_new0->value < consol.min_node->value) consol.min_node = _new0;

            if (_new0->child != nullptr) {
                _new0->child->left = _old;
                _old->right = _new0->child;
                _new0->child = _old;
                _old->left = nullptr;
            } else {
                _new0->child = _old;
                _new0->child->left = _new0->child->right = nullptr;
            }

            _old->parent = _new0;
            _old->left = nullptr;

            tree_size[log_degree] = nullptr;
            _new0->degree *= 2;

            log_degree++;

            while (tree_size[log_degree] != nullptr) {
                node *_new1 = tree_size[log_degree];
                if (_new1->value < _new0->value) {
                    std::swap(_new1, _new0);
                } else if (_new1 == consol.min_node) {
                    std::swap(_new1, _new0);
                    consol.min_node = _new0;
                }

                if (_new1->left != nullptr) _new1->left->right = _new1->right;
                if (_new1->right != nullptr) _new1->right->left = _new1->left;

                _new0->child->left = _new1;
                _new1->right = _new0->child;
                _new1->left = nullptr;
                _new1->parent = _new0;
                _new0->child = _new1;

                tree_size[log_degree] = nullptr;
                _new0->degree *= 2;

                log_degree++;
            }

            tree_size[log_degree] = _new0;
        }
        cur_node = cur_node->right;
        if (cur_node != nullptr) {
            delete cur_node->left;
            cur_node->left = nullptr;
        }
    }
    min_node = consol.min_node;
    delete tree_size;
}

template <typename T>
void priority_queue<T>::maintainMin(node *left_most_node) {
    node *cur = left_most_node, *min = left_most_node;
    while (cur != nullptr) {
        if (cur->value < min->value) min = cur;
        cur = cur->right;
    }
    nodeSwap(min_node, min);
    min_node = min;
}

// public funtions

template <typename T>
const T &priority_queue<T>::top() const {
    return min_node->value;
}

template <typename T>
bool priority_queue<T>::empty() const {
    return size_ == 0;
}

template <typename T>
size_type priority_queue<T>::size() const {
    return size_;
}

template <typename T>
void priority_queue<T>::push(const T &value) {
    node *new_node = new node(value);
    size_ += 1;
    if (min_node == nullptr) {
        min_node = new_node;
    } else {
        insertRight(min_node, new_node);
        if (value < min_node->value) {
            node *temp = min_node->right;
            nodeSwap(min_node, min_node->right);
            min_node = temp;
        }
    }
}

template <typename T>
void priority_queue<T>::pop() {
    if (size_ == 0) {
        std::cerr << "Error: popping from an empty queue!" << std::endl;
        exit(1);
    }

    size_ -= 1;

    node *cur_child = min_node->child;
    while (cur_child != nullptr) {
        node *next = cur_child->right;
        cur_child->left = cur_child->right = nullptr;
        insertRight(min_node, cur_child);
        cur_child->parent = nullptr;
        cur_child = next;
    }

    deleteMinNode();

    if (min_node == nullptr) return;

    maintainMin(min_node);
    consolidate();

    node *cur = min_node;
    while (cur->left != nullptr) cur = cur->left;
    nodeSwap(min_node, cur);
    min_node = cur;
}

}  // namespace unique

#endif