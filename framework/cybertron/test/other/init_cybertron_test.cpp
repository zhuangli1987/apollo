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

#include <thread>

#include "cybertron/common/log.h"
#include "cybertron/cybertron.h"
#include "cybertron/init.h"
#include "cybertron/proto/driver.pb.h"
using apollo::cybertron::proto::CarStatus;

namespace apollo {
namespace cybertron {

void VoidTask() { AINFO << "VoidTask running"; }

int UserTask(const std::shared_ptr<CarStatus>& msg) {
  AINFO << "receive msg";
  return 0;
}

TEST(InitCybertronTest, create_node) {
  auto node = CreateNode("create_node");
  EXPECT_EQ(node, nullptr);
}

TEST(InitCybertronTest, all_in_one) {
  EXPECT_FALSE(OK());
  EXPECT_FALSE(IsShutdown());

  EXPECT_TRUE(Init("init"));
  EXPECT_TRUE(OK());
  // repeated call
  EXPECT_TRUE(Init());

  auto node = CreateNode("create_node");
  EXPECT_NE(node, nullptr);

  PrintSchedulerStatistics();

  std::thread wait_th([]() { WaitForShutdown(); });

  Shutdown();
  if (wait_th.joinable()) {
    wait_th.join();
  }

  EXPECT_TRUE(IsShutdown());
  EXPECT_FALSE(OK());
  EXPECT_FALSE(Init());
  // repeated call
  Shutdown();
}

}  // namespace cybertron
}  // namespace apollo

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
