#pragma once

#include <initializer_list>
#include <stack>

/// Balance states for tree
enum BalanceStates {
    Balanced = 0,
    LeftBiggerOne = 1,
    LeftBiggerTwo = 2,
    RightBiggerOne = 3,
    RightBiggerTwo = 4,
    Other = 5
};

template<class ValueType>

/**
 * Set based on AVL (https://ru.wikipedia.org/wiki/%D0%90%D0%92%D0%9B-%D0%B4%D0%B5%D1%80%D0%B5%D0%B2%D0%BE)
 * Implemented erase, find, lower_bound, insert, all O(log size) time
 * @tparam ValueType
 */
class Set {
  private:
    class Node;

    size_t size_ = 0;
    Node* root = nullptr;
  public:
    /**
     * Iterator for Set
     */
    class iterator {
      public:
        iterator() {}

        explicit iterator(Node* node, const Set* owner) : node(node), owner(owner) {}

        bool operator==(const iterator& other) const {
            return node == other.node;
        }

        bool operator!=(const iterator& other) const {
            return node != other.node;
        }

        ValueType operator*() const {
            return node->GetData();
        };

        const ValueType* operator->() const {
            return node->GetDataAdress();
        }

        iterator operator++() {
            if (node->GetRight() == nullptr) {
                bool ok = true;
                while (ok) {
                    if (node->GetParent() == nullptr) {
                        node = nullptr;
                        break;
                    }
                    ok = node->GetParent()->GetRight() == node;
                    node = node->GetParent();
                }
            } else if (node->GetRight() != nullptr) {
                node = node->GetRight();
                while (node->GetLeft() != nullptr) {
                    node = node->GetLeft();
                }
            }
            return *this;
        }

        iterator operator++(int) {
            if (node->GetRight() == nullptr) {
                bool ok = true;
                while (ok) {
                    if (node->GetParent() == nullptr) {
                        node = nullptr;
                        break;
                    }
                    ok = node->GetParent()->GetRight() == node;
                    node = node->GetParent();
                }
            } else if (node->GetRight() != nullptr) {
                node = node->GetRight();
                while (node->GetLeft() != nullptr) {
                    node = node->GetLeft();
                }
            }
            return *this;
        }

        iterator operator--() {
            if (node == nullptr) {
                node = owner->root;
                while (node->GetRight() != nullptr) {
                    node = node->GetRight();
                }
            } else if (node->GetLeft() == nullptr) {
                bool ok = true;
                while (ok) {
                    if (node->GetParent() == nullptr) {
                        node = nullptr;
                        break;
                    }
                    ok = node->GetParent()->GetLeft() == node;
                    node = node->GetParent();
                };
            } else if (node->GetLeft() != nullptr) {
                node = node->GetLeft();
                while (node->GetRight() != nullptr) {
                    node = node->GetRight();
                }
            }
            return *this;
        }

        iterator operator--(int) {
            if (node == nullptr) {
                node = owner->root;
                while (node->GetRight() != nullptr) {
                    node = node->GetRight();
                }
            } else if (node->GetLeft() == nullptr) {
                bool ok = true;
                while (ok) {
                    if (node->GetParent() == nullptr) {
                        node = nullptr;
                        break;
                    }
                    ok = node->GetParent()->GetLeft() == node;
                    node = node->GetParent();
                }
            } else if (node->GetLeft() != nullptr) {
                node = node->GetLeft();
                while (node->GetRight() != nullptr) {
                    node = node->GetRight();
                }
            }
            return *this;
        }

      private:
        const Node* node;
        const Set* owner;
    };


    Set() {}

    Set(std::initializer_list <ValueType> elems) {
        for (const auto& i : elems) {
            insert(i);
        }
    }

    Set(std::initializer_list <ValueType>& elems) {
        for (const auto& i : elems) {
            insert(i);
        }
    }

    template<typename It>
    Set(It first, It last) {
        for (auto& it = first; it != last; ++it) {
            insert(*it);
        }
    }

    Set(Set& other) {
        Node* set_root = other.root;
        size_ = other.size_;
        if (set_root == nullptr) {
            root = nullptr;
            return;
        }
        root = new Node(set_root->GetData());
        std::stack <std::pair<Node*, Node*>> s;
        s.push({set_root, root});
        while (!s.empty()) {
            auto it = s.top();
            s.pop();
            if (it.first->GetLeft() != nullptr) {
                it.second->SetLeft(new Node(it.first->GetLeft()->GetData()));
                s.push({it.first->GetLeft(), it.second->GetLeft()});
            }
            if (it.first->GetRight() != nullptr) {
                it.second->SetRight(new Node(it.first->GetRight()->GetData()));
                s.push({it.first->GetRight(), it.second->GetRight()});
            }
        }
        UpdateTree(root);
    }

