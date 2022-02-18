#pragma once

#include <initializer_list>
#include <deque>

enum BalanceStates {
    Balanced = 0,
    LeftBiggerOne = 1,
    LeftBiggerTwo = 2,
    RightBiggerOne = 3,
    RightBiggerTwo = 4,
    Other = 5
};

template<class ValueType>
class Set {
private:
    class Node;

    std::size_t size_ = 0;
    Node *root = nullptr;
public:
    class iterator {
    public:
        iterator() {}

        explicit iterator(Node *node, const Set *owner) : node(node), owner(owner) {}

        bool operator==(const iterator &other) const {
            return node == other.node;
        }

        bool operator!=(const iterator &other) const {
            return node != other.node;
        }

        ValueType operator*() const {
            return node->GetData();
        };

        const ValueType *operator->() const {
            return node->GetDataAdress();
        }

        iterator operator++() {
            if (node->GetRight() == nullptr) {
                bool ok;
                do {
                    if (node->GetParent() == nullptr) {
                        node = nullptr;
                        break;
                    }
                    ok = node->GetParent()->GetRight() == node;
                    node = node->GetParent();
                } while (ok);
            } else if (node->GetRight() != nullptr) {
                node = node->GetRight();
                while (node->GetLeft() != nullptr) {
                    node = node->GetLeft();
                }
            }
            return *this;
        }

        iterator operator++(int) {
            ConstIterator old_node = *this;
            if (node->GetRight() == nullptr) {
                bool ok;
                do {
                    if (node->GetParent() == nullptr) {
                        node = nullptr;
                        break;
                    }
                    ok = node->GetParent()->GetRight() == node;
                    node = node->GetParent();
                } while (ok);
            } else if (node->GetRight() != nullptr) {
                node = node->GetRight();
                while (node->GetLeft() != nullptr) {
                    node = node->GetLeft();
                }
            }
            return old_node;
        }

        iterator operator--() {
            if (node == nullptr) {
                node = owner->root;
                while (node->GetRight() != nullptr) {
                    node = node->GetRight();
                }
            } else {
                if (node->GetLeft() == nullptr) {
                    bool ok;
                    do {
                        if (node->GetParent() == nullptr) {
                            node = nullptr;
                            break;
                        }
                        ok = node->GetParent()->GetLeft() == node;
                        node = node->GetParent();
                    } while (ok);
                } else if (node->GetLeft() != nullptr) {
                    node = node->GetLeft();
                    while (node->GetRight() != nullptr) {
                        node = node->GetRight();
                    }
                }
            }
            return *this;
        }

        iterator operator--(int) {
            ConstIterator old_node = *this;
            if (node == nullptr) {
                node = owner->root;
                while (node->GetRight() != nullptr) {
                    node = node->GetRight();
                }
            } else {
                if (node->GetLeft() == nullptr) {
                    bool ok;
                    do {
                        if (node->GetParent() == nullptr) {
                            node = nullptr;
                            break;
                        }
                        ok = node->GetParent()->GetLeft() == node;
                        node = node->GetParent();
                    } while (ok);
                } else if (node->GetLeft() != nullptr) {
                    node = node->GetLeft();
                    while (node->GetRight() != nullptr) {
                        node = node->GetRight();
                    }
                }
            }
            return old_node;
        }

    private:
        const Node *node;
        const Set *owner;
    };

    using ConstIterator = iterator;

    Set() {}

    Set(std::initializer_list <ValueType> elems) {
        for (const auto &i : elems) {
            insert(i);
        }
    }

    template<typename It>
    Set(It first, It last) {
        for (auto &it = first; it != last; ++it) {
            insert(*it);
        }
    }

    Set(Set &set) {
        Node *set_root = set.root;
        size_ = set.size_;
        if (set_root == nullptr) {
            root = nullptr;
            return;
        }
        root = new Node(set_root->GetData());
        std::deque <std::pair<Node *, Node *>> q;
        q.push_back({set_root, root});
        while (!q.empty()) {
            auto it = q.back();
            q.pop_back();
            if (it.first->GetLeft() != nullptr) {
                it.second->SetLeft(new Node(it.first->GetLeft()->GetData()));
                q.push_back({it.first->GetLeft(), it.second->GetLeft()});
            }
            if (it.first->GetRight() != nullptr) {
                it.second->SetRight(new Node(it.first->GetRight()->GetData()));
                q.push_back({it.first->GetRight(), it.second->GetRight()});
            }
        }
        UpdateTree(root);
    }

