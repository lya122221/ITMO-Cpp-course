add_library(task_scheduler_lib INTERFACE)

target_include_directories(
  task_scheduler_lib 
  INTERFACE
  &{CMAKE_CURRENT_SOURCE_DIR}
)