    Set& operator=(const Set& set) {
        if (this == &set) {
            return *this;
        }
        if (root != nullptr) {
            std::stack < Node * > s;
            s.push(root);
            while (!s.empty()) {
                auto it = s.top();
                s.pop();
                if (it->GetLeft() != nullptr) {
                    s.push(it->GetLeft());
                }
                if (it->GetRight() != nullptr) {
                    s.push(it->GetRight());
                }
                delete it;
            }
        }
        Node* set_root = set.root;
        size_ = set.size_;
        if (set_root == nullptr) {
            root = nullptr;
            return *this;
        }
        root = new Node(set_root->GetData());
        std::stack <std::pair<Node*, Node*>> s;
        s.push({set_root, root});
        while (!s.empty()) {
            auto it = s.top();
            s.pop();
            if (it.first->GetLeft() != nullptr) {
                it.second->SetLeft(new Node(it.first->GetLeft()->GetData()));
                s.push({it.first->GetLeft(), it.second->GetLeft()});
            }
            if (it.first->GetRight() != nullptr) {
                it.second->SetRight(new Node(it.first->GetRight()->GetData()));
                s.push({it.first->GetRight(), it.second->GetRight()});
            }
        }
        UpdateTree(root);
        return *this;
    }

    ~Set() {
        if (root == nullptr) {
            return;
        }
        std::stack < Node * > s;
        s.push(root);
        while (!s.empty()) {
            auto it = s.top();
            s.pop();
            if (it->GetLeft() != nullptr) {
                s.push(it->GetLeft());
            }
            if (it->GetRight() != nullptr) {
                s.push(it->GetRight());
            }
            delete it;
        }
    }

    /// Function returns count of elements in the set
    size_t size() const {
        return size_;
    }

    /// Function returns true if there are no elements in the set
    bool empty() const {
        return size_ == 0;
    }