    Set &operator=(const Set &set) {
        if (this == &set) {
            return *this;
        }
        if (root != nullptr) {
            std::deque < Node * > d;
            d.push_back(root);
            while (!d.empty()) {
                auto it = d.back();
                d.pop_back();
                if (it->GetLeft() != nullptr) {
                    d.push_back(it->GetLeft());
                }
                if (it->GetRight() != nullptr) {
                    d.push_back(it->GetRight());
                }
                delete it;
            }
        }
        Node *set_root = set.root;
        size_ = set.size_;
        if (set_root == nullptr) {
            root = nullptr;
            return *this;
        }
        root = new Node(set_root->GetData());
        std::deque <std::pair<Node *, Node *>> q;
        q.push_back({set_root, root});
        while (!q.empty()) {
            auto it = q.back();
            q.pop_back();
            if (it.first->GetLeft() != nullptr) {
                it.second->SetLeft(new Node(it.first->GetLeft()->GetData()));
                q.push_back({it.first->GetLeft(), it.second->GetLeft()});
            }
            if (it.first->GetRight() != nullptr) {
                it.second->SetRight(new Node(it.first->GetRight()->GetData()));
                q.push_back({it.first->GetRight(), it.second->GetRight()});
            }
        }
        UpdateTree(root);
        return *this;
    }

    ~Set() {
        if (root == nullptr) {
            return;
        }
        std::deque < Node * > d;
        d.push_back(root);
        while (!d.empty()) {
            auto it = d.back();
            d.pop_back();
            if (it->GetLeft() != nullptr) {
                d.push_back(it->GetLeft());
            }
            if (it->GetRight() != nullptr) {
                d.push_back(it->GetRight());
            }
            delete it;
        }
    }

    std::size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    void insert(const ValueType &el) {
        ++size_;
        if (root == nullptr) {
            root = new Node(el);
            return;
        }
        Node *now = root;
        while (now != nullptr) {
            if (el < now->GetData()) {
                if (now->GetLeft() == nullptr) {
                    now->SetLeft(new Node(el));
                    now = now->GetLeft();
                    break;
                } else {
                    now = now->GetLeft();
                }
            } else if (now->GetData() < el) {
                if (now->GetRight() == nullptr) {
                    now->SetRight(new Node(el));
                    now = now->GetRight();
                    break;
                }
                now = now->GetRight();
            } else {
                size_--;
                break;
            }
        }
        BalanceTree(now);
    }

    ConstIterator find(const ValueType &el) const {
        if (root == nullptr) {
            return ConstIterator(nullptr, this);
        }
        Node *now = root;
        while (now != nullptr) {
            if (el < now->GetData()) {
                if (now->GetLeft() == nullptr) {
                    return ConstIterator(nullptr, this);
                } else {
                    now = now->GetLeft();
                }
            } else if (now->GetData() < el) {
                if (now->GetRight() == nullptr) {
                    return ConstIterator(nullptr, this);
                }
                now = now->GetRight();
            } else {
                return ConstIterator(now, this);
            }
        }
        return ConstIterator(nullptr, this);
        //throw std::exception();
    }

    void erase(const ValueType &el) {
        if (root == nullptr) {
            return;
        }
        Node *now = root;
        while (now != nullptr) {
            if (el < now->GetData()) {
                if (now->GetLeft() == nullptr) {
                    return;
                } else {
                    now = now->GetLeft();
                }
            } else if (now->GetData() < el) {
                if (now->GetRight() == nullptr) {
                    return;
                }
                now = now->GetRight();
            } else {
                break;
            }
        }
        if (!now->IsLeaf()) {
            Node *swap = now;
            BalanceStates balance = now->GetBalance();
            if (balance == BalanceStates::Balanced || balance == BalanceStates::LeftBiggerOne) {
                swap = swap->GetLeft();
                bool was = false;
                while (swap->GetRight() != nullptr) {
                    was = true;
                    swap = swap->GetRight();
                }
                if (swap->GetLeft() != nullptr && swap->GetParent() != nullptr) {
                    if (was) {
                        swap->GetParent()->SetRight(swap->GetLeft());
                    } else {
                        swap->GetParent()->SetLeft(swap->GetLeft());
                    }
                }
            } else if (balance == BalanceStates::RightBiggerOne) {
                swap = swap->GetRight();
                bool was = false;
                while (swap->GetLeft() != nullptr) {
                    was = true;
                    swap = swap->GetLeft();
                }
                if (swap->GetRight() != nullptr && swap->GetParent() != nullptr) {
                    if (!was) {
                        swap->GetParent()->SetRight(swap->GetRight());
                    } else {
                        swap->GetParent()->SetLeft(swap->GetRight());
                    }
                }
            } else {
                //throw std::exception();
            }
            ValueType sw = swap->GetData();
            swap->SetData(now->GetData());
            now->SetData(sw);
            now = swap;
        }
        if (now->GetParent() == nullptr) {
            root = nullptr;
        } else {
            if (now->GetParent()->GetLeft() == now) {
                now->GetParent()->SetLeft(nullptr);
            } else if (now->GetParent()->GetRight() == now) {
                now->GetParent()->SetRight(nullptr);
            }
        }
        BalanceTree(now->GetParent());
        --size_;
        delete now;
    }

