#include "function.h"
#include <vector>

class ITaskNode {
public:
  virtual ~ITaskNode() = default;
  virtual void execute() = 0;
};

template <typename R>
class TTaskNode : public ITaskNode {
public:
  TTaskNode(Function<R()>&& task) : task_(std::move(task)) {}

  void execute() override {
    if (!result_) {
      result_ = std::make_unique<R>(task_());
    }
  }

  R& getResult() {
    if (!result_) {
      execute();
    }
    return *result_;
  }

private:
  std::unique_ptr<R> result_;
  Function<R()> task_;
};

template <>
class TTaskNode<void> : public ITaskNode {
public:
  TTaskNode(Function<void()>&& task) : task_(std::move(task)) {}

  void execute() override {
    task_();
  }

  void getResult() {
    execute();
  }

private:
  Function<void()> task_;
};

template <typename R>
class TTaskNode<R&> : public ITaskNode {
public:
  TTaskNode(Function<R&()>&& task) : task_(std::move(task)) {}

  void execute() override {
    if (!result_ptr_) {
      result_ptr_ = &task_(); 
    }
  }

  R& getResult() {
    if (!result_ptr_) {
      execute();
    }
    return *result_ptr_;
  }

private:
  R* result_ptr_; 
  Function<R&()> task_;
};

template <typename R>
class TFuture<R&> {
public:
  TFuture(std::shared_ptr<ITaskNode> task) : task_(std::move(task)) {}

  R& get() {
    auto* node = dynamic_cast<TTaskNode<R>*>(task_.get());

    if (!node) {
      throw std::runtime_error("Wrong type for getResultSync");
    }

    return node->getResult();
  }

private:
  std::shared_ptr<ITaskNode> task_;
};

template <typename R>
class TFuture {
public:
  TFuture(std::shared_ptr<ITaskNode> task) : task_(std::make_shared<ITaskNode>(std::move(task))) {}

  R get() {
    if (moved_) {
      throw std::runtime_error("Result already moved");
    }

    auto* node = dynamic_cast<TTaskNode<R>*>(task_.get());

    if (!node) {
      throw std::runtime_error("Wrong type for get in TFuture");
    }

    moved_ = true;
    return std::move(node->getResult());
  }

private:
  std::shared_ptr<ITaskNode> task_;
  bool moved_ = false;
};

class TTask {
public:
  TTask(std::shared_ptr<ITaskNode> node) : task_(std::move(node)) {}

  template <typename R>
  decltype(auto) getResultSync() {
    auto* node = dynamic_cast<TTaskNode<R>*>(task_.get());

    if (!node) {
      throw std::runtime_error("Wrong type for getResultSync");
    }

    return node->getResult();
  }
  
  template <typename R>
  TFuture<R> getFutureResult() {
    return TFuture(task_);
  }

  TTask apply() {

  }
private:
  std::shared_ptr<ITaskNode> task_;
};