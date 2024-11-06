#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <utility>

template <class T> class Vector {
  public:
    using ValueType = T;
    using SizeType = size_t;
    using Difference_type = std::ptrdiff_t;
    using Reference = T &;
    using ConstReference = const ValueType &;
    using Pointer = T *;
    using ConstPointer = const T *;
    using Iterator = T *;
    using ConstIterator = const T *;
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

    // Consturctors
    Vector() : cap_(0), size_(0), data_(nullptr) {
    }

    Vector(const Vector &vec)
        : size_(vec.size_), cap_(vec.cap_), data_(nullptr) {
        if (cap_ > 0) {
            data_ = new T[cap_];
            for (SizeType i = 0; i < size_; ++i) {
                data_[i] = vec.data_[i];
            }
        }
    }

    explicit Vector(SizeType count, const T &value) : Vector() {
        if (count > 0) {
            Reserve(count);
            size_ = count;
            for (SizeType i = 0; i < size_; ++i) {
                data_[i] = value;
            }
        } else {
            Vector();
        }
    }

    explicit Vector(SizeType count) : Vector() {
        Reserve(count);
        size_ = count;
        for (SizeType i = 0; i < size_; ++i) {
            data_[i] = T();
        }
    }

    explicit Vector(SizeType count, T &&value) : Vector() {
        if (count > 0) {
            Reserve(count);
            size_ = count;
            for (SizeType i = 0; i < size_ - 1; ++i) {
                data_[i] = value;
            }
            data_[size_ - 1] = std::move(value);
        } else {
            Vector();
        }
    }

    Vector(Vector &&vec)
        : size_(vec.Size()), cap_(vec.Capacity()), data_(vec.Data()) {
        vec.size_ = 0;
        vec.cap_ = 0;
        vec.data_ = nullptr;
    }

    template <class InputIt, typename = typename std::iterator_traits<
                                 InputIt>::iterator_category>
    Vector(InputIt first, InputIt last) : size_(0), cap_(0), data_(nullptr) {
        if (first != last) {
            SizeType count = std::distance(first, last);
            Reserve(count);
            for (auto it = first; it != last; ++it) {
                data_[size_++] = *it;
            }
        }
    }

    Vector(std::initializer_list<T> init)
        : size_(init.size()), cap_(init.size()), data_(new T[init.size()]) {
        std::copy(init.begin(), init.end(), data_);
    }

    Vector &operator=(const Vector &other) {
        Reserve(other.Size());
        size_ = other.Size();
        for (SizeType i = 0; i < size_; ++i) {
            data_[i] = other[i];
        }
        return *this;
    }

    Vector &operator=(Vector &&other) noexcept {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            cap_ = other.cap_;
            data_ = other.data_;
            other.size_ = 0;
            other.cap_ = 0;
            other.data_ = nullptr;
        }
        return *this;
    }

    Vector &operator=(std::initializer_list<ValueType> ilist) {
        size_ = 0;
        Reserve(ilist.size());
        for (auto elem : ilist) {
            data_[size_++] = elem;
        }
        return *this;
    }

    // Assigners
    void Assign(SizeType count, const T &value) {
        if (count > cap_) {
            Reserve(count);
        }
        size_ = std::max(count, size_);
        for (SizeType i = 0; i < count; ++i) {
            data_[i] = std::move(value);
        }
    }

    void Assign(std::initializer_list<T> ilist) {
        if (ilist.Size() > cap_) {
            Reserve(ilist.size());
        }
        size_ = max(ilist.Size(), size_);
        SizeType indx = 0;
        for (auto elem : ilist) {
            data_[indx++] = elem;
        }
    }

    // TODO : Assign - range

    // Destructors
    ~Vector() {
        cap_ = 0;
        size_ = 0;
        if (data_ != nullptr) {
            delete[] data_;
        }
        data_ = nullptr;
    }

    // Access
    Reference At(SizeType pos) {
        if (pos >= size_) {
            throw std::out_of_range("out_of_range");
        }
        return data_[pos];
    }

    ConstReference At(SizeType pos) const {
        if (pos >= size_) {
            throw std::out_of_range("out_of_range");
        }
        return data_[pos];
    }

    Reference operator[](SizeType pos) {
        if (pos >= size_) {
            throw std::out_of_range("out_of_range");
        }
        return data_[pos];
    }

    ConstReference operator[](SizeType pos) const {
        return data_[pos];
    }

    Reference Front() {
        return data_[0];
    }

    ConstReference Front() const {
        return data_[0];
    }

    Reference Back() {
        return data_[size_ - 1];
    }

    ConstReference Back() const {
        return data_[size_ - 1];
    }

    T *Data() {
        return data_;
    }

    const T *Data() const {
        return data_;
    }

    // Iterators
    Iterator begin() {
        return data_;
    }

    ConstIterator begin() const {
        return data_;
    }

    Iterator end() {
        return data_ + size_;
    }

    ConstIterator end() const {
        return data_ + size_;
    }

    ConstIterator cbegin() const {
        return begin();
    }

    ConstIterator cend() const {
        return end();
    }

    ReverseIterator rbegin() {
        return std::reverse_iterator(data_ + (size_));
    }

    ConstReverseIterator rbegin() const {
        return std::reverse_iterator(data_ + (size_));
    }

    ReverseIterator rend() {
        return std::reverse_iterator(data_);
    }

    ConstReverseIterator rend() const {
        return std::reverse_iterator(data_);
    }

    // Capacity
    bool Empty() const {
        return size_ == 0;
    }

    SizeType Size() const {
        return size_;
    }

    SizeType GetSize() const {
        return size_;
    }

    bool IsEmpty() const {
        return size_ == 0;
    }

    SizeType GetCapacity() const {
        return cap_;
    }

    SizeType Max_size() const {
        return 10;
    }

    SizeType Capacity() const {
        return cap_;
    }

    void Reserve(SizeType new_cap) {
        if (new_cap > cap_) {
            Change_cap(new_cap);
        }
    }

    SizeType Size() {
        return size_;
    }

    void ShrinkToFit() {
        if (size_ != cap_) {
            Change_cap(size_);
        }
    }

    // Modifiers
    void Clear() {
        size_ = 0;
    }

    Iterator Insert(ConstIterator pos, const T &value) {
        Insert(std::move(value));
    }

    Iterator Insert(ConstIterator pos, T &&value) {
        Difference_type index = pos - cbegin();
        PushBack(T());
        for (Difference_type i = size_ - 1; i > index; --i) {
            data_[i] = std::move(data_[i - 1]);
        }
        return data_ + index;
    }

    template <class... Args>
    Iterator Emplace(ConstIterator pos, Args &&...args) {
        data_[pos] = T(std::forward(args)...);
    }

    Iterator Erase(ConstIterator pos) {
        for (Iterator i = pos; i != rbegin(); ++i) {
            data_[i] = std::move(data_[i + 1]);
        }
        PopBack();
    }

    void PushBack(const T &value) {
        if (size_ == cap_) {
            Reserve(cap_ > 0 ? cap_ * RESIZE_COEFF : 1);
        }
        data_[size_++] = value;
    }

    void PushBack(T &&value) {
        if (size_ == cap_)
            Reserve(cap_ > 0 ? cap_ * RESIZE_COEFF : 1);
        data_[size_++] = std::move(value);
    }

    template <class... Args> Reference Emplace_back(Args &&...args) {
        if (cap_ == size_) {
            Reserve(cap_ * RESIZE_COEFF);
        }
        data_[size_++] = T(std::forward(args)...);
    }

    void PopBack() {
        if (size_ > 0) {
            size_--;
            data_[size_].~T();
        }
    }

    Vector<ValueType> &Resize(SizeType count) {
        SizeType old_size = size_;
        auto old_data = data_;
        auto old_cap = cap_;
        try {
            return Resize(count, T());
        } catch (...) {
            data_ = old_data;
            cap_ = old_cap;
            size_ = old_size;
            throw;
        }
    }

    Vector<ValueType> &Resize(SizeType count, const ValueType &value) {
        SizeType old_size = size_;
        auto old_data = data_;
        auto old_cap = cap_;
        try {
            if (count < size_) {
                while (count < size_) {
                    PopBack();
                }

            } else if (count > size_) {
                while (count > size_) {
                    PushBack(value);
                }
            }
            return *this;
        } catch (...) {
            data_ = old_data;
            cap_ = old_cap;
            size_ = old_size;
            throw;
        }
    }

    Vector<ValueType> &Resize(SizeType count, ValueType &&value) {
        if (count < size_) {
            while (count < size_) {
                PopBack();
            }
        } else if (count > size_) {
            while (count > size_) {
                PushBack(std::move(value));
            }
        }
        return *this;
    }

    void Swap(Vector &other) {
        std::swap(size_, other.size_);
        std::swap(cap_, other.cap_);
        std::swap(data_, other.data_);
    }

  private:
    const int RESIZE_COEFF = 2;
    SizeType cap_;
    SizeType size_;
    ValueType *data_;

    void Change_cap(SizeType new_cap) {
        if (new_cap == 0) {
            delete[] data_;
            data_ = nullptr;
            cap_ = 0;
            size_ = 0;
            return;
        }
        ValueType *new_data = new T[new_cap];

        SizeType elements_to_move = std::min(new_cap, size_);

        for (SizeType i = 0; i < elements_to_move; ++i) {
            new_data[i] = std::move(data_[i]);
        }
        delete[] data_;
        data_ = new_data;
        cap_ = new_cap;
        size_ = elements_to_move;
    }
};

template <typename T>
bool operator==(const Vector<T> &lhs, const Vector<T> &rhs) {
    if (lhs.Size() != rhs.Size()) {
        return false;
    }
    for (size_t i = 0; i < lhs.Size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

template <typename T>
bool operator<(const Vector<T> &lhs, const Vector<T> &rhs) {
    for (size_t i = 0; i < std::min(lhs.Size(), rhs.Size()); ++i) {
        if (lhs[i] < rhs[i]) {
            return true;
        } else if (lhs[i] > rhs[i]) {
            return false;
        }
    }
    return lhs.Size() < rhs.Size();
}

template <typename T>
bool operator!=(const Vector<T> &lhs, const Vector<T> &rhs) {
    return !(lhs == rhs);
}

template <typename T>
bool operator<=(const Vector<T> &lhs, const Vector<T> &rhs) {
    return lhs < rhs || lhs == rhs;
}

template <typename T>
bool operator>(const Vector<T> &lhs, const Vector<T> &rhs) {
    return rhs < lhs;
}

template <typename T>
bool operator>=(const Vector<T> &lhs, const Vector<T> &rhs) {
    return !(lhs < rhs);
}