#pragma once

#include <algorithm>
#include <cassert>

class ListHook {
public:
    ListHook() {
        // create unlinked node
        MakeUnlinked();
    }

    bool IsLinked() const {
        return is_linked_;
    }

    // unlink
    void Unlink() {
        // if is not linked, nothing should be done
        // this should never happen
        if (!is_linked_) {
            return;
        }

        // dummy_ should not be removed
        // this should never happen
        if (prev_ == nullptr && next_ == nullptr) {
            return;
        }

        // it is not dummy

        // the size is 1, the only two left is this and dummy_
        if (prev_ == next_) {
            ListHook* dummy = prev_;

            // make this unlink
            MakeUnlinked();

            // set the links of dummy to prev and next to nullptr
            dummy->set_next(nullptr);
            dummy->set_prev(nullptr);

            return;
        }

        // the size is at least 2
        // other is next
        // the previous of other is prev
        ListHook* next_node = next_;
        ListHook* prev_node = prev_;

        // make this unlink
        MakeUnlinked();

        // connect prev_other and other
        prev_node->set_next(next_node);
        next_node->set_prev(prev_node);
    }

    // Must unlink element from list
    ~ListHook() {
        Unlink();
    }

    ListHook(const ListHook&) = delete;

    // get previous and next
    ListHook* get_prev() {
        return prev_;
    }
    ListHook* get_next() {
        return next_;
    }

    // set previous, next and is_linked
    void set_prev(ListHook* prev) {
        prev_ = prev;
    }
    void set_next(ListHook* next) {
        next_ = next;
    }
    void set_is_linked(bool is_linked) {
        is_linked_ = is_linked;
    }

private:
    // List is allowed to access all ListHook fields
    template <class T>
    friend class List;

    // flag, whether the ListHook is linked
    bool is_linked_;

    // previous and next ListHook pointers
    ListHook* prev_;
    ListHook* next_;

    // setting parameters for unlinked node
    void MakeUnlinked() {
        prev_ = nullptr;
        next_ = nullptr;
        is_linked_ = false;
    }

    // that helper function might be useful
    void LinkBefore(ListHook* other) {

        // that should never happen 
        assert(other != nullptr);

        // that should also never happen
        assert(other->IsLinked());

        // if other has no prev and next
        // the size is 0
        if (other->get_next() == nullptr) {

            // connect to the other from behind
            set_next(other);
            other->set_prev(this);

            // the list is circled
            // connect to the another side of the other
            set_prev(other);
            other->set_next(this);

            // set up is_linked flag
            is_linked_ = true;

            return;
        }

        // other has some prev and next
        // let's get the one before the other
        ListHook* prev_other = other->get_prev();

        // connect to the other
        set_next(other);
        other->set_prev(this);

        // connect to prev_other
        set_prev(prev_other);
        prev_other->set_next(this);

        // set up is_linked flag
        is_linked_ = true;
        return;
    }
};

template <typename T>
class List {
public:
    class Iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
    public:

        Iterator(ListHook* node) {
            node_ = node;
        }

        Iterator& operator++() {
            // this should never happen
            assert(node_ != nullptr);

            // get next value
            node_ = node_->next_;
            
            // return the new iterator
            return *this;
        }

        Iterator operator++(int) {
            // save the old iterator
            Iterator old = *this;

            // shift the iterator
            ++(*this);

            // return the old iterator
            return old;
        }


        T& operator*() const {
            
            return *static_cast<T*>(node_);
        }

        T* operator->() const {
            return static_cast<T*>(node_);
        }

        bool operator==(const Iterator& rhs) const {
            return (node_ == rhs.node_);
        }
        bool operator!=(const Iterator& rhs) const {
            return (node_ != rhs.node_);
        }

    private:
        ListHook* node_;
    };

    List() {
        dummy_.is_linked_ = true;
    }

    List(const List&) = delete;
    List(List&& other) {
        // use move assignment operator
        *this = std::move(other);
    }

    // must unlink all elements from list
    ~List() {
        while (!IsEmpty()) {
            PopFront();
        }
    }

    List& operator=(const List&) = delete;
    List& operator=(List&& other) {

        

        if (other.IsEmpty()) {

            // make this also empty
            // remove links
            // make them nullptr

            dummy_.next_ = nullptr;
            dummy_.prev_ = nullptr;
        } else {

            // there exists prev and next of dummy
            ListHook* other_dummy_prev = other.dummy_.prev_;
            ListHook* other_dummy_next = other.dummy_.next_;

            // connect other_dummy_next to dummy
            other_dummy_next->prev_ = &dummy_;
            dummy_.next_ = other_dummy_next;
            
            // connect other_dummy_prev to dummy
            other_dummy_prev->next_ = &dummy_;
            dummy_.prev_ = other_dummy_prev;
        }

        // make other empty
        // remove links
        // make them nullptr
        other.dummy_.prev_ = nullptr;
        other.dummy_.next_ = nullptr;

        return *this;
         
    }

    bool IsEmpty() const {
        return dummy_.next_ == nullptr;
    }


    // that method is allowed to be O(n)
    size_t Size() const {
        if (IsEmpty()) {
            return 0;
        }

        ListHook* cur = dummy_.next_;
        size_t size = 0;
        while (cur != &dummy_) {
            cur = cur->next_;
            ++size;
        }
        return size;
    }

    // note that IntrusiveList doesn't own elements,
    // and never copies or moves T
    void PushBack(T* elem) {
        elem->LinkBefore(&dummy_);
    }

    void PushFront(T* elem) {
        if (IsEmpty()) {
            elem->LinkBefore(&dummy_);
        } else {
            ListHook* begin = dummy_.next_;
            elem->LinkBefore(begin);
        }
    }

    T& Front() {
        assert(!IsEmpty());
        T* begin = static_cast<T*>(dummy_.next_);
        return *begin;
    }

    const T& Front() const {
        assert(!IsEmpty());
        T* begin = static_cast<T*>(dummy_.next_);
        return *begin;
    }

    T& Back() {
        assert(!IsEmpty());
        T* prevend_ = static_cast<T*>(dummy_.prev_);
        return *prevend_;
    }

    const T& Back() const {
        assert(!IsEmpty());
        T* prevend_ = static_cast<T*>(dummy_.prev_);
        return *prevend_;
    }

    void PopBack() {
        assert(!IsEmpty());
        T* prevend_ = static_cast<T*>(dummy_.prev_);
        prevend_->Unlink();
    }

    void PopFront() {
        assert(!IsEmpty());
        T* begin = static_cast<T*>(dummy_.next_);
        begin->Unlink();
    }

    Iterator Begin() {
        assert(!IsEmpty());
        T* begin = static_cast<T*>(dummy_.next_);
        return IteratorTo(begin);
    }
    Iterator End() {
        return Iterator(&dummy_);
    }

    // complexity of this function must be O(1)
    Iterator IteratorTo(T* element) {
        return Iterator(element);
    }

private:
    // base case (zero size placeholder)
    // it will call constructor with no arguments
    // ListHook is default constructible
    ListHook dummy_;
};

template <typename T>
typename List<T>::Iterator begin(List<T>& list) {  // NOLINT
    return list.Begin();
}

template <typename T>
typename List<T>::Iterator end(List<T>& list) {  // NOLINT
    return list.End();
}