#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>

template<typename T, size_t NodeMaxSize = 10, typename Allocator = std::allocator<T>>
class unrolled_list {
private:
    using AllocTraits = std::allocator_traits<Allocator>;

public:
    static_assert(NodeMaxSize != 0, "NodeMaxSize must be greater than 0");
    using size_type = typename AllocTraits::size_type;

private:
    struct BaseNode {
        BaseNode* prev;
        BaseNode* next;

        BaseNode() = default;
        BaseNode(BaseNode*, BaseNode*);
        virtual ~BaseNode();
    };
    struct Node : public BaseNode {
        alignas(T) std::byte storage[NodeMaxSize * sizeof(T)];
        size_type count;

        T* data();    
        const T* data() const;    

        Node(BaseNode*, BaseNode*);
        Node(BaseNode*, BaseNode*, T*, size_type);
        Node(BaseNode*, BaseNode*, const T&, size_type = NodeMaxSize);
        ~Node() override;
    };

public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using pointer = typename AllocTraits::pointer;
    using const_pointer = typename AllocTraits::const_pointer;
    using difference_type = typename AllocTraits::difference_type;
    using allocator_type = Allocator;

private:
    using node_allocator_t = typename AllocTraits::template rebind_alloc<Node>;
    using NodeAllocTraits = std::allocator_traits<node_allocator_t>;

    template<bool IsConst>
    class Iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = AllocTraits::difference_type;
        using pointer = std::conditional_t<IsConst, const_pointer, pointer>;
        using reference = std::conditional_t<IsConst, const_reference, reference>;
        using base_ptr = std::conditional_t<IsConst, const BaseNode*, BaseNode*>;
        using node_ptr = std::conditional_t<IsConst, const Node*, Node*>;

        Iterator(const Iterator<false>&);
        Iterator(base_ptr, size_type);
 
        bool operator==(const Iterator<IsConst>&) const;
        bool operator!=(const Iterator<IsConst>&) const;
        reference operator*() const;
        pointer operator->() const;
        Iterator& operator++();
        Iterator operator++(int);
        Iterator& operator--();
        Iterator operator--(int);
        friend unrolled_list;
        
    private:
        base_ptr ptr_;
        size_type index_;
    };

public:
    using const_iterator = Iterator<true>;
    using iterator = Iterator<false>;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    unrolled_list();
    unrolled_list(const unrolled_list&);
    unrolled_list(const unrolled_list&, allocator_type);
    unrolled_list& operator=(const unrolled_list&);
    ~unrolled_list();

    unrolled_list(size_type, const T&);
    template<std::input_iterator InputIt>
    unrolled_list(InputIt, InputIt, const allocator_type&);
    template<std::input_iterator InputIt>
    unrolled_list(InputIt, InputIt);

    unrolled_list(std::initializer_list<T>);
    unrolled_list& operator=(std::initializer_list<T>);

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    reverse_iterator rbegin();
    reverse_iterator rend();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;
    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;

    bool operator==(const unrolled_list<T, NodeMaxSize, Allocator>&) const;
    bool operator!=(const unrolled_list<T, NodeMaxSize, Allocator>&) const;

    void swap(unrolled_list<T, NodeMaxSize, Allocator>&);

    size_type size() const;
    size_type max_size() const;
    bool empty() const;

    template<std::input_iterator InputIt>
    iterator insert(const_iterator, InputIt, InputIt);
    iterator insert(iterator, const T&);
    iterator insert(const_iterator, const T&);
    iterator insert(const_iterator, size_type, const T&);
    iterator insert(const_iterator, std::initializer_list<T>);

    iterator erase(const_iterator) noexcept;
    iterator erase(const_iterator, const_iterator) noexcept;

    void clear() noexcept;

    template<std::input_iterator InputIt>
    void assign(InputIt, InputIt);
    void assign(std::initializer_list<T>);
    void assign(size_type, const T&);

    T& front();
    const T& front() const;
    T& back();
    const T& back() const;

    void pop_back() noexcept;
    void push_back(const T&);
    
    void pop_front() noexcept;
    void push_front(const T&);

    unrolled_list(allocator_type);
    unrolled_list(size_type, allocator_type);
    
    allocator_type get_allocator() noexcept;

    static_assert(
        std::is_same<value_type, typename allocator_type::value_type>::value,
        "Allocator::value_type must be same type as value_type");

