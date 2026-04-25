#pragma once
#include <memory>

template <typename T>
class Function;

template <typename R, typename... Args>
class Function<R(Args...)> {
public:
  Function() : callable_(nullptr) {}
  template <typename T>
  Function(T&& f) : callable_(new Derived<std::decay_t<T>>(std::forward<T>(f))) {}
  ~Function() { delete callable_; }

  Function(const Function& other) {
    if (other.callable_) {
      callable_ = other.callable_->GetCopy(); 
    } else {
      callable_ = nullptr;
    }
  }

  Function(Function&& other) noexcept {
    callable_ = other.callable_;
    other.callable_ = nullptr;
  }

  Function& operator=(const Function& other) {
    if (this != &other) {
      delete callable_;
      if (other.callable_) {
        callable_ = other.callable_->GetCopy();
      } else {
        callable_ = nullptr;
      }
    }
    return *this;
  }

  Function& operator=(Function&& other) noexcept {
    if (this != &other) {
      delete callable_;
      callable_ = other.callable_;
      other.callable_ = nullptr;
    }
    return *this;
  }

  R operator()(Args... args) const {
    if (!callable_) {
      throw std::bad_function_call();
    }
    return callable_->invoke(std::forward<Args>(args)...);
  }
private:
  struct Base {
    virtual R invoke(Args... args) = 0; 
    virtual ~Base() = default;
    virtual Base* GetCopy() const = 0;
  };

  template <typename T>
  struct Derived : Base {
    T func_;
    Derived(const T& func) : func_(func) {}
    Derived(T&& func) : func_(std::move(func)) {}

    ~Derived() override = default;

    Base* GetCopy() const override {
      return new Derived<T>(func_);
    }

    R invoke(Args... args) override {
      return func_(std::forward<Args>(args)...);
    }
  };

  Base* callable_;
};