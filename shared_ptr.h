#ifndef SHARED_PTR_H_
#define SHARED_PTR_H_

#include <cassert>

namespace pointers {

template<typename T>
class SharedPtr {
 public:
  SharedPtr() = default;

  explicit SharedPtr(T* ptr) : pointer_{ptr} {
    if (pointer_ != nullptr) {
      counter_ = new Counter(1, 0);
    }
  }

  SharedPtr(const SharedPtr& other_ptr)
      : pointer_{other_ptr.pointer_}, counter_{other_ptr.counter_} {
    if (counter_ != nullptr) {
      (counter_->num_of_shared)++;
    }
  }
  SharedPtr(SharedPtr&& other_ptr) noexcept
      : pointer_{other_ptr.pointer_}, counter_{other_ptr.counter_} {
    other_ptr.pointer_ = nullptr;
    other_ptr.counter_ = nullptr;
  }

  SharedPtr& operator=(const SharedPtr& other_ptr) {
    if (pointer_ == other_ptr.pointer_) {
      return (*this);
    }
    Reset();
    pointer_ = other_ptr.pointer_;
    counter_ = other_ptr.counter_;
    if (counter_ != nullptr) {
      counter_->num_of_shared++;
    }
    return (*this);
  }
  SharedPtr& operator=(SharedPtr&& other_ptr) {
    // The same.
    if (this == &other_ptr) {
      return (*this);
    }
    // Equal but not the same.
    if (pointer_ == other_ptr.pointer_) {
      if (counter_ != nullptr) {
        counter_->num_of_shared--;
      }
      other_ptr.pointer_ = nullptr;
      other_ptr.counter_ = nullptr;
      return (*this);
    }
    Reset();
    pointer_ = other_ptr.pointer_;
    counter_ = other_ptr.counter_;
    other_ptr.pointer_ = nullptr;
    other_ptr.counter_ = nullptr;
    return (*this);
  }

  void Reset() {
    if (counter_ != nullptr) {
      counter_->num_of_shared--;
      if (counter_->num_of_shared == 0) {
        delete pointer_;
        pointer_ = nullptr;
        if (counter_->num_of_weak == 0) {
          delete counter_;
          counter_ = nullptr;
        }
      }
    }
    pointer_ = nullptr;
    counter_ = nullptr;
  }

  ~SharedPtr() {
    Reset();
  }

  T* Get() {
    return pointer_;
  }
  T& operator*() {
    assert(pointer_ != nullptr);
    return (*pointer_);
  }
  T* operator->() {
    return pointer_;
  }
  bool operator==(const SharedPtr& other_ptr) const {
    return pointer_ == other_ptr.pointer_;
  }
  bool operator!=(const SharedPtr& other_ptr) const {
    return !(*this == other_ptr);
  }
  bool operator==(const T* other_ptr) const {
    return pointer_ == other_ptr;
  }
  bool operator!=(const T* other_ptr) const {
    return !(*this == other_ptr);
  }

  int GetCounter() const {
    return counter_->num_of_shared;
  }

 private:
  struct Counter {
    explicit Counter(int shared, int weak)
        : num_of_shared{shared}, num_of_weak{weak} {}
    int num_of_shared;
    int num_of_weak;
  };

  T* pointer_ = nullptr;
  Counter* counter_ = nullptr;

  friend bool operator==(const T* lhs, const SharedPtr<T>& rhs) {
    return lhs == rhs.pointer_;
  }

  friend bool operator!=(const T* lhs, const SharedPtr<T>& rhs) {
    return !(lhs == rhs);
  }

  template <typename V>
  friend class WeakPtr;

  SharedPtr(T* ptr, Counter* counter) : pointer_{ptr}, counter_{counter} {
    counter_->num_of_shared++;
  }
};

}  // namespace pointers

#endif  // SHARED_PTR_H_
