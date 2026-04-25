#include <gtest/gtest.h>
#include "include/taskscheduler.h"

TEST(TTaskTest, GetResultSyncAfterExecute) {
    TTaskScheduler scheduler;
    auto task = scheduler.add([](int x) { return x * 2; }, 21);
    
    scheduler.executeAll();
    
    EXPECT_EQ(task.getResultSync<int>(), 42);
}

TEST(TTaskTest, GetResultSyncLazyExecution) {
    TTaskScheduler scheduler;
    bool was_executed = false;
    
    auto task = scheduler.add([&was_executed]() { 
        was_executed = true; 
        return 100; 
    });
    
    int res = task.getResultSync<int>();
    
    EXPECT_EQ(res, 100);
    EXPECT_TRUE(was_executed);
}

TEST(TTaskTest, GetResultSyncThrowsOnTypeMismatch) {
    TTaskScheduler scheduler;
    auto task = scheduler.add([]() { return 42; });
    
    EXPECT_THROW(task.getResultSync<std::string>(), std::runtime_error);
}

TEST(TTaskTest, VoidTaskExecution) {
    TTaskScheduler scheduler;
    int val = 0;
    auto task = scheduler.add([&val]() { val = 5; });
    
    scheduler.executeAll();
    EXPECT_EQ(val, 5);
}

TEST(TTaskApplyTest, ChainedApplyChangesTypes) {
    TTaskScheduler scheduler;
    
    auto task1 = scheduler.add([]() { return 10; });
    
    auto task2 = task1.apply([](int val) {
        return std::to_string(val) + " apples";
    });
    
    auto task3 = task2.apply([](std::string s) {
        return s.length();
    });
    
    scheduler.executeAll();
    
    EXPECT_EQ(task3.getResultSync<size_t>(), 9);
}

TEST(TTaskApplyTest, LazyChainExecution) {
    TTaskScheduler scheduler;
    bool task1_run = false;
    bool task2_run = false;
    
    auto task1 = scheduler.add([&task1_run]() { 
        task1_run = true; 
        return 5; 
    });
    
    auto task2 = task1.apply([&task2_run](int x) { 
        task2_run = true; 
        return x * 10; 
    });
    
    
    int result = task2.getResultSync<int>();
    
    EXPECT_EQ(result, 50);
    EXPECT_TRUE(task1_run);
    EXPECT_TRUE(task2_run);
}

TEST(AdvancedSchedulerTest, BranchingGraphWithReferences) {
    TTaskScheduler scheduler;
    int shared_data = 42;
    
    auto base_task = scheduler.add([&shared_data]() -> int& { return shared_data; });
    
    auto fut1 = base_task.getFutureResult<int&>();
    auto fut2 = base_task.getFutureResult<int&>();
    
    auto branch1 = scheduler.add([](int& val) { return val + 10; }, fut1);
    auto branch2 = scheduler.add([](int& val) { return val * 2; }, fut2);
    
    scheduler.executeAll();
    
    EXPECT_EQ(branch1.getResultSync<int>(), 52);
    EXPECT_EQ(branch2.getResultSync<int>(), 84);
}

TEST(TTaskApplyTest, ApplyWithReferenceArgument) {
    TTaskScheduler scheduler;
    int state = 10;
    
    auto task1 = scheduler.add([&state]() -> int& { return state; });
    
    auto task2 = task1.apply([](int& val) {
        val += 5;
        return val;
    });
    
    int result = task2.getResultSync<int>();
    
    EXPECT_EQ(result, 15);
    EXPECT_EQ(state, 15);
}

TEST(TTaskTest, VoidTaskGetResultSync) {
    TTaskScheduler scheduler;
    int counter = 0;
    auto task = scheduler.add([&counter]() { counter++; });
    
    task.getResultSync<void>();
    
    EXPECT_EQ(counter, 1);
}

TEST(TTaskTest, GetResultSyncThrowsOnReferenceTypeMismatch) {
    TTaskScheduler scheduler;
    auto task = scheduler.add([]() { return 42; });
    
    EXPECT_THROW(task.getResultSync<std::string&>(), std::runtime_error);
}