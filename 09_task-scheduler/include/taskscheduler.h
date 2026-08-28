#pragma once
#include "function.h"
#include "task.h"
#include <vector>
#include "invoke.h"
#include <memory>

template <typename T>
T& GetArg(T& arg) {
  return arg;
}

template <typename T>
auto GetArg(TFuture<T>& futureArg) -> decltype(futureArg.get()) {
  return futureArg.get();
}

class TTaskScheduler {
public:
  template <typename F, typename... Args>
  TTask add(F&& task, Args&&... args) {
    auto new_task = [func = std::forward<F>(task), args...]() mutable -> decltype(auto) {
      return Invoke(func, GetArg(args)...);
    };

    std::shared_ptr<ITaskNode> new_node = std::make_shared<TTaskNode<decltype(new_task())>>(std::move(new_task));

    tasks_.push_back(new_node);
    return TTask(new_node, this);
  }
  
  void executeAll() {
    for (const std::shared_ptr<ITaskNode>& task : tasks_) {
      task->execute();
    }
  }

private:
  std::vector<std::shared_ptr<ITaskNode>> tasks_;
};

template <typename T>
struct lambda_traits;

template <typename ClassType, typename ReturnType, typename Arg>
struct lambda_traits<ReturnType(ClassType::*)(Arg) const> {
  using arg_type = Arg;
};

template <typename ClassType, typename ReturnType, typename Arg>
struct lambda_traits<ReturnType(ClassType::*)(Arg)> {
  using arg_type = Arg;
};

template <typename F>
TTask TTask::apply(F&& f) {
  using LambdaType = std::decay_t<F>;
  using ExpectedType = typename lambda_traits<decltype(&LambdaType::operator())>::arg_type;

  return scheduler_->add(std::forward<F>(f), this->getFutureResult<ExpectedType>());
}