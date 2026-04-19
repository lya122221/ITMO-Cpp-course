#include "function.h"
#include "task.h"
#include <vector>

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
    auto task = [func = std::forward<F>(f), args...]() {
      func(GetArg(args)...); 
    };

    return TTask(std::move(task));
  }
  
  void executeAll() {
  }

private:
  std::vector<TTask*> tasks_;
};