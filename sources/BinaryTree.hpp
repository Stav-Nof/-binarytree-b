#include <iostream>
#include <string>
#include <stack>

namespace ariel {

    enum Order {
        Pre,
        In,
        Post
    };

    enum State {
        Left,
        Right,
        Curr,
        Up
    };

    template<typename T>
    class BinaryTree {
    private:
        struct Node {
            T _value;
            Node *_left, *_right;

            bool hasRight() const {
                return _right != nullptr;
            }

            bool hasLeft() const {
                return _left != nullptr;
            }

            Node(const T &v, Node *p = nullptr)
                    : _value(v), _left(nullptr), _right(nullptr) {
            }

            ~Node() {
                delete _left;
                delete _right;
            }

            Node(const Node &other) : _value(other._value), _right(nullptr), _left(nullptr) {
                if (other.hasLeft()) {
                    _left = new Node(*other._left);
                }
                if (other.hasRight()) {
                    _right = new Node(*other._right);
                }
            };

            Node(Node &&other) noexcept: _value(nullptr), _right(nullptr), _left(nullptr) {
                _value = other._value;
                _right = other._right;
                _left = other._left;
                other._value = nullptr;
                other._right = nullptr;
                other._left = nullptr;
            }

            Node &operator=(Node &&other) noexcept {
                if (this != &other) {
                    delete _right;
                    delete _left;
                    _value = other._value;
                    _right = other._right;
                    _left = other._left;
                    other._value = nullptr;
                    other._right = nullptr;
                    other._left = nullptr;
                }
                return *this;
            }

            Node &operator=(const Node &other) {
                if (this != &other) {
                    delete _left;
                    delete _right;
                    _value = other._value;
                    _right = other.hasRight() ? new Node(*other._right) : nullptr;
                    _left = other.hasLeft() ? new Node(*other._left) : nullptr;
                }
                return *this;
            }
        };

        Node *_root;

        Node *findNode(T value) {
            Node *currNode = nullptr;
            for (iterator it = begin_preorder(); it != end_preorder(); ++it) {
                if (*it == value) {
                    currNode = it.getCurrNode();
                    break;
                }
            }
            return currNode;
        }

        void haveRoot() {
            if (_root == nullptr) {
                throw ("can't add nodes while the root is null");
            }
        }

    public:
        BinaryTree() : _root(nullptr) {}

        BinaryTree(const BinaryTree<T> &other) {
            _root = new Node(*other._root);
        }

        ~BinaryTree() {
            delete _root;
        } 

        BinaryTree<T>(BinaryTree<T> &&other) noexcept: _root(std::move(other._root)) {
        }

        BinaryTree<T> &operator=(BinaryTree<T> &&other) noexcept {
            if (this != &other) {
                delete _root;
                _root = other._root;
                other._root = nullptr;
            }
            return *this;
        }

        BinaryTree<T> &operator=(const BinaryTree<T> &other) {
            if (this != &other) {
                if (_root != nullptr) {
                    delete _root;
                    _root = nullptr;
                }
                _root = new Node(*other._root);
            }
            return *this;
        }

        BinaryTree<T> &add_root(T value) {
            if (_root == nullptr) {
                _root = new Node(value);
            } else {
                _root->_value = value;
            }
            return *this;
        }

        BinaryTree<T> &add_left(T value, T newNode) {
            haveRoot();
            Node *node = findNode(value);
            if (node == nullptr) {
                throw ("node not found, can't add new node");
            }
            if (node->hasLeft()) {
                node->_left->_value = newNode;
            } else {
                node->_left = new Node(newNode, node);
            }
            return *this;
        }

        BinaryTree<T> &add_right(T value, T newNode) {
            haveRoot();
            Node *node = findNode(value);
            if (node == nullptr) {
                throw ("node not found, can't add new node");
            }
            if (node->hasRight()) {
                node->_right->_value = newNode;
            } else {
                node->_right = new Node(newNode, node);
            }
            return *this;
        }

        class iterator {
        private:
            Node *_currNode, *temp;
            Order _order;
            std::stack<Node *> stack;
            State _state;

            void incrementPreorder() {
                if (stack.empty()) {
                    _currNode = nullptr;
                } else {
                    if (_currNode->hasRight()) {
                        stack.push(_currNode->_right);
                    }
                    if (_currNode->hasLeft()) {
                        stack.push(_currNode->_left);
                    }
                    _currNode = stack.top();
                    stack.pop();
                }
            }

