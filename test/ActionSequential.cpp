//#include <memory>

#include "gtest/gtest.h"
#include "../src/action/ActionSequential.hpp"

TEST(ActionSequential, NoSubActions_ExecutionFinishes) {
  ActionSequential action({});
  action.start();

  EXPECT_EQ(true, action.execute());
}

TEST(ActionSequential, 2SubActions_Executes2Times) {
  ActionSequential action({std::make_shared<Action>(), std::make_shared<Action>()});
  action.start();

  EXPECT_EQ(false, action.execute());
  EXPECT_EQ(false, action.execute());
  EXPECT_EQ(true, action.execute());
}

TEST(ActionSequential, 2SubActions_Loop3Times_Executes6Times) {
  ActionSequential action({std::make_shared<Action>(), std::make_shared<Action>()}, 3);
  action.start();

  EXPECT_EQ(false, action.execute());
  EXPECT_EQ(false, action.execute());
  EXPECT_EQ(false, action.execute());
  EXPECT_EQ(false, action.execute());
  EXPECT_EQ(false, action.execute());
  EXPECT_EQ(false, action.execute());
  EXPECT_EQ(true, action.execute());
}

TEST(ActionSequential, 2SubActions_LoopForever_ExecutesAtLeast100Times) {
  ActionSequential action({std::make_shared<Action>(), std::make_shared<Action>()}, ActionSequential::LOOP_FOREVER);
  action.start();

  for (int i = 0; i < 100; i++) {
    EXPECT_EQ(false, action.execute());
  }
}
