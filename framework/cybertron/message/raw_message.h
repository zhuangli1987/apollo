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

#ifndef CYBERTRON_MESSAGE_RAW_MESSAGE_H_
#define CYBERTRON_MESSAGE_RAW_MESSAGE_H_

#include <cassert>
#include <memory>
#include <string>

namespace apollo {
namespace cybertron {
namespace message {

struct RawMessage {
 public:
  RawMessage() { message = ""; }

  explicit RawMessage(const std::string &data) { message = data; }

  ~RawMessage() {}

  bool SerializeToString(std::string *str) const {
    if (str == nullptr) {
      return false;
    }
    *str = message;
    return true;
  }

  bool ParseFromString(const std::string &str) {
    message = str;
    return true;
  }

  static std::string TypeName() {
    return "apollo.cybertron.message.RawMessage";
  }

  std::string message;
  uint64_t timestamp;

 private:
  RawMessage(const RawMessage &) = delete;
  RawMessage &operator=(const RawMessage &) = delete;
};

}  // namespace message
}  // namespace cybertron
}  // namespace apollo

#endif  // CYBERTRON_MESSAGE_RAW_MESSAGE_H_
