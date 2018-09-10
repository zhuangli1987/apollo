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

#ifndef CYBERTRON_MESSAGE_INTRA_MESSAGE_TRAITS_H_
#define CYBERTRON_MESSAGE_INTRA_MESSAGE_TRAITS_H_

#include <cassert>
#include <memory>
#include <string>

#include "cybertron/message/intra_message.h"

namespace apollo {
namespace cybertron {
namespace message {

template <typename MessageT,
          typename std::enable_if<
              std::is_base_of<IntraMessage, MessageT>::value,
              int>::type = 0>
inline bool SerializeToString(const MessageT& message, std::string* str) {
  return message.SerializeToString(str);
}

template <typename MessageT,
          typename std::enable_if<
              std::is_base_of<IntraMessage, MessageT>::value,
              int>::type = 0>
inline bool ParseFromString(const std::string& str, MessageT* message) {
  return message->ParseFromString(str);
}

template <typename MessageT,
          typename std::enable_if<
              std::is_base_of<IntraMessage, MessageT>::value,
              int>::type = 0>
inline std::string MessageType() {
  return MessageT::TypeName();
}

template <typename MessageT,
          typename std::enable_if<
              std::is_base_of<IntraMessage, MessageT>::value,
              int>::type = 0>
std::string MessageType(const MessageT& message) {
  return message.TypeName();
}

template <typename MessageT,
          typename std::enable_if<
              std::is_base_of<IntraMessage, MessageT>::value,
              int>::type = 0>
inline void GetDescriptorString(const std::string& type,
                                std::string* desc_str) {
  * desc_str = IntraMessage::GetDescriptorString();
}

}  // namespace message
}  // namespace cybertron
}  // namespace apollo

#endif  // CYBERTRON_MESSAGE_INTRA_MESSAGE_TRAITS_H_
