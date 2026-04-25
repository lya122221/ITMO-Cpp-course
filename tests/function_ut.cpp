#include <gtest/gtest.h>
#include <string>
#include <memory>

#include "include/function.h" 

int add_five(int x) { return x + 5; }

struct Multiplier {
  int factor;
  int operator()(int x) const { return x * factor; }
};

TEST(FunctionTest, FreeFunction) {
  Function<int(int)> f = add_five;
  EXPECT_EQ(f(10), 15);
}

TEST(FunctionTest, LambdaNoCapture) {
  Function<int(int, int)> f = [](int a, int b) { return a * b; };
  EXPECT_EQ(f(3, 4), 12);
}

TEST(FunctionTest, LambdaWithCapture) {
  int multiplier = 10;
  Function<int(int)> f = [multiplier](int x) { return x * multiplier; };
  EXPECT_EQ(f(5), 50);
}

TEST(FunctionTest, FunctorObject) {
  Function<int(int)> f = Multiplier{3};
  EXPECT_EQ(f(10), 30);
}

TEST(FunctionTest, CopySemantics) {
  int value = 5;
  Function<int()> f1 = [&value]() { return ++value; };
  Function<int()> f2 = f1;

  EXPECT_EQ(f1(), 6);
  EXPECT_EQ(f2(), 7);
}

TEST(FunctionTest, MoveSemantics) {
  Function<int(int)> f1 = [](int x) { return x + 1; };
  Function<int(int)> f2 = std::move(f1);

  EXPECT_EQ(f2(10), 11);
  EXPECT_THROW(f1(10), std::bad_function_call);
}

TEST(FunctionTest, EmptyFunctionThrows) {
  Function<void()> f;
  EXPECT_THROW(f(), std::bad_function_call);
}

TEST(FunctionTest, PerfectForwarding) {
  Function<std::unique_ptr<int>(std::unique_ptr<int>)> f = [](std::unique_ptr<int> p) {
    return p; 
  };
  
  auto ptr = std::make_unique<int>(42);
  auto result = f(std::move(ptr));
  EXPECT_EQ(*result, 42);
}

TEST(FunctionTest, EmptyCopyAndAssign) {
  Function<int()> empty1;
  
  Function<int()> empty2 = empty1; 
  EXPECT_THROW(empty2(), std::bad_function_call);
  
  Function<int()> empty3;
  empty3 = empty1;
  EXPECT_THROW(empty3(), std::bad_function_call);
  
  Function<int()> empty4 = std::move(empty1);
  EXPECT_THROW(empty4(), std::bad_function_call);
}