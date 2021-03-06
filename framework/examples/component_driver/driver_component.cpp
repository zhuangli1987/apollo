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
#include "examples/component_driver/driver_component.h"

#include <chrono>

#include "cybertron/common/log.h"
#include "cybertron/time/time.h"

using apollo::cybertron::Time;
using apollo::cybertron::proto::RoleAttributes;

static std::shared_ptr<Writer<CarStatus>> carstatus_writer_ = nullptr;
DriverComponent::DriverComponent() {}

bool DriverComponent::Init() {
  RoleAttributes attr;
  attr.set_channel_name("/driver/channel");
  driver_writer_ = node_->CreateWriter<Driver>(attr);
  if (driver_writer_ == nullptr) {
    AERROR << "Driver create writer failed.";
    return false;
  }

  attr.set_channel_name("/carstatus/channel");
  carstatus_writer_ = node_->CreateWriter<CarStatus>(attr);
  if (carstatus_writer_ == nullptr) {
    AERROR << "Driver create writer failed.";
    return false;
  }

  attr.set_channel_name("/food");
  dog_writer_ = node_->CreateWriter<RawMessage>(attr);
  if (dog_writer_ == nullptr) {
    AERROR << "Driver create writer failed.";
    return false;
  }

  return true;
}

bool DriverComponent::Proc() {
  static int i = 0;
  auto out_msg = std::make_shared<Driver>();
  out_msg->set_msg_id(i++);
  auto t = std::chrono::steady_clock::now();
  out_msg->set_timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
                             t.time_since_epoch())
                             .count());
  out_msg->mutable_header()->set_timestamp(Time::Now().ToNanosecond());
  driver_writer_->Write(out_msg);

  for (int n = 0; n < 10; ++n) {
    static int j = 0;
    auto car_msg = std::make_shared<CarStatus>();
    car_msg->mutable_header()->set_timestamp(Time::Now().ToNanosecond());
    car_msg->set_msg_id(j++);
    car_msg->set_result(0);
  }

  auto food = std::make_shared<RawMessage>("ham sausage");
  dog_writer_->Write(food);

  return true;
}