    ConstIterator lower_bound(const ValueType &el) const {
        if (root == nullptr) {
            return iterator(nullptr, this);
        }
        Node *now = root;
        while (now != nullptr) {
            if (el < now->GetData()) {
                if (now->GetLeft() == nullptr || now->GetLeft()->GetMax() < el) {
                    return ConstIterator(now, this);
                } else {
                    now = now->GetLeft();
                }
            } else if (now->GetData() < el) {
                if (now->GetRight() == nullptr) {
                    return ConstIterator(nullptr, this);
                }
                now = now->GetRight();
            } else {
                return ConstIterator(now, this);
            }
        }
        //throw std::exception();
        return ConstIterator(nullptr, this);
    }

    ConstIterator begin() const {
        Node *node = root;
        if (node == nullptr) {
            return iterator(node, this);
        }
        while (node->GetLeft() != nullptr) {
            node = node->GetLeft();
            node->UpdateNode();
        }
        return iterator(node, this);
    }

    ConstIterator end() const {
        return ConstIterator(nullptr, this);
    }

private:
    void UpdateTree(Node *node) {
        if (node == nullptr) {
            return;
        }
        UpdateTree(node->GetLeft());
        UpdateTree(node->GetRight());
        node->UpdateNode();
    }

    void BalanceTree(Node *node) {
        if (node == nullptr) {
            return;
        }
        BalanceStates balance = node->GetBalance();
        Node *parent = node->GetParent();
        if (balance == Other) {
            //throw std::exception();
        } else if (balance == LeftBiggerTwo) {
            if (node->GetLeftLeftHeight() >= node->GetLeftRightHeight()) {
                if (parent == nullptr) {
                    root = node->SmallRightRotation();
                    root->SetParent(nullptr);
                } else if (parent->GetLeft() == node) {
                    parent->SetLeft(node->SmallRightRotation());
                } else {
                    parent->SetRight(node->SmallRightRotation());
                }
            } else {
                if (parent == nullptr) {
                    root = node->BigRightRotation();
                    root->SetParent(nullptr);
                } else if (parent->GetLeft() == node) {
                    parent->SetLeft(node->BigRightRotation());
                } else {
                    parent->SetRight(node->BigRightRotation());
                }
            }
        } else if (balance == RightBiggerTwo) {
            if (node->GetRightRightHeight() >= node->GetRightLeftHeight()) {
                if (parent == nullptr) {
                    root = node->SmallLeftRotation();
                    root->SetParent(nullptr);
                } else if (parent->GetLeft() == node) {
                    parent->SetLeft(node->SmallLeftRotation());
                } else {
                    parent->SetRight(node->SmallLeftRotation());
                }
            } else {
                if (parent == nullptr) {
                    root = node->BigLeftRotation();
                    root->SetParent(nullptr);
                } else if (parent->GetLeft() == node) {
                    parent->SetLeft(node->BigLeftRotation());
                } else {
                    parent->SetRight(node->BigLeftRotation());
                }
            }
        }
        node->UpdateNode();
        if (parent != nullptr) {
            parent->UpdateNode();
        }
        BalanceTree(node->GetParent());
    }

    class Node {
    private:
        Node *left = nullptr;
        Node *right = nullptr;
        Node *parent = nullptr;
        ValueType data_;
        std::size_t height = 0;
        ValueType min_;
        ValueType max_;
    public:
        void UpdateNode() {
            height = 0;
            min_ = data_;
            max_ = data_;
            if (!IsLeaf()) {
                if (left != nullptr) {
                    min_ = left->GetMin();
                    height = std::max(height, left->GetHeight());
                }
                if (right != nullptr) {
                    max_ = right->GetMax();
                    height = std::max(height, right->GetHeight());
                }
            }
            ++height;
        }

