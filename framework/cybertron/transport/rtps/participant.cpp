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

#include "cybertron/transport/rtps/participant.h"

#include "cybertron/common/global_data.h"
#include "cybertron/common/log.h"
#include "cybertron/proto/transport_conf.pb.h"

namespace apollo {
namespace cybertron {
namespace transport {

Participant::Participant(const std::string& name, int send_port,
                         eprosima::fastrtps::ParticipantListener* listener)
    : shutdown_(false), fastrtps_participant_(nullptr) {
  CreateFastRtpsParticipant(name, send_port, listener);
}

Participant::~Participant() {}

void Participant::Shutdown() {
  if (shutdown_) {
    return;
  }
  shutdown_ = true;

  RETURN_IF_NULL(fastrtps_participant_);
  eprosima::fastrtps::Domain::removeParticipant(fastrtps_participant_);
  fastrtps_participant_ = nullptr;
}

void Participant::CreateFastRtpsParticipant(
    const std::string& name, int send_port,
    eprosima::fastrtps::ParticipantListener* listener) {
  uint32_t domain_id = 80;
  const char* val = ::getenv("CYBER_DOMAIN_ID");
  if (val != NULL) {
    try {
      domain_id = std::stoi(val);
    } catch (const std::exception& e) {
      AERROR << "convert domain_id error " << e.what();
      return;
    }
  }

  auto part_attr_conf = std::make_shared<proto::RtpsParticipantAttr>();
  auto global_conf = common::GlobalData::Instance()->Config();
  if (global_conf.has_transport_conf() &&
      global_conf.transport_conf().has_participant_attr()) {
    part_attr_conf->CopyFrom(global_conf.transport_conf().participant_attr());
  } else {
    AERROR << "No rtps participant attr conf.";
    return;
  }

  eprosima::fastrtps::ParticipantAttributes attr;
  attr.rtps.defaultSendPort = send_port;
  attr.rtps.port.domainIDGain = part_attr_conf->domain_id_gain();
  attr.rtps.port.portBase = part_attr_conf->port_base();
  attr.rtps.use_IP6_to_send = false;
  attr.rtps.builtin.use_SIMPLE_RTPSParticipantDiscoveryProtocol = true;
  attr.rtps.builtin.use_SIMPLE_EndpointDiscoveryProtocol = true;
  attr.rtps.builtin.m_simpleEDP.use_PublicationReaderANDSubscriptionWriter =
      true;
  attr.rtps.builtin.m_simpleEDP.use_PublicationWriterANDSubscriptionReader =
      true;
  attr.rtps.builtin.domainId = domain_id;
  attr.rtps.builtin.leaseDuration.seconds = part_attr_conf->lease_duration();
  attr.rtps.builtin.leaseDuration_announcementperiod.seconds =
      part_attr_conf->announcement_period();

  attr.rtps.setName(name.c_str());

  std::string ip_env("127.0.0.1");
  const char* ip_val = ::getenv("CYBER_IP");
  if (ip_val != nullptr) {
    ip_env = ip_val;
    if (ip_env.size() == 0) {
      AERROR << "invalid CYBER_IP (an empty string)";
      return;
    }
  }
  ADEBUG << "cyber ip: " << ip_env;

  eprosima::fastrtps::rtps::Locator_t locator;
  locator.port = 0;
  RETURN_IF(!locator.set_IP4_address(ip_env));

  locator.kind = LOCATOR_KIND_UDPv4;

  attr.rtps.defaultUnicastLocatorList.push_back(locator);
  attr.rtps.defaultOutLocatorList.push_back(locator);
  attr.rtps.builtin.metatrafficUnicastLocatorList.push_back(locator);

  locator.set_IP4_address(239, 255, 0, 1);
  attr.rtps.builtin.metatrafficMulticastLocatorList.push_back(locator);

  fastrtps_participant_ =
      eprosima::fastrtps::Domain::createParticipant(attr, listener);
  RETURN_IF_NULL(fastrtps_participant_);
  eprosima::fastrtps::Domain::registerType(fastrtps_participant_, &type_);
}

}  // namespace transport
}  // namespace cybertron
}  // namespace apollo
