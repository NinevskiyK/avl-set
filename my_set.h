#pragma once

#include <initializer_list>
#include <algorithm>

template<class ValueType>

/**
 * Set based on AVL (https://ru.wikipedia.org/wiki/%D0%90%D0%92%D0%9B-%D0%B4%D0%B5%D1%80%D0%B5%D0%B2%D0%BE)
 * Implemented erase, find, lower_bound, insert, all O(log size) time
 * @tparam ValueType
 */
class Set {
  private:
    /// Balance states for tree
    enum BalanceStates {
        Balanced = 0,
        LeftBiggerOne = 1,
        LeftBiggerTwo = 2,
        RightBiggerOne = 3,
        RightBiggerTwo = 4,
        Other = 5
    };


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

        const ValueType* GetDataAddress() const {
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

    ///Function copies structure and data of set  from "from" to "to" (Before calling make sure that root copied data from other root)
    void Copy(Node* from, Node* to) {
        if (from->GetLeft() != nullptr) {
            to->SetLeft(new Node(from->GetLeft()->GetData()));
            Copy(from->GetLeft(), to->GetLeft());
        }
        if (from->GetRight() != nullptr) {
            to->SetRight(new Node(from->GetRight()->GetData()));
            Copy(from->GetRight(), to->GetRight());
        }
    }

    ///Function Inserts el to set, if element already in set returns nullptr
    Node* Insert(Node* node, const ValueType& el) {
        if (el < node->GetData()) {
            if (node->GetLeft() == nullptr) {
                node->SetLeft(new Node(el));
                return node->GetLeft();
            }
            return Insert(node->GetLeft(), el);
        } else if (node->GetData() < el) {
            if (node->GetRight() == nullptr) {
                node->SetRight(new Node(el));
                return node->GetRight();
            }
            return Insert(node->GetRight(), el);
        } else {
            return nullptr;
        }
    }

    /// Function finds the node with el(nullptr if there is no such node)
    Node* Find(Node* node, const ValueType& el) const {
        if (el < node->GetData()) {
            if (node->GetLeft() == nullptr) {
                return nullptr;
            }
            return Find(node->GetLeft(), el);
        } else if (node->GetData() < el) {
            if (node->GetRight() == nullptr) {
                return nullptr;
            }
            return Find(node->GetRight(), el);
        } else {
            return node;
        }
    }

    ///Function deletes all nodes
    void EraseAll(Node* node) {
        if (node == nullptr) {
            return;
        }
        EraseAll(node->GetLeft());
        EraseAll(node->GetRight());
        delete node;
    }

    ///Function finds first node with data >= el
    Node* LowerBound(Node* node, const ValueType& el) const {
        if (el < node->GetData()) {
            if (node->GetLeft() == nullptr || node->GetLeft()->GetMax() < el) {
                return node;
            }
            return LowerBound(node->GetLeft(), el);
        } else if (node->GetData() < el) {
            if (node->GetRight() == nullptr) {
                return nullptr;
            }
            return LowerBound(node->GetRight(), el);
        } else {
            return node;
        }
    }

  public:
    /**
     * Iterator for Set
     */
    class iterator {
      public:
        iterator() {}

        explicit iterator(Node* node) : node(node), end_(false) {}

        explicit iterator(Node* node, bool end_) : node(node), end_(end_) {}


        bool operator==(const iterator& other) const {
            return node == other.node && end_ == other.end_;
        }

        bool operator!=(const iterator& other) const {
            return node != other.node || end_ != other.end_;
        }

        ValueType operator*() const {
            return node->GetData();
        };

        const ValueType* operator->() const {
            return node->GetDataAddress();
        }

        iterator operator++() {
            if (node->GetRight() == nullptr) {
                const Node* prv = nullptr;
                while (node != nullptr && node->GetRight() == prv) {
                    prv = node;
                    node = node->GetParent();
                }
                if (node == nullptr) {
                    node = prv;
                    end_ = true;
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
                const Node* prv = nullptr;
                while (node != nullptr && node->GetRight() == prv) {
                    prv = node;
                    node = node->GetParent();
                }
                if (node == nullptr) {
                    node = prv;
                    end_ = true;
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
            if (end_ == true) {
                end_ = false;
                while (node->GetRight() != nullptr) {
                    node = node->GetRight();
                }
            } else if (node->GetLeft() == nullptr) {
                const Node* prv = nullptr;
                while (node != nullptr && node->GetLeft() == prv) {
                    prv = node;
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

        iterator operator--(int) {
            if (end_ == true) {
                end_ = false;
                while (node->GetRight() != nullptr) {
                    node = node->GetRight();
                }
            } else if (node->GetLeft() == nullptr) {
                const Node* prv = nullptr;
                while (node != nullptr && node->GetLeft() == prv) {
                    prv = node;
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
        bool end_ = false;
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
        size_ = other.size_;
        if (other.root == nullptr) {
            root = nullptr;
            return;
        }
        root = new Node(other.root->GetData());
        Copy(other.root, root);
        UpdateTree(root);
    }

    Set& operator=(const Set& set) {
        if (this == &set) {
            return *this;
        }
        EraseAll(root);
        size_ = set.size_;
        if (set.root == nullptr) {
            root = nullptr;
            return *this;
        }
        root = new Node(set.root->GetData());
        Copy(set.root, root);
        UpdateTree(root);
        return *this;
    }

    ~Set() {
        EraseAll(root);
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
        Node* inserted = Insert(root, el);
        if (inserted == nullptr) {
            --size_;
        } else {
            BalanceTree(inserted);
        }
    }

    /// Function finds the element in set and returns iterator(nullptr if no element), O(log size) time
    iterator find(const ValueType& el) const {
        if (root == nullptr) {
            return iterator(root, true);
        }
        Node* found = Find(root, el);
        if (found == nullptr) {
            return iterator(root, true);
        }
        return iterator(found);
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
            return iterator(root, true);
        }
        Node* now = LowerBound(root, el);
        if (now == nullptr) {
            return iterator(root, true);
        }
        return iterator(now);
    }

    /// Function returns iterator on the first(the smallest) element, O(log size) time
    iterator begin() const {
        Node* node = root;
        if (node == nullptr) {
            return iterator(root, true);
        }
        while (node->GetLeft() != nullptr) {
            node = node->GetLeft();
            node->UpdateNode();
        }
        return iterator(node);
    }

    iterator end() const {
        return iterator(root, true);
    }

  private:
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

  private:
    size_t size_ = 0;
    Node* root = nullptr;
};