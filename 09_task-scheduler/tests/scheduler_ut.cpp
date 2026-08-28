
#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include "include/taskscheduler.h" 

TEST(TTaskSchedulerTest, AddTaskNoArgs) {
    TTaskScheduler scheduler;
    int counter = 0;
    
    scheduler.add([&counter]() { counter++; });
    
    EXPECT_EQ(counter, 0);
}

TEST(TTaskSchedulerTest, AddTaskWithArgs) {
    TTaskScheduler scheduler;
    std::string result;
    
    auto concat = [](const std::string& a, const std::string& b) {
        return a + b;
    };
    
    scheduler.add(concat, "Hello, ", "World!");
    SUCCEED();
}

TEST(TTaskSchedulerTest, ExecuteAllRunsAllTasks) {
    TTaskScheduler scheduler;
    int a = 0, b = 0;
    
    scheduler.add([&a]() { a = 10; });
    scheduler.add([&b]() { b = 20; });
    
    scheduler.executeAll();
    
    EXPECT_EQ(a, 10);
    EXPECT_EQ(b, 20);
}

TEST(AdvancedSchedulerTest, ManyArguments) {
    TTaskScheduler scheduler;
    
    auto sum5 = [](int a, int b, int c, int d, int e) {
        return a + b + c + d + e;
    };
    
    auto task = scheduler.add(sum5, 1, 2, 3, 4, 5);
    scheduler.executeAll();
    
    EXPECT_EQ(task.getResultSync<int>(), 15);
}

struct TMathHelper {
    int Multiply(int a, int b) { return a * b; }
};

TEST(AdvancedSchedulerTest, ClassMethodAsTask) {
    TTaskScheduler scheduler;
    TMathHelper helper;
    
    auto task = scheduler.add(&TMathHelper::Multiply, &helper, 7, 8);
    
    scheduler.executeAll();
    
    EXPECT_EQ(task.getResultSync<int>(), 56);
}

TEST(AdvancedSchedulerTest, MoveOnlyTypePassedThroughFuture) {
    TTaskScheduler scheduler;
    
    auto provider_task = scheduler.add([]() { 
        return std::make_unique<int>(1024); 
    });
    
    auto future_ptr = provider_task.getFutureResult<std::unique_ptr<int>>();
    
    auto consumer_task = scheduler.add([](std::unique_ptr<int> ptr) { 
        return *ptr + 100; 
    }, future_ptr);

    scheduler.executeAll();
    
    EXPECT_EQ(consumer_task.getResultSync<int>(), 1124);
}

TEST(TTaskSchedulerTest, EmptySchedulerExecute) {
    TTaskScheduler scheduler;
    
    EXPECT_NO_THROW(scheduler.executeAll());
}

TEST(TTaskSchedulerTest, AddTaskAfterExecuteAll) {
    TTaskScheduler scheduler;
    int state = 0;
    
    scheduler.add([&state]() { state = 1; });
    scheduler.executeAll();
    EXPECT_EQ(state, 1);
    
    scheduler.add([&state]() { state = 2; });
    scheduler.executeAll();
    
    EXPECT_EQ(state, 2);
}