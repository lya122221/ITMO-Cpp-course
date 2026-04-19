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
private:
  std::unique_ptr<R> result_;
  Function<R()> task_;
};

class TTask {
public:
  TTask(std::shared_ptr<ITaskNode> node) : task_(std::move(node)) {}

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