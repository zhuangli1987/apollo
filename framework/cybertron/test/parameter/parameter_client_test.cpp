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
#include "cybertron/parameter/parameter_client.h"
#include "cybertron/parameter/parameter_server.h"

namespace apollo {
namespace cybertron {

using apollo::cybertron::proto::Param;
using apollo::cybertron::proto::ParamType;

class ParameterClientTest : public ::testing::Test {
 protected:
  ParameterClientTest() {}
  virtual ~ParameterClientTest() {}

  std::shared_ptr<Node> node_;
  std::shared_ptr<ParameterServer> ps_;
  std::shared_ptr<ParameterClient> pc_;

  virtual void SetUp() {
    // Called before every TEST_F(ParameterClientTest, *)
    node_ = CreateNode("parameter_server");
    ps_.reset(new ParameterServer(node_));
    pc_.reset(new ParameterClient(node_, "parameter_server"));
  }

  virtual void TearDown() {
    // Called after every TEST_F(ParameterClientTest, *)
    ps_.reset();
    pc_.reset();
    node_.reset();
  }
};

TEST_F(ParameterClientTest, set_parameter) {
  EXPECT_TRUE(pc_->SetParameter(Parameter("int", 1)));
  usleep(100000);
  EXPECT_EQ("int", ps_->param_map_["int"].name());
  EXPECT_EQ(1, ps_->param_map_["int"].int_value());

  ps_.reset();
  EXPECT_FALSE(pc_->SetParameter(Parameter("int", 1)));
}

TEST_F(ParameterClientTest, get_parameter) {
  ps_->SetParameter(Parameter("int", 1));
  Parameter parameter;
  EXPECT_TRUE(pc_->GetParameter("int", &parameter));
  EXPECT_EQ("int", parameter.Name());
  EXPECT_EQ(1, parameter.AsInt64());
  EXPECT_FALSE(pc_->GetParameter("double", &parameter));

  ps_.reset();
  EXPECT_FALSE(pc_->GetParameter("int", &parameter));
}

TEST_F(ParameterClientTest, list_parameter) {
  ps_->SetParameter(Parameter("int", 1));
  std::vector<Parameter> parameters;
  EXPECT_TRUE(pc_->ListParameters(&parameters));
  EXPECT_EQ(1, parameters.size());
  EXPECT_EQ("int", parameters[0].Name());
  EXPECT_EQ(1, parameters[0].AsInt64());

  ps_.reset();
  EXPECT_FALSE(pc_->ListParameters(&parameters));
}

}  // namespace cybertron
}  // namespace apollo

int main(int argc, char** argv) {
  apollo::cybertron::Init(argv[0]);
  testing::InitGoogleTest(&argc, argv);
  // google::InitGoogleLogging(argv[0]);
  return RUN_ALL_TESTS();
}
