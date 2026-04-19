#include "function.h"
#include "task.h"
#include <vector>
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
    auto new_task = [func = std::forward<F>(task), args...]() mutable {
      return func(GetArg(args)...); 
    };

    std::shared_ptr<ITaskNode> new_node = std::make_shared<TTaskNode<decltype(new_task())>>(std::move(new_task));

    tasks_.push_back(new_node);
    return TTask(new_node);
  }
  
  void executeAll() {
    for (const std::shared_ptr<ITaskNode>& task : tasks_) {
      task->execute();
    }
  }

private:
  std::vector<std::shared_ptr<ITaskNode>> tasks_;
};