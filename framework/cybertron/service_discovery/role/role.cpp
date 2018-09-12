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

#include "cybertron/service_discovery/role/role.h"

#include "cybertron/common/log.h"

namespace apollo {
namespace cybertron {
namespace service_discovery {

RoleBase::RoleBase() : timestamp_(0) {}

RoleBase::RoleBase(const RoleAttributes& attr, uint64_t timestamp)
    : attributes_(attr), timestamp_(timestamp) {}

RoleBase::~RoleBase() {}

bool RoleBase::Match(const RoleAttributes& target_attr) const {
  if (target_attr.has_node_id() &&
      target_attr.node_id() != attributes_.node_id()) {
    return false;
  }

  if (target_attr.has_process_id() &&
      target_attr.process_id() != attributes_.process_id()) {
    return false;
  }

  if (target_attr.has_host_name() &&
      target_attr.host_name() != attributes_.host_name()) {
    return false;
  }

  return true;
}

bool RoleBase::IsEarlierThan(const RoleBase& other) const {
  return timestamp_ < other.timestamp();
}

RoleWriter::RoleWriter() {}

RoleWriter::RoleWriter(const RoleAttributes& attr, uint64_t timestamp)
    : RoleBase(attr, timestamp) {}

RoleWriter::~RoleWriter() {}

bool RoleWriter::Match(const RoleAttributes& target_attr) const {
  if (target_attr.has_channel_id() &&
      target_attr.channel_id() != attributes_.channel_id()) {
    return false;
  }

  if (target_attr.has_id() && target_attr.id() != attributes_.id()) {
    return false;
  }

  return RoleBase::Match(target_attr);
}

RoleServer::RoleServer() {}

RoleServer::RoleServer(const RoleAttributes& attr, uint64_t timestamp)
    : RoleBase(attr, timestamp) {}

RoleServer::~RoleServer() {}

bool RoleServer::Match(const RoleAttributes& target_attr) const {
  if (target_attr.has_service_id() &&
      target_attr.service_id() != attributes_.service_id()) {
    return false;
  }

  return RoleBase::Match(target_attr);
}

}  // namespace service_discovery
}  // namespace cybertron
}  // namespace apollo
