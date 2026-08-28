#include <gtest/gtest.h>

#include "include/taskscheduler.h"

TEST(TFutureTest, ResolveFutureAsArgument) {
    TTaskScheduler scheduler;
    
    auto task1 = scheduler.add([]() { return 10; });
    TFuture<int> future1 = task1.getFutureResult<int>();
    
    auto task2 = scheduler.add([](int a, int b) { 
        return a + b; 
    }, future1, 20);
    
    scheduler.executeAll();
    
    EXPECT_EQ(task2.getResultSync<int>(), 30);
}

TEST(TFutureTest, ThrowsOnSecondGetForValue) {
    TTaskScheduler scheduler;
    auto task = scheduler.add([]() { return 42; });
    auto future = task.getFutureResult<int>();
    
    scheduler.executeAll();
    
    EXPECT_EQ(future.get(), 42);
    
    EXPECT_THROW(future.get(), std::runtime_error);
}

TEST(TFutureTest, MultipleGetAllowedForReference) {
    TTaskScheduler scheduler;
    int global_val = 100;
    
    auto task = scheduler.add([&global_val]() -> int& { return global_val; });
    auto future = task.getFutureResult<int&>();
    
    scheduler.executeAll();
    
    EXPECT_EQ(future.get(), 100);
    EXPECT_EQ(future.get(), 100);
    
    future.get() = 200;
    EXPECT_EQ(global_val, 200);
}

TEST(TFutureTest, TypeMismatchThrowsInFuture) {
    TTaskScheduler scheduler;
    auto task = scheduler.add([]() { return 3.14f; });
    
    auto future = task.getFutureResult<int>(); 
    
    scheduler.executeAll();
    
    EXPECT_THROW(future.get(), std::runtime_error);
}

TEST(AdvancedSchedulerTest, MultipleFuturesInOneTask) {
    TTaskScheduler scheduler;
    
    auto taskA = scheduler.add([]() { return 100; });
    auto taskB = scheduler.add([]() { return 500; });
    
    auto futA = taskA.getFutureResult<int>();
    auto futB = taskB.getFutureResult<int>();
    
    auto taskC = scheduler.add([](int a, int b) { 
        return a + b; 
    }, futA, futB);
    
    scheduler.executeAll();
    
    EXPECT_EQ(taskC.getResultSync<int>(), 600);
}

TEST(AdvancedSchedulerTest, MoveOnlyTypes) {
    TTaskScheduler scheduler;
    
    auto producer = scheduler.add([]() { 
        return std::make_unique<int>(777); 
    });
    
    auto fut = producer.getFutureResult<std::unique_ptr<int>>();
    
    auto consumer = scheduler.add([](std::unique_ptr<int> ptr) { 
        return *ptr; 
    }, fut);
    
    scheduler.executeAll();
    
    EXPECT_EQ(consumer.getResultSync<int>(), 777);
}

TEST(TFutureTest, ReferenceFutureTypeMismatchThrows) {
    TTaskScheduler scheduler;
    
    auto task = scheduler.add([]() { return 100; });
    
    auto future = task.getFutureResult<int&>();
    
    scheduler.executeAll();
    
    EXPECT_THROW(future.get(), std::runtime_error);
}