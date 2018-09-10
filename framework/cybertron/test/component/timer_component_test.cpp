/******************************************************************************
 * Copyright 2017 The Apollo Authors. All Rights Reserved.
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

#include "cybertron/component/timer_component.h"
#include "cybertron/init.h"

namespace apollo {
namespace cybertron {

static bool ret_proc = true;
static bool ret_init = true;

class Component_Timer : public TimerComponent {
 public:
  Component_Timer() {}
  bool Init() { return ret_init; }
  bool Proc() { return ret_proc; }
};

TEST(TimerComponent, timertest) {
  ret_proc = true;
  ret_init = true;
  apollo::cybertron::proto::TimerComponentConfig compcfg;
  compcfg.set_name("driver");
  compcfg.set_interval(100);

  std::shared_ptr<Component_Timer> com = std::make_shared<Component_Timer>();
  EXPECT_EQ(true, com->Initialize(compcfg));
  EXPECT_EQ(true, com->Process());
  EXPECT_TRUE(com->ConfigFilePath().empty());
}

TEST(TimerComponentFalse, timerfail) {
  ret_proc = false;
  ret_init = false;
  apollo::cybertron::proto::TimerComponentConfig compcfg;
  compcfg.set_name("driver1");
  compcfg.set_interval(100);

  std::shared_ptr<Component_Timer> com = std::make_shared<Component_Timer>();
  EXPECT_EQ(false, com->Initialize(compcfg));
  EXPECT_EQ(false, com->Process());
  EXPECT_TRUE(com->ConfigFilePath().empty());
}
}  // namespace cybertron
}  // namespace apollo

int main(int argc, char** argv) {
  apollo::cybertron::Init(argv[0]);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
