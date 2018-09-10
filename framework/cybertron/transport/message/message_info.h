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

#ifndef CYBERTRON_TRANSPORT_MESSAGE_MESSAGE_INFO_H_
#define CYBERTRON_TRANSPORT_MESSAGE_MESSAGE_INFO_H_

#include <cstdint>
#include <string>

#include "cybertron/transport/common/identity.h"

namespace apollo {
namespace cybertron {
namespace transport {

class MessageInfo {
 public:
  MessageInfo();
  MessageInfo(const Identity& sender_id, uint64_t seq_num);
  MessageInfo(const Identity& sender_id, uint64_t seq_num,
              const Identity& spare_id);
  MessageInfo(const MessageInfo& another);
  virtual ~MessageInfo();

  MessageInfo& operator=(const MessageInfo& another);
  bool operator==(const MessageInfo& another) const;

  bool SerializeTo(std::string* dst) const;
  bool DeserializeFrom(const std::string& src);

  // getter and setter
  const Identity& sender_id() const { return sender_id_; }
  void set_sender_id(const Identity& sender_id) { sender_id_ = sender_id; }

  uint64_t seq_num() const { return seq_num_; }
  void set_seq_num(uint64_t seq_num) { seq_num_ = seq_num; }

  const Identity& spare_id() const { return spare_id_; }
  void set_spare_id(const Identity& spare_id) { spare_id_ = spare_id; }

 private:
  Identity sender_id_;
  uint64_t seq_num_;
  Identity spare_id_;
};

}  // namespace transport
}  // namespace cybertron
}  // namespace apollo

#endif  // CYBERTRON_TRANSPORT_MESSAGE_MESSAGE_INFO_H_
