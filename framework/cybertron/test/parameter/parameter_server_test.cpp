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

#include "cybertron/message/protobuf_factory.h"
#include "cybertron/cybertron.h"
#include "cybertron/parameter/parameter_server.h"

namespace apollo {
namespace cybertron {

using apollo::cybertron::proto::Param;
using apollo::cybertron::proto::ParamType;

class ParameterServerTest : public ::testing::Test {
 protected:
  ParameterServerTest() {}
  virtual ~ParameterServerTest() {}

  std::shared_ptr<Node> node_;
  std::shared_ptr<ParameterServer> ps_;

  virtual void SetUp() {
    // Called before every TEST_F(ParameterServerTest, *)
    node_ = CreateNode("parameter_server");
    ps_.reset(new ParameterServer(node_));
  }

  virtual void TearDown() {
    // Called after every TEST_F(ParameterServerTest, *)
  }
};

TEST_F(ParameterServerTest, set_parameter) {
  ps_->SetParameter(Parameter("int", 1));
  EXPECT_EQ("int", ps_->param_map_["int"].name());
  EXPECT_EQ(1, ps_->param_map_["int"].int_value());
}

TEST_F(ParameterServerTest, get_parameter) {
  ps_->SetParameter(Parameter("int", 1));
  Parameter parameter;
  ps_->GetParameter("int", &parameter);
  EXPECT_EQ("int", parameter.Name());
  EXPECT_EQ(1, parameter.AsInt64());
}

TEST_F(ParameterServerTest, list_parameter) {
  ps_->SetParameter(Parameter("int", 1));
  std::vector<Parameter> parameters;
  ps_->ListParameters(&parameters);
  EXPECT_EQ(1, parameters.size());
  EXPECT_EQ("int", parameters[0].Name());
  EXPECT_EQ(1, parameters[0].AsInt64());
}

}  // namespace cybertron
}  // namespace apollo

int main(int argc, char** argv) {
  apollo::cybertron::Init(argv[0]);
  testing::InitGoogleTest(&argc, argv);
  // google::InitGoogleLogging(argv[0]);
  return RUN_ALL_TESTS();
}
