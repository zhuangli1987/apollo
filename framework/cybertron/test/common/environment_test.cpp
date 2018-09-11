/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include "gtest/gtest.h"

#include <stdlib.h>

#include "cybertron/common/environment.h"

namespace apollo {
namespace cybertron {
namespace common {

TEST(EnvironmentTest, get_env) {
  unsetenv("EnvironmentTest_get_env");
  std::string env_value = GetEnv("EnvironmentTest_get_env");
  EXPECT_EQ(env_value, "");
  setenv("EnvironmentTest_get_env", "123456789", 1);
  env_value = GetEnv("EnvironmentTest_get_env");
  EXPECT_EQ(env_value, "123456789");
  unsetenv("EnvironmentTest_get_env");
}

TEST(EnvironmentTest, work_root) {
  std::string before = WorkRoot();
  unsetenv("CYBERTRON_PATH");
  std::string after = GetEnv("CYBERTRON_PATH");
  EXPECT_EQ(after, "");
  setenv("CYBERTRON_PATH", before.c_str(), 1);
  after = WorkRoot();
  EXPECT_EQ(after, before);
}

TEST(EnvironmentTest, module_root) {
  std::string before = GetEnv("MODULE_PATH");
  unsetenv("MODULE_PATH");
  EXPECT_EQ(ModuleRoot(), WorkRoot());

  setenv("MODULE_PATH", "/home/caros", 1);
  EXPECT_EQ(ModuleRoot(), "/home/caros/");
  setenv("MODULE_PATH", "/home/caros/", 1);
  EXPECT_EQ(ModuleRoot(), "/home/caros/");

  setenv("MODULE_PATH", before.c_str(), 1);
}

}  // namespace common
}  // namespace cybertron
}  // namespace apollo

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
