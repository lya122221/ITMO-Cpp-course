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
class TTaskNode : ITaskNode {
public:
  TTaskNode(Function<R()>&& task) : task_(std::move(func)) {}

  void execute() override {
  }
private:
  std::unique_ptr<R> result_;
  Function<R()> task_;
};

class TTask {
public:
  template <typename R>
  TTask(Function<R()> task) : task_(std::make_shared<ITaskNode>(TTaskNode<R>(std::move(task)))) {}

  template <typename R>
  auto getResultSync() {

  }
  
  template <typename R>
  TFuture<R> getFutureResult() {

  }
  TTask apply() {

  }
private:
  std::shared_ptr<ITaskNode> task_;
};