        void SetData(ValueType data) {
            data_ = data;
        }

        bool operator==(const Node &other) const {
            return &this == &other;
        }

        bool operator!=(const Node &other) const {
            return &this != &other;
        }

        Node() {}

        explicit Node(const ValueType &el) : data_(el) {}

        Node(Node *l, Node *r) : left(l), right(r) {}

        BalanceStates GetBalance() {
            std::size_t left_height = 0;
            if (GetLeft() != nullptr) {
                left_height = GetLeftHeight();
            }
            std::size_t right_height = 0;
            if (GetRight() != nullptr) {
                right_height = GetRightHeight();
            }
            if (left_height == right_height) {
                return Balanced;
            } else if (left_height > right_height) {
                if (left_height - right_height == 1) {
                    return LeftBiggerOne;
                } else if (left_height - right_height == 2) {
                    return LeftBiggerTwo;
                } else {
                    return Other;
                }
            } else {
                if (right_height - left_height == 1) {
                    return RightBiggerOne;
                } else if (right_height - left_height == 2) {
                    return RightBiggerTwo;
                } else {
                    return Other;
                }
            }
        }

        std::size_t GetRightRightHeight() const {
            std::size_t h = 0;
            if (GetRight() != nullptr) {
                h = GetRight()->GetRightHeight();
            }
            return h;
        }

        std::size_t GetRightLeftHeight() const {
            std::size_t h = 0;
            if (GetRight() != nullptr) {
                h = GetRight()->GetLeftHeight();
            }
            return h;
        }

        std::size_t GetLeftRightHeight() const {
            std::size_t h = 0;
            if (GetLeft() != nullptr) {
                h = GetLeft()->GetRightHeight();
            }
            return h;
        }

        std::size_t GetLeftLeftHeight() const {
            std::size_t h = 0;
            if (GetLeft() != nullptr) {
                h = GetLeft()->GetLeftHeight();
            }
            return h;
        }

        ValueType GetMin() const {
            return min_;
        }

        ValueType GetMax() const {
            return max_;
        }

        std::size_t GetRightHeight() const {
            std::size_t h = 0;
            if (GetRight() != nullptr) {
                h = GetRight()->GetHeight();
            }
            return h;
        }

        std::size_t GetLeftHeight() const {
            std::size_t h = 0;
            if (GetLeft() != nullptr) {
                h = GetLeft()->GetHeight();
            }
            return h;
        }

        std::size_t GetHeight() const {
            return height;
        }

        Node *GetParent() const {
            return parent;
        }

        Node *GetLeft() const {
            return left;
        }

        Node *GetRight() const {
            return right;
        }

        bool IsLeaf() const {
            return left == nullptr && right == nullptr;
        }

        ValueType GetData() const {
            return data_;
        }

        const ValueType *GetDataAdress() const {
            return &data_;
        }

        void SetParent(Node *node) {
            parent = node;
        }

        void SetLeft(Node *node) {
            UpdateNode();
            if (node != nullptr) {
                node->SetParent(this);
            }
            left = node;
        }

        void SetRight(Node *node) {
            UpdateNode();
            if (node != nullptr) {
                node->SetParent(this);
            }
            right = node;
        }

        Node *SmallLeftRotation() {
            if (this->IsLeaf()) {
                return this;
            }
            Node *new_node = this->GetRight();
            this->SetRight(new_node->GetLeft());
            new_node->SetLeft(this);
            UpdateNode();
            new_node->UpdateNode();
            return new_node;
        }

        Node *SmallRightRotation() {
            if (this->IsLeaf()) {
                return this;
            }
            Node *new_node = this->GetLeft();
            this->SetLeft(new_node->GetRight());
            new_node->SetRight(this);
            UpdateNode();
            new_node->UpdateNode();
            return new_node;
        }

        Node *BigLeftRotation() {
            Node *new_node = this->right->SmallRightRotation();
            this->SetRight(new_node);
            UpdateNode();
            new_node->UpdateNode();
            return SmallLeftRotation();
        }

        Node *BigRightRotation() {
            Node *new_node = this->left->SmallLeftRotation();
            this->SetLeft(new_node);
            UpdateNode();
            new_node->UpdateNode();
            return SmallRightRotation();
        }
    };
};
