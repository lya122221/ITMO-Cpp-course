#include <type_traits>

template <typename F, typename... Args, typename = std::enable_if_t<!std::is_member_function_pointer_v<std::decay_t<F>>>>
decltype(auto) Invoke(F&& f, Args&&... args) {
  return std::forward<F>(f)(std::forward<Args>(args)...);
}

template <typename R, typename ClassType, typename... MethodArgs, typename Obj, typename... Args>
decltype(auto) Invoke(R (ClassType::*method)(MethodArgs...), Obj&& obj, Args&&... args) {
  if constexpr (std::is_pointer_v<std::decay_t<Obj>>) {
    return (std::forward<Obj>(obj)->*method)(std::forward<Args>(args)...);
  } else {
    return (std::forward<Obj>(obj).*method)(std::forward<Args>(args)...);
  }
}

template <typename R, typename ClassType, typename... MethodArgs, typename Obj, typename... Args>
decltype(auto) Invoke(R (ClassType::*method)(MethodArgs...) const, Obj&& obj, Args&&... args) {
  if constexpr (std::is_pointer_v<std::decay_t<Obj>>) {
    return (std::forward<Obj>(obj)->*method)(std::forward<Args>(args)...);
  } else {
    return (std::forward<Obj>(obj).*method)(std::forward<Args>(args)...);
  }
}