            void incrementInorder() {
                while (temp != nullptr) {
                    stack.push(temp);
                    temp = temp->_left;
                }
                if (stack.empty()) {
                    _currNode = nullptr;
                    return;
                }
                _currNode = stack.top();
                stack.pop();
                temp = _currNode->_right;
            }

            void incrementPostorder() {
                if (stack.empty()) {
                    _currNode = nullptr;
                    _state = Curr;
                } else if (_state == Curr) {
                    _state = Up;
                }
                while (_state != Curr) {
                    switch (_state) {
                        case Left:
                            if (_currNode->hasLeft()) {
                                stack.push(_currNode);
                                _currNode = _currNode->_left;
                            } else {
                                _state = Right;
                            }
                            break;
                        case Right:
                            if (_currNode->hasRight()) {
                                stack.push(_currNode);
                                _currNode = _currNode->_right;
                                _state = Left;
                            } else {
                                _state = Curr;
                            }
                            break;
                        case Curr:
                            _state = Up;
                            break;
                        case Up:
                            temp = _currNode;
                            _currNode = stack.top();
                            stack.pop();
                            _state = temp == _currNode->_left ? Right : Curr;
                            break;
                        default:
                            break;
                    }
                }
            }

        public:

            iterator(Node *root = nullptr, Order order = In) : _order(order) {
                if (root == nullptr || order == Pre) {
                    _currNode = root;
                    stack.push(nullptr);
                } else if (order == In) {
                    while (root->hasLeft()) {
                        stack.push(root);
                        root = root->_left;
                    }
                    _currNode = root;
                    temp = _currNode->_right;
                } else {
                    temp = root;
                    while (root->hasLeft() || root->hasRight()) {
                        while (root->hasLeft()) {
                            stack.push(root);
                            root = root->_left;
                        }
                        while (root->hasRight() && !root->hasLeft()) {
                            stack.push(root);
                            root = root->_right;
                        }
                    }
                    _state = Curr;
                    _currNode = root;
                }
            }

            Node *getCurrNode() {
                return _currNode;
            }

            T &operator*() const {
                return _currNode->_value;
            }

            T *operator->() const {
                return &(_currNode->_value);
            }

            // ++i;
            iterator &operator++() {
                if (_order == Post) {
                    incrementPostorder();
                } else if (_order == Pre) {
                    incrementPreorder();
                } else {
                    incrementInorder();
                }
                return *this;
            }

            // i++;
            // Usually iterators are passed by value and not by const& as they are small.
            iterator operator++(int) {
                iterator tmp = *this;
                if (_order == Post) {
                    incrementPostorder();
                } else if (_order == Pre) {
                    incrementPreorder();
                } else {
                    incrementInorder();
                }
                return tmp;
            }

            bool operator==(const iterator &otherPtr) const {
                return _currNode == otherPtr._currNode;
            }

            bool operator!=(const iterator &otherPtr) const {
                return _currNode != otherPtr._currNode;
            }
        };

        class cIterator {
        private:
            Node *_currNode, *temp;
            Order _order;
            std::stack<Node *> stack;
            State _state;

            void incrementPreorder() {
                if (stack.empty()) {
                    _currNode = nullptr;
                } else {
                    if (_currNode->hasRight()) {
                        stack.push(_currNode->_right);
                    }
                    if (_currNode->hasLeft()) {
                        stack.push(_currNode->_left);
                    }
                    _currNode = stack.top();
                    stack.pop();
                }
            }

            void incrementInorder() {
                if (temp != nullptr) {
                    stack.push(temp);
                    temp = temp->_left;
                }
                if (stack.empty()) {
                    _currNode = nullptr;
                    return;
                }
                while (temp != nullptr) {
                    stack.push(temp);
                    temp = temp->_left;
                }
                _currNode = stack.top();
                stack.pop();
                temp = _currNode->_right;
            }

