#ifndef _UNIQUE_PRIORITY_QUEUE_H_
#define _UNIQUE_PRIORITY_QUEUE_H

#include <cstring>
#include <iostream>
#include <map>

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
        node *left, *right;
        node *parent, *child;

        node(T Value) {
            value = Value;
            left = right = nullptr;
            parent = child = nullptr;
            degree = 1;
        }
        node(node *ptr) {
            value = ptr->value;
            left = ptr->left;
            right = ptr->right;
            parent = ptr->parent;
            child = ptr->child;
            degree = ptr->degree;
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
    }

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

    size_++;
}

template <typename T>
void priority_queue<T>::pop() {
    if (size_ == 0) {
        std::cerr << "Error: popping from an empty priority_queue!"
                  << std::endl;
        exit(0);
    }

    node *cur_child = min_node->child;
    while (cur_child != nullptr) {
        node *next = cur_child->right;
        cur_child->left = cur_child->right = nullptr;
        insertRight(min_node, cur_child);
        cur_child->parent = nullptr;
        cur_child = next;
    }
    deleteMinNode();

    if (min_node->right == nullptr && min_node->left == nullptr) {
        delete min_node;
        min_node = nullptr;
    } else {
        node *temp = min_node->right;
        delete min_node;
        min_node = temp;
    }

    if (min_node != nullptr) {
        node *p1 = min_node, *pm = min_node;
        while (p1 != nullptr) {
            if (p1->value < pm->value) {
                pm = p1;
            }
            p1 = p1->right;
        }
        nodeSwap(min_node, pm);
        min_node = pm;
    }

    if (min_node != nullptr) {
        node **tree_size = new node *[40];
        memset(tree_size, 0, sizeof(node *) * 40);

        unique::priority_queue<T> cons;
        node *cur_node = this->min_node;  // actually the left-most node

        while (cur_node != nullptr) {
            node *temp = new node(cur_node);
            temp->parent = temp->left = temp->right = nullptr;

            if (tree_size[(int)(1 + log(cur_node->degree) / log(2))] ==
                nullptr) {
                if (cons.min_node == nullptr) {
                    cons.min_node = temp;
                    cons.min_node->left = nullptr;
                    cons.min_node->right = nullptr;
                } else {
                    cons.insertRight(cons.min_node, temp);

                    if (temp->value < cons.min_node->value)
                        cons.min_node = temp;
                }
                tree_size[(int)(1 + log(cur_node->degree) / log(2))] = temp;
            } else {
                node *cons_node =
                    tree_size[(int)(1 + log(temp->degree) / log(2))];

                if (temp->value < cons_node->value) {
                    if (temp->child != nullptr) temp->child->parent = cons_node;
                    if (cons_node->child != nullptr)
                        cons_node->child->parent = temp;
                    std::swap(temp->value, cons_node->value);
                    std::swap(temp->child, cons_node->child);
                    std::swap(temp->degree, cons_node->degree);
                    std::swap(temp->parent, cons_node->parent);
                }

                if (temp->value < cons.min_node->value) cons.min_node = temp;
                if (cons_node->value < cons.min_node->value)
                    cons.min_node = cons_node;

                if (cons_node->child != nullptr) {
                    cons_node->child->left = temp;
                    temp->right = cons_node->child;
                    cons_node->child = temp;
                    // cons_node->left = nullptr;
                    temp->left = nullptr;
                } else {
                    cons_node->child = temp;
                    cons_node->child->left = cons_node->child->right = nullptr;
                }

                temp->parent = cons_node;
                temp->left = nullptr;

                tree_size[(int)(1 + log(temp->degree) / log(2))] = nullptr;
                cons_node->degree *= 2;

                while (tree_size[(int)(1 + log(cons_node->degree) / log(2))] !=
                       nullptr) {
                    node *cons1_node =
                        tree_size[(int)(1 + log(cons_node->degree) / log(2))];
                    if (cons1_node->value < cons_node->value) {
                        std::swap(cons1_node, cons_node);
                    } else if (cons1_node == cons.min_node) {
                        std::swap(cons1_node, cons_node);
                        cons.min_node = cons_node;
                    }

                    if (cons1_node->left != nullptr)
                        cons1_node->left->right = cons1_node->right;
                    if (cons1_node->right != nullptr)
                        cons1_node->right->left = cons1_node->left;

                    cons_node->child->left = cons1_node;
                    cons1_node->right = cons_node->child;
                    cons1_node->left = nullptr;
                    cons1_node->parent = cons_node;
                    cons_node->child = cons1_node;

                    tree_size[(int)(1 + log(cons_node->degree) / log(2))] =
                        nullptr;
                    cons_node->degree *= 2;
                }

                tree_size[(int)(1 + log(cons_node->degree) / log(2))] =
                    cons_node;
            }
            cur_node = cur_node->right;
            if (cur_node != nullptr) {
                delete cur_node->left;
                cur_node->left = nullptr;
            }
        }
        min_node = cons.min_node;
        delete tree_size;
    }

    size_--;

    node *p = min_node;
    if (p == nullptr) return;
    while (p->left != nullptr) {
        p = p->left;
    }

    nodeSwap(min_node, p);
    min_node = p;
}

}  // namespace unique

#endif