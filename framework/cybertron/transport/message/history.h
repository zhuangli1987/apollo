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

#ifndef CYBERTRON_TRANSPORT_MESSAGE_HISTORY_H_
#define CYBERTRON_TRANSPORT_MESSAGE_HISTORY_H_

#include <cstdint>
#include <list>
#include <memory>
#include <mutex>
#include <vector>

#include "cybertron/common/global_data.h"
#include "cybertron/transport/message/history_attributes.h"
#include "cybertron/transport/message/message_info.h"

namespace apollo {
namespace cybertron {
namespace transport {

template <typename MessageT>
class History {
 public:
  using MessagePtr = std::shared_ptr<MessageT>;
  struct CachedMessage {
    CachedMessage(const MessagePtr& message, const MessageInfo& message_info)
        : msg(message), msg_info(message_info) {}

    MessagePtr msg;
    MessageInfo msg_info;
  };

  explicit History(const HistoryAttributes& attr);
  virtual ~History();

  void Enable() { enabled_ = true; }
  void Disable() { enabled_ = false; }

  void Add(const MessagePtr& msg, const MessageInfo& msg_info);
  void Clear();
  void GetCachedMessage(std::vector<CachedMessage>* msgs);
  size_t GetSize();

  uint32_t depth() const { return depth_; }
  uint32_t max_depth() const { return max_depth_; }
  bool is_full() const { return is_full_; }

 private:
  bool enabled_;
  uint32_t depth_;
  uint32_t max_depth_;
  bool is_full_;
  std::list<CachedMessage> msgs_;
  std::mutex msgs_mutex_;
};

template <typename MessageT>
History<MessageT>::History(const HistoryAttributes& attr)
    : enabled_(false), max_depth_(1000), is_full_(false) {
  auto global_conf = common::GlobalData::Instance()->Config();
  if (global_conf.has_transport_conf() &&
      global_conf.transport_conf().has_resource_limit()) {
    max_depth_ =
        global_conf.transport_conf().resource_limit().max_history_depth();
  }

  if (attr.history_policy == proto::QosHistoryPolicy::HISTORY_KEEP_ALL) {
    depth_ = max_depth_;
  } else {
    depth_ = attr.depth;
    if (depth_ > max_depth_) {
      depth_ = max_depth_;
    }
  }
}

template <typename MessageT>
History<MessageT>::~History() {
  Clear();
}

template <typename MessageT>
void History<MessageT>::Add(const MessagePtr& msg,
                            const MessageInfo& msg_info) {
  if (!enabled_) {
    return;
  }
  std::lock_guard<std::mutex> lock(msgs_mutex_);
  if (is_full_) {
    msgs_.pop_front();
  }

  msgs_.emplace_back(msg, msg_info);

  if (!is_full_) {
    if (msgs_.size() == depth_) {
      is_full_ = true;
    }
  }
}

template <typename MessageT>
void History<MessageT>::Clear() {
  std::lock_guard<std::mutex> lock(msgs_mutex_);
  msgs_.clear();
}

template <typename MessageT>
void History<MessageT>::GetCachedMessage(std::vector<CachedMessage>* msgs) {
  if (msgs == nullptr) {
    return;
  }

  {
    std::lock_guard<std::mutex> lock(msgs_mutex_);
    msgs->reserve(msgs_.size());
    for (auto& item : msgs_) {
      msgs->emplace_back(item);
    }
  }
}

template <typename MessageT>
size_t History<MessageT>::GetSize() {
  std::lock_guard<std::mutex> lock(msgs_mutex_);
  return msgs_.size();
}

}  // namespace transport
}  // namespace cybertron
}  // namespace apollo

#endif  // CYBERTRON_TRANSPORT_MESSAGE_HISTORY_H_