            void incrementPostorder() {
                if (stack.empty()) {
                    _currNode = nullptr;
                    _state = Curr;
                } else if (_state == Curr) {
                    _state = Up;
                }
                while (_state != Curr) {
                    switch (_state) {
                        case Left:
                            if (_currNode->hasLeft()) {
                                stack.push(_currNode);
                                _currNode = _currNode->_left;
                            } else {
                                _state = Right;
                            }
                            break;
                        case Right:
                            if (_currNode->hasRight()) {
                                stack.push(_currNode);
                                _currNode = _currNode->_right;
                                _state = Left;
                            } else {
                                _state = Curr;
                            }
                            break;
                        case Curr:
                            _state = Up;
                            break;
                        case Up:
                            temp = _currNode;
                            _currNode = stack.top();
                            stack.pop();
                            _state = temp == _currNode->_left ? Right : Curr;
                            break;
                        default:
                            break;
                    }
                }
            }

        public:

            cIterator(Node *root = nullptr, Order order = In) : _order(order) {
                if (root == nullptr || order == Pre) {
                    _currNode = root;
                    stack.push(nullptr);
                } else if (order == In) {
                    while (root->hasLeft()) {
                        stack.push(root);
                        root = root->_left;
                    }
                    _currNode = root;
                    temp = _currNode->_right;
                } else {
                    temp = root;
                    while (root->hasLeft() || root->hasRight()) {
                        while (root->hasLeft()) {
                            stack.push(root);
                            root = root->_left;
                        }
                        while (root->hasRight() && !root->hasLeft()) {
                            stack.push(root);
                            root = root->_right;
                        }
                    }
                    _state = Curr;
                    _currNode = root;
                }
            }

            Node *getCurrNode() {
                return _currNode;
            }

            const T &operator*() const {
                return _currNode->_value;
            }

            const T *operator->() const {
                return &(_currNode->_value);
            }

            // ++i;
            cIterator &operator++() {
                if (_order == Post) {
                    incrementPostorder();
                } else if (_order == Pre) {
                    incrementPreorder();
                } else {
                    incrementInorder();
                }
                return *this;
            }

            // i++;
            // Usually iterators are passed by value and not by const& as they are small.
            cIterator operator++(int) {
                cIterator tmp = *this;
                if (_order == Post) {
                    incrementPostorder();
                } else if (_order == Pre) {
                    incrementPreorder();
                } else {
                    incrementInorder();
                }
                return tmp;
            }

            bool operator==(const cIterator &otherPtr) const {
                return _currNode == otherPtr._currNode;
            }

            bool operator!=(const cIterator &otherPtr) const {
                return _currNode != otherPtr._currNode;
            }
        };

        iterator begin_preorder() const {
            return iterator{_root, Pre};
        }

        iterator end_preorder() const {
            return iterator{};
        }

        cIterator cbegin_preorder() const {
            return cIterator{_root, Pre};
        }

        cIterator cend_preorder() const {
            return cIterator{};
        }

        iterator begin_inorder() const {
            return iterator{_root, In};
        }

        iterator end_inorder() const {
            return iterator{};
        }

        iterator begin() const {
            return iterator{_root};
        }

        iterator end() const {
            return iterator{};
        }

        iterator begin_postorder() const {
            return iterator{_root, Post};
        }

        iterator end_postorder() const {
            return iterator{};
        }

        friend std::ostream &operator<<(std::ostream &os, const BinaryTree &b) {
            if (b._root == nullptr) { os << "empty tree" << std::endl; }
            else {
                os << b._root->_value << std::endl;
                printSubTree(os, b._root, "");
                os << std::endl;
            }
            return os;
        }

        friend void printSubTree(std::ostream &os, Node *root, const std::string &prefix) {
            if (root == nullptr) {
                return;
            }
            if (!root->hasLeft() && !root->hasRight()) {
                return;
            }
            os << prefix;
            os << ((root->hasLeft() && root->hasRight()) ? "├── " : "");
            os << ((!root->hasLeft() && root->hasRight()) ? "└── " : "");
            if (root->hasRight()) {
                bool printStrand = (root->hasLeft() && (root->_right->hasRight() || root->_right->hasLeft()));
                std::string newPrefix = prefix + (printStrand ? "│   " : "    ");
                os << root->_right->_value << std::endl;
                printSubTree(os, root->_right, newPrefix);
            }

            if (root->hasLeft()) {
                os << (root->hasRight() ? prefix : "") << "└── " << root->_left->_value << std::endl;
                printSubTree(os, root->_left, prefix + "    ");
            }
        }
    };
}  // namespace ariel
