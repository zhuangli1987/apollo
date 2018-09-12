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

#include "cybertron/common/global_data.h"
#include "cybertron/service_discovery/specific_manager/service_manager.h"
#include "cybertron/transport/rtps/participant.h"

namespace apollo {
namespace cybertron {
namespace service_discovery {

class ServiceManagerTest : public ::testing::Test {
 protected:
  ServiceManagerTest() {
    std::string participant_name =
        common::GlobalData::Instance()->HostName() + "+" +
        std::to_string(common::GlobalData::Instance()->ProcessId());
    participant_ =
        std::make_shared<transport::Participant>(participant_name, 11511);
    service_manager_ = std::make_shared<ServiceManager>();
    service_manager_->Init(participant_->fastrtps_participant());
  }
  virtual ~ServiceManagerTest() {
    service_manager_->Shutdown();
    participant_->Shutdown();
  }

  virtual void SetUp() {}

  virtual void TearDown() {}

  std::shared_ptr<ServiceManager> service_manager_;
  transport::ParticipantPtr participant_;
};

TEST_F(ServiceManagerTest, server_operation) {
  // join
  RoleAttributes role_attr;
  EXPECT_FALSE(service_manager_->Join(role_attr, RoleType::ROLE_SERVER));
  EXPECT_FALSE(service_manager_->Join(role_attr, RoleType::ROLE_NODE));

  role_attr.set_host_name(common::GlobalData::Instance()->HostName());
  role_attr.set_process_id(common::GlobalData::Instance()->ProcessId());
  role_attr.set_node_name("node");
  uint64_t node_id = common::GlobalData::RegisterNode("node");
  role_attr.set_node_id(node_id);
  role_attr.set_service_name("service");
  uint64_t service_id = common::GlobalData::RegisterService("service");
  role_attr.set_service_id(service_id);
  EXPECT_TRUE(service_manager_->Join(role_attr, RoleType::ROLE_SERVER));
  EXPECT_FALSE(service_manager_->Join(role_attr, RoleType::ROLE_NODE));
  EXPECT_TRUE(service_manager_->HasService("service"));
  EXPECT_FALSE(service_manager_->HasService("client"));
  // repeated call
  EXPECT_TRUE(service_manager_->Join(role_attr, RoleType::ROLE_SERVER));

  // get servers
  std::vector<RoleAttributes> servers;
  EXPECT_TRUE(servers.empty());
  service_manager_->GetServers(&servers);
  EXPECT_EQ(servers.size(), 1);

  // leave
  EXPECT_TRUE(service_manager_->Leave(role_attr, RoleType::ROLE_SERVER));
  EXPECT_FALSE(service_manager_->HasService("service"));

  servers.clear();
  EXPECT_TRUE(servers.empty());
  service_manager_->GetServers(&servers);
  EXPECT_TRUE(servers.empty());
}

TEST_F(ServiceManagerTest, client_operation) {
  // join
  RoleAttributes role_attr;
  role_attr.set_host_name(common::GlobalData::Instance()->HostName());
  role_attr.set_process_id(common::GlobalData::Instance()->ProcessId());
  role_attr.set_node_name("node");
  uint64_t node_id = common::GlobalData::RegisterNode("node");
  role_attr.set_node_id(node_id);
  role_attr.set_service_name("service");
  uint64_t service_id = common::GlobalData::RegisterService("service");
  role_attr.set_service_id(service_id);
  EXPECT_TRUE(service_manager_->Join(role_attr, RoleType::ROLE_CLIENT));
  // repeated call
  EXPECT_TRUE(service_manager_->Join(role_attr, RoleType::ROLE_CLIENT));

  // get clients
  std::vector<RoleAttributes> clients;
  EXPECT_TRUE(clients.empty());
  service_manager_->GetClients("service", &clients);
  EXPECT_EQ(clients.size(), 2);

  // leave
  EXPECT_TRUE(service_manager_->Leave(role_attr, RoleType::ROLE_CLIENT));

  clients.clear();
  EXPECT_TRUE(clients.empty());
  service_manager_->GetClients("service", &clients);
  EXPECT_TRUE(clients.empty());
}

TEST_F(ServiceManagerTest, topo_module_leave) {
  RoleAttributes role_attr;
  role_attr.set_host_name(common::GlobalData::Instance()->HostName());
  role_attr.set_process_id(common::GlobalData::Instance()->ProcessId());
  role_attr.set_node_name("node");
  uint64_t node_id = common::GlobalData::RegisterNode("node");
  role_attr.set_node_id(node_id);
  role_attr.set_service_name("service");
  uint64_t service_id = common::GlobalData::RegisterService("service");
  role_attr.set_service_id(service_id);
  EXPECT_TRUE(service_manager_->Join(role_attr, RoleType::ROLE_SERVER));
  EXPECT_TRUE(service_manager_->Join(role_attr, RoleType::ROLE_CLIENT));

  EXPECT_TRUE(service_manager_->HasService("service"));

  service_manager_->OnTopoModuleLeave(role_attr.host_name(),
                                     role_attr.process_id());
  EXPECT_FALSE(service_manager_->HasService("service"));
}

}  // namespace service_discovery
}  // namespace cybertron
}  // namespace apollo

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
