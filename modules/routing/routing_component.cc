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

#include "modules/routing/routing_component.h"

#include "modules/common/adapters/adapter_gflags.h"
#include "modules/routing/common/routing_gflags.h"

DECLARE_string(flagfile);

namespace apollo {
namespace routing {

bool RoutingComponent::Init() {
  AINFO << "Loading gflag from file: " << ConfigFilePath();
  google::SetCommandLineOption("flagfile", ConfigFilePath().c_str());

  response_writer_ =
      node_->CreateWriter<RoutingResponse>(FLAGS_routing_response_topic);

  return routing_.Init().ok() && routing_.Start().ok();
}

bool RoutingComponent::Proc(const std::shared_ptr<RoutingRequest>& request) {
  auto response = std::make_shared<RoutingResponse>();
  if (!routing_.Process(request, response.get())) {
    return false;
  }
  common::util::FillHeader(node_->Name(), response.get());

  response_writer_->Write(response);
  return true;
}

}  // namespace routing
}  // namespace apollo

