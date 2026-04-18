#include <memory>

template <typename T>
class Function;

template <typename R, typename... Args>
class Function<R(Args...)> {
public:
  Function() = default;
  template <typename T>
  Function(T&& f) : callable_(new Derived<std::decay_t<T>>(std::forward<T>(f))) {}
  ~Function() { delete callable_; }

  R operator()(Args... args) const {
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