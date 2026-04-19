#include "function.h"
#include <vector>

template <typename R>
class TFuture {
public:
  R get() {
  
  }
private:

};

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

  R getResult() {
    if (!result) {
      execute();
    }
    return *result;
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
private:
  Function<void()> task_;
};

class TTask {
public:
  TTask(std::shared_ptr<ITaskNode> node) : task_(std::move(node)) {}

  template <typename R>
  auto getResultSync() {
    auto* node = dynamic_cast<TTaskNode<R>*>(task_.get());

    if (!node) {
      throw std::runtime_error("Wrong type for getResultSync");
    }

    return node->getResult();
  }
  
  template <typename R>
  TFuture<R> getFutureResult() {

  }
  TTask apply() {

  }
private:
  std::shared_ptr<ITaskNode> task_;
};