    /// Function inserts element in set, O(log size) time
    void insert(const ValueType& el) {
        ++size_;
        if (root == nullptr) {
            root = new Node(el);
            return;
        }
        Node* now = root;
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

    /// Function finds the element in set and returns iterator(nullptr if no element), O(log size) time
    iterator find(const ValueType& el) const {
        if (root == nullptr) {
            return iterator(nullptr, this);
        }
        Node* now = root;
        while (now != nullptr) {
            if (el < now->GetData()) {
                if (now->GetLeft() == nullptr) {
                    return iterator(nullptr, this);
                } else {
                    now = now->GetLeft();
                }
            } else if (now->GetData() < el) {
                if (now->GetRight() == nullptr) {
                    return iterator(nullptr, this);
                }
                now = now->GetRight();
            } else {
                return iterator(now, this);
            }
        }
        return iterator(nullptr, this);
    }

    /// Function erases element from set, if no such element does nothing, O(log size) time
    void erase(const ValueType& el) {
        if (root == nullptr) {
            return;
        }
        Node* now = Find(root, el);
        if (now == nullptr) {
            return;
        }
        if (!now->IsLeaf()) {
            Node* swap = NodeToLeaf(now, now->GetBalance());
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

    /// Function returns iterator on the first element >= given(nullptr if there is no such element), O(log size) time
    /// (https://www.cplusplus.com/reference/algorithm/lower_bound/)
    iterator lower_bound(const ValueType& el) const {
        if (root == nullptr) {
            return iterator(nullptr, this);
        }
        Node* now = root;
        while (now != nullptr) {
            if (el < now->GetData()) {
                if (now->GetLeft() == nullptr || now->GetLeft()->GetMax() < el) {
                    return iterator(now, this);
                } else {
                    now = now->GetLeft();
                }
            } else if (now->GetData() < el) {
                if (now->GetRight() == nullptr) {
                    return iterator(nullptr, this);
                }
                now = now->GetRight();
            } else {
                return iterator(now, this);
            }
        }
        return iterator(nullptr, this);
    }

    /// Function returns iterator on the first(the smallest) element, O(log size) time
    iterator begin() const {
        Node* node = root;
        if (node == nullptr) {
            return iterator(node, this);
        }
        while (node->GetLeft() != nullptr) {
            node = node->GetLeft();
            node->UpdateNode();
        }
        return iterator(node, this);
    }

    iterator end() const {
        return iterator(nullptr, this);
    }

  private:
    Node* Find(Node* now, const ValueType& el) {
        while (now != nullptr) {
            if (el < now->GetData()) {
                if (now->GetLeft() == nullptr) {
                    return nullptr;
                } else {
                    now = now->GetLeft();
                }
            } else if (now->GetData() < el) {
                if (now->GetRight() == nullptr) {
                    return nullptr;
                }
                now = now->GetRight();
            } else {
                break;
            }
        }
        return now;
    }

    Node* NodeToLeaf(Node* swap, BalanceStates balance) {
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
            } else {
                //throw std::exception();
            }
        }
        return swap;
    }

    void UpdateTree(Node* node) {
        if (node == nullptr) {
            return;
        }
        UpdateTree(node->GetLeft());
        UpdateTree(node->GetRight());
        node->UpdateNode();
    }

    void BalanceLeftTree(Node* node) {
        Node* parent = node->GetParent();
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
    }

    void BalanceRightTree(Node* node) {
        Node* parent = node->GetParent();
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

    ///Function makes tree balanced
    void BalanceTree(Node* node) {
        if (node == nullptr) {
            return;
        }
        BalanceStates balance = node->GetBalance();
        if (balance == Other) {
            //throw std::exception();
        } else if (balance == LeftBiggerTwo) {
            BalanceLeftTree(node);
        } else if (balance == RightBiggerTwo) {
            BalanceRightTree(node);
        }
        node->UpdateNode();
        if (node->GetParent() != nullptr) {
            node->GetParent()->UpdateNode();
        }
        BalanceTree(node->GetParent());
    }

    /**
     * Class for nodes of AVL tree
     * Implemented rotations used in AVL tree
     */
    class Node {
      private:
        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent = nullptr;
        ValueType data_;
        size_t height = 0;
        ValueType min_;
        ValueType max_;
      public:
        ///Function updates information in node using children
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

        bool operator==(const Node& other) const {
            return &this == &other;
        }

        bool operator!=(const Node& other) const {
            return &this != &other;
        }

        Node() {}

        explicit Node(const ValueType& el) : data_(el) {}

        Node(Node* l, Node* r) : left(l), right(r) {}


        BalanceStates GetBalance() {
            size_t left_height = 0;
            if (GetLeft() != nullptr) {
                left_height = GetLeftHeight();
            }
            size_t right_height = 0;
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

        ///Returns a height of right child of node's right child
        size_t GetRightRightHeight() const {
            size_t h = 0;
            if (GetRight() != nullptr) {
                h = GetRight()->GetRightHeight();
            }
            return h;
        }

        ///Returns a height of left child of node's right child
        size_t GetRightLeftHeight() const {
            size_t h = 0;
            if (GetRight() != nullptr) {
                h = GetRight()->GetLeftHeight();
            }
            return h;
        }

        ///Returns a height of right child of node's left child
        size_t GetLeftRightHeight() const {
            size_t h = 0;
            if (GetLeft() != nullptr) {
                h = GetLeft()->GetRightHeight();
            }
            return h;
        }

        ///Returns a height of left child of node's left child
        size_t GetLeftLeftHeight() const {
            size_t h = 0;
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

        ///Returns a height of a right child of the node
        size_t GetRightHeight() const {
            size_t h = 0;
            if (GetRight() != nullptr) {
                h = GetRight()->GetHeight();
            }
            return h;
        }

        ///Returns a height of a left child of the node
        size_t GetLeftHeight() const {
            size_t h = 0;
            if (GetLeft() != nullptr) {
                h = GetLeft()->GetHeight();
            }
            return h;
        }

        size_t GetHeight() const {
            return height;
        }

        Node* GetParent() const {
            return parent;
        }

        Node* GetLeft() const {
            return left;
        }

        Node* GetRight() const {
            return right;
        }

        bool IsLeaf() const {
            return left == nullptr && right == nullptr;
        }

        ValueType GetData() const {
            return data_;
        }

        const ValueType* GetDataAdress() const {
            return &data_;
        }

        void SetParent(Node* node) {
            parent = node;
        }

        ///Function defines left child of the node
        void SetLeft(Node* node) {
            UpdateNode();
            if (node != nullptr) {
                node->SetParent(this);
            }
            left = node;
        }

        ///Function defines right child of the node
        void SetRight(Node* node) {
            UpdateNode();
            if (node != nullptr) {
                node->SetParent(this);
            }
            right = node;
        }

        Node* SmallLeftRotation() {
            if (this->IsLeaf()) {
                return this;
            }
            Node* new_node = this->GetRight();
            this->SetRight(new_node->GetLeft());
            new_node->SetLeft(this);
            UpdateNode();
            new_node->UpdateNode();
            return new_node;
        }

        Node* SmallRightRotation() {
            if (this->IsLeaf()) {
                return this;
            }
            Node* new_node = this->GetLeft();
            this->SetLeft(new_node->GetRight());
            new_node->SetRight(this);
            UpdateNode();
            new_node->UpdateNode();
            return new_node;
        }

        Node* BigLeftRotation() {
            Node* new_node = this->right->SmallRightRotation();
            this->SetRight(new_node);
            UpdateNode();
            new_node->UpdateNode();
            return SmallLeftRotation();
        }

        Node* BigRightRotation() {
            Node* new_node = this->left->SmallLeftRotation();
            this->SetLeft(new_node);
            UpdateNode();
            new_node->UpdateNode();
            return SmallRightRotation();
        }
    };
};