private:
    BaseNode fake_node_;
    size_type size_;
    [[no_unique_address]] node_allocator_t allocator_;
};

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::BaseNode::BaseNode(
        BaseNode* left, BaseNode* right)
        : prev(left), next(right)  {}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::Node::Node(
        BaseNode* left, BaseNode* right, T* new_data, size_type new_count)
        : BaseNode(left, right) {
    size_t i = 0;
    Allocator alloc{};
    try {
        for (; i < new_count; ++i) {
            AllocTraits::construct(alloc, data() + i, new_data[i]);
        }
    } catch (...) {
        for (size_t j = 0; j < i; ++j) {
            AllocTraits::destroy(alloc, data() + j);
        }
        throw;
    }
    count = new_count;    
}


template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::Node::Node(
        BaseNode* left, BaseNode* right, const T& value, size_type new_count) {
    size_t i = 0;
    Allocator alloc{};
    try {
        for (; i < new_count; ++i) {
            AllocTraits::construct(alloc, data() + i, value);
        }
    } catch (...) {
        for (size_t j = 0; j < i; ++j) {
            AllocTraits::destroy(alloc, data() + j);
        }
        throw;
    }
    count = new_count;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::BaseNode::~BaseNode() {}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::Node::~Node() {
    allocator_type alloc = Allocator{};
    for (size_t i = 0; i < count; ++i) {
        AllocTraits::destroy(alloc, data() + i);
    }
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::Node::Node(BaseNode* left, 
                                                     BaseNode* right)
        : count(0), BaseNode(left, right) {}

template<typename T, size_t NodeMaxSize, typename Allocator>
T* unrolled_list<T, NodeMaxSize, Allocator>::Node::data() {
    return reinterpret_cast<T*>(storage);
}

template<typename T, size_t NodeMaxSize, typename Allocator>
const T* unrolled_list<T, NodeMaxSize, Allocator>::Node::data() const {
    return reinterpret_cast<const T*>(storage);
}

template<typename T, size_t NodeMaxSize, typename Allocator>
template<bool IsConst>
unrolled_list<T, NodeMaxSize, Allocator>::Iterator<IsConst>::Iterator(
        base_ptr ptr, size_type index) : ptr_(ptr), index_(index) {}

template<typename T, size_t NodeMaxSize, typename Allocator>
template<bool IsConst>
unrolled_list<T, NodeMaxSize, Allocator>::Iterator<IsConst>::Iterator(
    const Iterator<false>& other)
    : ptr_(other.ptr_), index_(other.index_) {}

template<typename T, size_t NodeMaxSize, typename Allocator>
template<bool IsConst>
bool unrolled_list<T, NodeMaxSize, Allocator>::Iterator<IsConst>::operator==(
        const Iterator<IsConst>& other) const {
    return ptr_ == other.ptr_ && index_ == other.index_;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
template<bool IsConst>
bool unrolled_list<T, NodeMaxSize, Allocator>::Iterator<IsConst>::operator!=(
        const Iterator<IsConst>& other) const {
    return !(*this == other);
}

template<typename T, size_t NodeMaxSize, typename Allocator>
template<bool IsConst>
unrolled_list<T, NodeMaxSize, Allocator>::Iterator<IsConst>::reference 
    unrolled_list<T, NodeMaxSize, Allocator>::Iterator<IsConst>::operator*() const {
    return static_cast<node_ptr>(ptr_)->data()[index_];
}

template<typename T, size_t NodeMaxSize, typename Allocator>
template<bool IsConst>
unrolled_list<T, NodeMaxSize, Allocator>::Iterator<IsConst>::pointer 
    unrolled_list<T, NodeMaxSize, Allocator>::Iterator<IsConst>::operator->() const {
    return &(static_cast<node_ptr>(ptr_)->data()[index_]);
}

template<typename T, size_t NodeMaxSize, typename Allocator>
template<bool IsConst>
unrolled_list<T, NodeMaxSize, Allocator>::Iterator<IsConst>& 
    unrolled_list<T, NodeMaxSize, Allocator>::Iterator<IsConst>::operator++() {
    if (index_ + 1 == static_cast<node_ptr>(ptr_)->count) {
        ptr_ = static_cast<node_ptr>(ptr_)->next;
        index_ = 0;
        return *this;
    } 
    ++index_;
    return *this;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
template<bool IsConst>
unrolled_list<T, NodeMaxSize, Allocator>::Iterator<IsConst> 
    unrolled_list<T, NodeMaxSize, Allocator>::Iterator<IsConst>::operator++(int) {
    Iterator<IsConst> temp = *this;
    ++*this;
    return temp;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
template<bool IsConst>
unrolled_list<T, NodeMaxSize, Allocator>::Iterator<IsConst>& 
    unrolled_list<T, NodeMaxSize, Allocator>::Iterator<IsConst>::operator--() {
    if (index_ == 0) {
        ptr_ = static_cast<node_ptr>(ptr_)->prev;
        index_ = static_cast<node_ptr>(ptr_)->count - 1;
        return *this;
    }
    --index_;
    return *this;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
template<bool IsConst>
unrolled_list<T, NodeMaxSize, Allocator>::Iterator<IsConst>
    unrolled_list<T, NodeMaxSize, Allocator>::Iterator<IsConst>::operator--(int) {
    Iterator<IsConst> temp = *this;
    --*this;
    return temp;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::iterator 
    unrolled_list<T, NodeMaxSize, Allocator>::begin() {
    return iterator{fake_node_.next, 0};
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::iterator 
    unrolled_list<T, NodeMaxSize, Allocator>::end() {
    return iterator{&fake_node_, 0};
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::const_iterator 
    unrolled_list<T, NodeMaxSize, Allocator>::begin() const {
    return const_iterator{fake_node_.next, 0};
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::const_iterator 
    unrolled_list<T, NodeMaxSize, Allocator>::end() const {
    return const_iterator{&fake_node_, 0};
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::const_iterator 
    unrolled_list<T, NodeMaxSize, Allocator>::cbegin() const {
    return const_iterator{fake_node_.next, 0};
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::const_iterator 
    unrolled_list<T, NodeMaxSize, Allocator>::cend() const {
    return const_iterator{&fake_node_, 0};
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::unrolled_list() 
        : fake_node_(&fake_node_, &fake_node_), size_(0) {}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::unrolled_list(
        const unrolled_list& other) {
    node_allocator_t new_allocator
        = NodeAllocTraits::select_on_container_copy_construction(other.allocator_);
    try {
        bool is_first = true;
        Node* prev_node = nullptr;
        for (BaseNode* cur = other.fake_node_.next; cur != &other.fake_node_; cur = cur->next) {
            Node* node = static_cast<Node*>(cur);
            if (is_first) {
                is_first = false;
                prev_node = NodeAllocTraits::allocate(new_allocator, 1);
                NodeAllocTraits::construct(
                    new_allocator, prev_node,
                    &fake_node_, nullptr, node->data(), node->count);
                fake_node_.next = prev_node;
                continue;
            }
            Node* new_node = NodeAllocTraits::allocate(new_allocator, 1);
            NodeAllocTraits::construct(
                new_allocator, new_node, 
                prev_node, nullptr, node->data(), node->count);
            prev_node->next = new_node;
            prev_node = new_node;
        }
        if (prev_node) {
            prev_node->next = &fake_node_;
            fake_node_.prev = prev_node;
        } else {
            fake_node_.next = &fake_node_;
            fake_node_.prev = &fake_node_;
        }
        size_ = other.size_;
        allocator_ = new_allocator;
    } catch (...) {
        BaseNode* next_node = nullptr;
        for (BaseNode* cur = fake_node_.next; cur != &fake_node_; cur = next_node) {
            Node* node = static_cast<Node*>(cur);
            next_node = node->next;
            for (size_t i = 0; i < node->count; ++i) {
                NodeAllocTraits::destroy(new_allocator, node->data() + i);
            }
            NodeAllocTraits::deallocate(new_allocator, node, 1);
        }
        fake_node_.prev = &fake_node_;
        fake_node_.next = &fake_node_;
        size_ = 0;
        throw;
    }
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::unrolled_list(
        const unrolled_list& other, Allocator new_allocator) {
    try {
        bool is_first = true;
        Node* prev_node = nullptr;
        for (BaseNode* cur = other.fake_node_.next; cur != &other.fake_node_; cur = cur->next) {
            Node* node = static_cast<Node*>(cur);
            if (is_first) {
                is_first = false;
                prev_node = NodeAllocTraits::allocate(new_allocator, 1);
                NodeAllocTraits::construct(
                    new_allocator, prev_node,
                    &fake_node_, nullptr, node->data(), node->count);
                fake_node_.next = prev_node;
                continue;
            }
            Node* new_node = NodeAllocTraits::allocate(new_allocator, 1);
            NodeAllocTraits::construct(
                new_allocator, new_node, 
                prev_node, nullptr, node->data(), node->count);
            prev_node->next = new_node;
            prev_node = new_node;
        }
        if (prev_node) {
            prev_node->next = &fake_node_;
            fake_node_.prev = prev_node;
        } else {
            fake_node_.next = &fake_node_;
            fake_node_.prev = &fake_node_;
        }
        size_ = other.size_;
        allocator_ = new_allocator;
    } catch (...) {
        BaseNode* next_node = nullptr;
        for (BaseNode* cur = fake_node_.next; cur != &fake_node_; cur = next_node) {
            Node* node = static_cast<Node*>(cur);
            next_node = node->next;
            for (size_t i = 0; i < node->count; ++i) {
                NodeAllocTraits::destroy(new_allocator, node->data() + i);
            }
            NodeAllocTraits::deallocate(new_allocator, node, 1);
        }
        fake_node_.prev = &fake_node_;
        fake_node_.next = &fake_node_;
        size_ = 0;
        throw;
    }
}


template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::~unrolled_list() {
    Node* next_node = nullptr;
    for (Node* node = static_cast<Node*>(fake_node_.next); node != &fake_node_; node = next_node) {
        next_node = static_cast<Node*>(node->next);
        NodeAllocTraits::destroy(allocator_, node);
        NodeAllocTraits::deallocate(allocator_, node, 1);
    }
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::unrolled_list(
        size_type size, const T& value) {
    if (size == 0) {
        fake_node_.prev = &fake_node_;
        fake_node_.next = &fake_node_;
        size_ = 0;
        return;
    }
    Node* prev_node = nullptr;
    for (size_t i = 0; i < size / NodeMaxSize; ++i) {
        Node* new_node = NodeAllocTraits::allocate(allocator_, 1);
        if (i == 0) {
            fake_node_.next = new_node;
            NodeAllocTraits::construct(allocator_, new_node, 
                                       &fake_node_, nullptr, value);
            prev_node = new_node;
            ++size_;
            continue;
        }
        prev_node->next = new_node;
        NodeAllocTraits::construct(allocator_, new_node, 
                                   prev_node, nullptr, value);

        prev_node = new_node;
        ++size_;
    }
    if (size % NodeMaxSize > 0) {
        Node* new_node = NodeAllocTraits::allocate(allocator_, 1);
        NodeAllocTraits::construct(allocator_, new_node, 
                                   prev_node, &fake_node_, value, size % NodeMaxSize);
        if (prev_node) {
            prev_node->next = new_node;
        } else {
            fake_node_.prev = new_node;
            fake_node_.next = new_node;            
        }
        prev_node = new_node;
        ++size_;
    }
    if (prev_node) {
        prev_node->next = &fake_node_;
        fake_node_.prev = prev_node;
    }

    size_ = size;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
template<std::input_iterator InputIt>
unrolled_list<T, NodeMaxSize, Allocator>::unrolled_list(
        InputIt first, InputIt last, 
        const allocator_type& allocator)
        : size_(0), allocator_(allocator) {
    try {
        for (InputIt it = first; it != last; ++it) {
            push_back(*it);
        }
    } catch (...) {
        clear();
        throw;
    }
}

template<typename T, size_t NodeMaxSize, typename Allocator>
template<std::input_iterator InputIt>
unrolled_list<T, NodeMaxSize, Allocator>::unrolled_list(
        InputIt first, InputIt last) : size_(0) {
    for (InputIt it = first; it != last; ++it) {
        push_back(*it);
    }
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::unrolled_list(
        std::initializer_list<T> init_list) : size_(0) {
    for (const T& value : init_list) {
        push_back(value);
    }
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::unrolled_list(
        Allocator allocator) : allocator_(allocator), size_(0) {}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::unrolled_list(
        size_type size, 
        allocator_type allocator) : allocator_(allocator), size_(size) {
    if (size == 0) {
        fake_node_.prev = &fake_node_;
        fake_node_.next = &fake_node_;
        size_ = 0;
        return;
    }
    Node* prev_node = nullptr;
    for (size_t i = 0; i < size / NodeMaxSize; ++i) {
        Node* new_node = NodeAllocTraits::allocate(allocator_, 1);
        if (i == 0) {
            fake_node_.next = new_node;
            NodeAllocTraits::construct(allocator_, new_node, &fake_node_, nullptr); 
            new_node->count = NodeMaxSize;
            for (size_t j = 0; j < NodeMaxSize; ++j) {
                AllocTraits::construct(allocator, new_node->data() + j);
            }
            prev_node = new_node;
            continue;
        }
        prev_node->next = new_node;
        NodeAllocTraits::construct(allocator_, new_node, prev_node, nullptr);
        new_node->count = NodeMaxSize;
        for (size_t j = 0; j < NodeMaxSize; ++j) {
            AllocTraits::construct(allocator, new_node->data() + j);
        }
        prev_node = new_node;
    }

    if (size % NodeMaxSize > 0) {
        Node* new_node = NodeAllocTraits::allocate(allocator_, 1);
        NodeAllocTraits::construct(allocator_, new_node, prev_node, nullptr);
        new_node->count = size % NodeMaxSize;
        if (!prev_node) {
            new_node->prev = &fake_node_;
            new_node->next = &fake_node_;
            fake_node_.next = new_node;
            fake_node_.prev = new_node;
        } else {
            prev_node->next = new_node;
            new_node->next = &fake_node_;
        }
        for (size_t j = 0; j < size % NodeMaxSize; ++j) {
            AllocTraits::construct(allocator, new_node->data() + j);
        }
    }
}

template<typename T, size_t NodeMaxSize, typename Allocator>
void unrolled_list<T, NodeMaxSize, Allocator>::push_back(const T& value) {
    Allocator alloc(allocator_);
    if (empty() || static_cast<Node*>(fake_node_.prev)->count == NodeMaxSize) {
        Node* new_node = NodeAllocTraits::allocate(allocator_, 1);
        if (empty()) {
            NodeAllocTraits::construct(allocator_, new_node, 
                                       &fake_node_, &fake_node_);
            try {
                AllocTraits::construct(alloc, new_node->data(), value);
            } catch (...) {
                NodeAllocTraits::deallocate(allocator_, new_node, 1);
                throw;
            }
            fake_node_.prev = new_node;
            fake_node_.next = new_node;
        } else {
            Node* last_prev = static_cast<Node*>(fake_node_.prev);
            NodeAllocTraits::construct(allocator_, new_node,
                                       last_prev, &fake_node_);
            try {
                AllocTraits::construct(alloc, new_node->data(), value);
            } catch (...) {
                NodeAllocTraits::deallocate(allocator_, new_node, 1);
                throw;
            }
            last_prev->next = new_node;
            fake_node_.prev = new_node;
        }
        ++size_;
        ++new_node->count;
        return;
    }
    Node* back_node = static_cast<Node*>(fake_node_.prev);
    AllocTraits::construct(alloc, back_node->data() + back_node->count, value);
    ++back_node->count;
    ++size_;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
bool unrolled_list<T, NodeMaxSize, Allocator>::empty() const {
    return size_ == 0;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::size_type 
    unrolled_list<T, NodeMaxSize, Allocator>::size() const {
    return size_;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::size_type 
    unrolled_list<T, NodeMaxSize, Allocator>::max_size() const {
    return std::numeric_limits<size_type>::max();
}

template<typename T, size_t NodeMaxSize, typename Allocator>
void unrolled_list<T, NodeMaxSize, Allocator>::push_front(const T& value) {
    Allocator alloc(allocator_);
    if (empty() || static_cast<Node*>(fake_node_.next)->count == NodeMaxSize) {
        Node* new_node = NodeAllocTraits::allocate(allocator_, 1);
        if (empty()) {
            NodeAllocTraits::construct(allocator_, new_node, 
                                       &fake_node_, &fake_node_);
            try {
                AllocTraits::construct(alloc, new_node->data(), value);
            } catch (...) {
                NodeAllocTraits::deallocate(allocator_, new_node, 1);
                throw;
            }
            fake_node_.prev = new_node;
            fake_node_.next = new_node;
        } else {
            Node* last_next = static_cast<Node*>(fake_node_.next);
            NodeAllocTraits::construct(allocator_, new_node,
                                       &fake_node_, last_next);
            try {
            AllocTraits::construct(alloc, new_node->data(), value);
            } catch (...) {
                NodeAllocTraits::deallocate(allocator_, new_node, 1);
                throw;
            }
            last_next->prev = new_node;
            fake_node_.next = new_node;
        }
        ++size_;
        ++new_node->count;
        return;
    }
    Node* front_node = static_cast<Node*>(fake_node_.next);
    for (int32_t i = front_node->count; i > 0; --i) {
        front_node->data()[i] = std::move(front_node->data()[i - 1]);
    }
    AllocTraits::construct(alloc, front_node->data(), value);
    ++front_node->count; 
    ++size_;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
void unrolled_list<T, NodeMaxSize, Allocator>::pop_back() noexcept {
    if (empty())
        return;

    Allocator alloc(allocator_);
    Node* back_node = static_cast<Node*>(fake_node_.prev);
    if (back_node->count > 1) {
        --back_node->count;
        AllocTraits::destroy(alloc, back_node->data() + back_node->count);
    } else {
        if (size_ == 1) {
            fake_node_.next = &fake_node_;
            fake_node_.prev = &fake_node_;
        } else {
            back_node->prev->next = &fake_node_;
            fake_node_.prev = back_node->prev;
        }
        NodeAllocTraits::destroy(allocator_, back_node);
        NodeAllocTraits::deallocate(allocator_, back_node, 1);
    }
    --size_;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
void unrolled_list<T, NodeMaxSize, Allocator>::pop_front() noexcept {
    if (empty())
        return;

    Allocator alloc(allocator_);
    Node* front_node = static_cast<Node*>(fake_node_.next);
    if (front_node->count > 1) {
        AllocTraits::destroy(alloc, front_node->data());
        --front_node->count;
        for (size_t j = 0; j < front_node->count; ++j) {
            front_node->data()[j] = front_node->data()[j + 1]; 
        }
        AllocTraits::destroy(alloc, front_node->data() + front_node->count);
    } else {
        if (size_ == 1) {
            fake_node_.next = &fake_node_;
            fake_node_.prev = &fake_node_;
        } else {
            fake_node_.next = front_node->next;
            front_node->next->prev = fake_node_.next;
        }
        NodeAllocTraits::destroy(allocator_, front_node);
        NodeAllocTraits::deallocate(allocator_, front_node, 1);
    }
    --size_;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
T& unrolled_list<T, NodeMaxSize, Allocator>::front() {
    return *static_cast<Node*>(fake_node_.next)->data();
}

template<typename T, size_t NodeMaxSize, typename Allocator>
const T& unrolled_list<T, NodeMaxSize, Allocator>::front() const {
    return *static_cast<Node*>(fake_node_.next)->data();
}

template<typename T, size_t NodeMaxSize, typename Allocator>
T& unrolled_list<T, NodeMaxSize, Allocator>::back() {
    Node* back_node = static_cast<Node*>(fake_node_.prev);
    return *(back_node->data() + (back_node->count - 1));
}

template<typename T, size_t NodeMaxSize, typename Allocator>
const T& unrolled_list<T, NodeMaxSize, Allocator>::back() const {
    Node* back_node = static_cast<Node*>(fake_node_.prev);
    return *(back_node->data() + (back_node->count - 1));
}

template<typename T, size_t NodeMaxSize, typename Allocator>
Allocator unrolled_list<T, NodeMaxSize, Allocator>::get_allocator() noexcept {
    return allocator_;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
void unrolled_list<T, NodeMaxSize, Allocator>::clear() noexcept {
    if (empty())
        return;

    size_ = 0;
    BaseNode* next_node = nullptr;
    for (BaseNode* cur = fake_node_.next; cur != &fake_node_; cur = next_node) {
        Node* node = static_cast<Node*>(cur);
        next_node = node->next;
        NodeAllocTraits::destroy(allocator_, node);        
        NodeAllocTraits::deallocate(allocator_, node, 1);
    }    
    fake_node_.next = &fake_node_;
    fake_node_.prev = &fake_node_;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
template<std::input_iterator InputIt>
void unrolled_list<T, NodeMaxSize, Allocator>::assign(
        InputIt first, InputIt last) {
    clear();
    for (InputIt it = first; it != last; ++it) {
        push_back(*it);
    }
}

template<typename T, size_t NodeMaxSize, typename Allocator>
void unrolled_list<T, NodeMaxSize, Allocator>::assign(
        std::initializer_list<T> init_list) {
    clear();
    for (const T& value : init_list) {
        push_back(value);
    }
}

template<typename T, size_t NodeMaxSize, typename Allocator>
void unrolled_list<T, NodeMaxSize, Allocator>::assign(
        size_type new_size, const T& value) {
    clear();
    for (size_t i = 0; i < new_size; ++i) {
        push_back(value);
    }
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::reverse_iterator 
        unrolled_list<T, NodeMaxSize, Allocator>::rbegin() {
    return reverse_iterator(end());
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::reverse_iterator 
        unrolled_list<T, NodeMaxSize, Allocator>::rend() {
    return reverse_iterator(begin());
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::const_reverse_iterator 
        unrolled_list<T, NodeMaxSize, Allocator>::rbegin() const {
    return const_reverse_iterator(end());
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::const_reverse_iterator 
        unrolled_list<T, NodeMaxSize, Allocator>::rend() const {
    return const_reverse_iterator(begin());
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::const_reverse_iterator 
        unrolled_list<T, NodeMaxSize, Allocator>::crbegin() const {
    return const_reverse_iterator(cend());
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::const_reverse_iterator 
        unrolled_list<T, NodeMaxSize, Allocator>::crend() const {
    return const_reverse_iterator(cbegin());
}

template<typename T, size_t NodeMaxSize, typename Allocator>
bool unrolled_list<T, NodeMaxSize, Allocator>::operator==(
        const unrolled_list& other) const {
    if (size_ != other.size_)
        return false;
    const_iterator it = begin();
    const_iterator other_it = other.begin();
    for (; it != end(); ++it, ++other_it) {
        if (*it != *other_it)
            return false;
    }
    return true;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
bool unrolled_list<T, NodeMaxSize, Allocator>::operator!=(
        const unrolled_list& other) const {
    return !(*this == other);
}

template<typename T, size_t NodeMaxSize, typename Allocator>
void unrolled_list<T, NodeMaxSize, Allocator>::swap(
        unrolled_list<T, NodeMaxSize, Allocator>& other) {
    std::swap(allocator_, other.allocator_);
    std::swap(size_, other.size_);

    BaseNode* prev_node = fake_node_.next;
    BaseNode* next_node = fake_node_.prev;
    BaseNode* other_prev_node = other.fake_node_.next;
    BaseNode* other_next_node = other.fake_node_.prev;

    if (prev_node == &fake_node_) {
        if (other_next_node == &other.fake_node_)
            return;
        
        other.fake_node_.prev = &other.fake_node_;
        other.fake_node_.next = &other.fake_node_;
        fake_node_.prev = other_next_node;
        fake_node_.next = other_prev_node;
        fake_node_.prev->next = &fake_node_;
        fake_node_.next->prev = &fake_node_;
        return;
    }

    if (other_next_node == &other.fake_node_) {
        fake_node_.prev = &fake_node_;
        fake_node_.next = &fake_node_;
        other.fake_node_.prev = next_node;
        other.fake_node_.next = prev_node;
        other.fake_node_.prev->next = &other.fake_node_;
        other.fake_node_.next->prev = &other.fake_node_;
        return;
    }
    other.fake_node_.prev = next_node;
    other.fake_node_.next = prev_node;
    fake_node_.prev = other_next_node;
    fake_node_.next = other_prev_node;

    other.fake_node_.prev->next = &other.fake_node_;
    other.fake_node_.next->prev = &other.fake_node_;
    fake_node_.prev->next = &fake_node_;
    fake_node_.next->prev = &fake_node_;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
template<std::input_iterator InputIt>
unrolled_list<T, NodeMaxSize, Allocator>::iterator 
        unrolled_list<T, NodeMaxSize, Allocator>::insert(
            const_iterator cpos, InputIt first, InputIt last) {
    iterator result(const_cast<Node*>(static_cast<const Node*>(cpos.ptr_)),
                cpos.index_);
    iterator current = result;
    size_t i = 0;
    try {
        for (InputIt it = first; it != last; ++it, ++i) {
            current = insert(current, *it);
            ++current;
        }
    } catch (...) {
        for (size_t j = 0; j < i; ++j) {
            erase(result);
        }
        throw;
    }
    return result;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::iterator 
        unrolled_list<T, NodeMaxSize, Allocator>::insert(
        iterator pos, const T& value) {
    return insert(const_iterator(pos), value);
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::iterator 
        unrolled_list<T, NodeMaxSize, Allocator>::insert(
        const_iterator cpos, const T& value) {
    Allocator alloc(allocator_);

    Node* node;
    if (cpos == cend()) {
        if (empty()) {
            ++size_;
            node = NodeAllocTraits::allocate(allocator_, 1);
            NodeAllocTraits::construct(allocator_, node, 
                                       &fake_node_, &fake_node_);
            node->count = 1;
            try {
                AllocTraits::construct(alloc, node->data(), value);
            } catch (...) {
                NodeAllocTraits::deallocate(allocator_, node, 1);
                throw;
            }
            fake_node_.prev = node;
            fake_node_.next = node;
            return begin();
        }
        ++size_;
        Node* back_node = static_cast<Node*>(fake_node_.prev);
        if (back_node->count < NodeMaxSize) {
            AllocTraits::construct(alloc, back_node->data() + back_node->count, value);
            return iterator(back_node, back_node->count++);
        }
        Node* new_node = NodeAllocTraits::allocate(allocator_, 1);
        NodeAllocTraits::construct(allocator_, new_node, back_node, &fake_node_);
        new_node->count = 1;
        try {
            AllocTraits::construct(alloc, new_node->data(), value);
        } catch (...) {
            NodeAllocTraits::deallocate(allocator_, new_node, 1);
            throw;
        }
        return iterator(new_node, 0);
    }

    node = const_cast<Node*>(static_cast<const Node*>(cpos.ptr_));

    if (node->count < NodeMaxSize) {
        for (int32_t i = node->count; i > cpos.index_; --i) {
            node->data()[i] = std::move(node->data()[i - 1]);
        }
        
        try {
            AllocTraits::construct(alloc, node->data() + cpos.index_, value);
        } catch (...) {
            for (size_t i = cpos.index_; i < node->count; ++i) {
                node->data()[i] = std::move(node->data()[i + 1]);
            }
            throw;
        }
        ++size_;
        ++node->count;
        return iterator(static_cast<BaseNode*>(node), cpos.index_);
    }
    Node* next_node;
    if (node->next == &fake_node_) {
        next_node = NodeAllocTraits::allocate(allocator_, 1);
        try {
            NodeAllocTraits::construct(allocator_, next_node, 
                                       node, &fake_node_);
            node->next = next_node;
            fake_node_.prev = next_node;
        } catch (...) {
            NodeAllocTraits::deallocate(allocator_, next_node, 1);
            throw;
        }
    } else if (static_cast<Node*>(node->next)->count == NodeMaxSize) {
        next_node = NodeAllocTraits::allocate(allocator_, 1);
        try {
            NodeAllocTraits::construct(allocator_, next_node, 
                                       node, node->next);
            node->next->prev = next_node;
            node->next = next_node;
        } catch (...) {
            NodeAllocTraits::deallocate(allocator_, next_node, 1);
            throw;
        }
    } else {
        next_node = static_cast<Node*>(node->next);
    }

    for (int32_t i = next_node->count; i > 0; --i) {
        next_node->data()[i] = std::move(next_node->data()[i - 1]);
    }
    ++next_node->count;
    next_node->data()[0] = std::move(node->data()[node->count - 1]);
    for (int32_t i = node->count - 1; i > cpos.index_; --i) {
        node->data()[i] = std::move(node->data()[i - 1]);
    }
    ++size_;
    node->data()[cpos.index_] = std::move(value);
    return iterator(static_cast<BaseNode*>(node), cpos.index_);
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::iterator 
        unrolled_list<T, NodeMaxSize, Allocator>::insert(
        const_iterator cpos, size_type size, const T& value) {
    iterator it = insert(cpos, value);
    iterator result = it;
    size_t i = 1;
    try {
        for (; i < size; ++i, it = insert(it++, value)) {}
    } catch (...) {
        for (size_t j = 0; j <= i; ++j) {
            erase(result);
        }
        throw;
    }
    return result;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::iterator 
        unrolled_list<T, NodeMaxSize, Allocator>::insert(
        const_iterator cpos, std::initializer_list<T> init_list) {
    if (!init_list.size()) 
        return iterator(const_cast<Node*>(static_cast<const Node*>(cpos.ptr_)),
                        cpos.index_);
    iterator it = insert(cpos, *init_list.begin());
    iterator result = it;
    size_t i = 0;
    try {
        for (auto init_it = init_list.begin() + 1; init_it != init_list.end(); ++init_it) {
            it = insert(++it, *init_it);
            ++i;
        }
    } catch (...) {
        for (size_t j = 0; j < i; ++j) {
            erase(result);
        }
        throw;
    }
    return result;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::iterator 
        unrolled_list<T, NodeMaxSize, Allocator>::erase(const_iterator cpos) noexcept {
    Allocator alloc(allocator_);
    Node* node = const_cast<Node*>(static_cast<const Node*>(cpos.ptr_));
    iterator result(node, cpos.index_);
    --size_;
    for (size_t i = cpos.index_; i < node->count - 1; ++i) {
        node->data()[i] = std::move(node->data()[i + 1]);
    }
    --node->count;

    BaseNode* next_node = node->next;
    BaseNode* prev_node = node->prev;
    if (node->count == 0) {
        NodeAllocTraits::destroy(allocator_, node);
        NodeAllocTraits::deallocate(allocator_, node, 1);
        if (next_node == &fake_node_) {
            if (prev_node == &fake_node_) {
                fake_node_.next = &fake_node_;
                fake_node_.prev = &fake_node_;
                return begin();
            }
            fake_node_.prev = prev_node;
            prev_node->next = &fake_node_;
            return --end();
        }
        if (prev_node == &fake_node_) {
            fake_node_.next = next_node;
            next_node->prev = &fake_node_;
            return begin();
        }
        prev_node->next = next_node;
        next_node->prev = prev_node;
        return iterator(next_node, 0);
    }

    return result;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>::iterator 
        unrolled_list<T, NodeMaxSize, Allocator>::erase(
        const_iterator first_pos, const_iterator last_pos) noexcept {
    iterator it(const_cast<Node*>(static_cast<const Node*>(first_pos)), 
                first_pos.index_);
    while (it != last_pos) {
        it = erase(it);
    }
    return it;
}

template<typename T, size_t NodeMaxSize, typename Allocator>
unrolled_list<T, NodeMaxSize, Allocator>& 
        unrolled_list<T, NodeMaxSize, Allocator>::unrolled_list::operator=(
        const unrolled_list& other) {
    if (this == &other)
        return *this;

    node_allocator_t new_allocator(allocator_);
    if constexpr(NodeAllocTraits::propagate_on_container_copy_assignment::value) {
        new_allocator = other.allocator_;
    }
    unrolled_list temp(other, new_allocator);
    swap(temp);
    return *this;
}
