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

#ifndef CYBERTRON_NODE_READER_BASE_H_
#define CYBERTRON_NODE_READER_BASE_H_

#include <atomic>
#include <memory>
#include <string>
#include <unordered_map>

#include "cybertron/common/macros.h"
#include "cybertron/common/util.h"
#include "cybertron/event/perf_event_cache.h"
#include "cybertron/transport/transport.h"

namespace apollo {
namespace cybertron {

using apollo::cybertron::common::GlobalData;
using apollo::cybertron::event::PerfEventCache;
using apollo::cybertron::event::TransPerf;

class ReaderBase {
 public:
  explicit ReaderBase(const proto::RoleAttributes& role_attr)
      : role_attr_(role_attr), init_(false) {}
  virtual ~ReaderBase() {}

  virtual bool Init() = 0;
  virtual void Shutdown() = 0;
  virtual void ClearData() = 0;
  virtual void Observe() = 0;
  virtual bool Empty() const = 0;
  virtual bool HasReceived() const = 0;
  virtual double GetDelaySec() const = 0;

  const std::string& GetChannelName() const {
    return role_attr_.channel_name();
  }

  const uint64_t ChannelId() const { return role_attr_.channel_id(); }

  const proto::QosProfile& QosProfile() const {
    return role_attr_.qos_profile();
  }

  bool inited() const { return init_.load(); }

 protected:
  proto::RoleAttributes role_attr_;
  std::atomic<bool> init_;
};

template <typename MessageT>
class ReceiverManager {
 public:
  ~ReceiverManager() { receiver_map_.clear(); }

  auto GetReceiver(const proto::RoleAttributes& role_attr) ->
      typename std::shared_ptr<transport::Receiver<MessageT>>;

 private:
  std::unordered_map<std::string,
                     typename std::shared_ptr<transport::Receiver<MessageT>>>
      receiver_map_;
  std::mutex receiver_map_mutex_;

  DECLARE_SINGLETON(ReceiverManager<MessageT>)
};

template <typename MessageT>
ReceiverManager<MessageT>::ReceiverManager() {}

template <typename MessageT>
auto ReceiverManager<MessageT>::GetReceiver(
    const proto::RoleAttributes& role_attr) ->
    typename std::shared_ptr<transport::Receiver<MessageT>> {
  std::lock_guard<std::mutex> lock(receiver_map_mutex_);
  // because multi reader for one channel will write datacache multi times,
  // so reader for datacache we use map to keep one instance for per channel
  const std::string& channel_name = role_attr.channel_name();
  if (receiver_map_.count(channel_name) == 0) {
    receiver_map_[channel_name] =
        transport::Transport::CreateReceiver<MessageT>(
            role_attr, [](const std::shared_ptr<MessageT>& msg,
                          const transport::MessageInfo& msg_info,
                          const proto::RoleAttributes& reader_attr) {
              (void)msg_info;
              (void)reader_attr;
              PerfEventCache::Instance()->AddTransportEvent(
                  TransPerf::TRANS_TO, reader_attr.channel_id(),
                  msg_info.seq_num());
              data::DataDispatcher<MessageT>::Instance()->Dispatch(
                  reader_attr.channel_id(), msg);
              PerfEventCache::Instance()->AddTransportEvent(
                  TransPerf::WRITE_NOTIFY, reader_attr.channel_id(),
                  msg_info.seq_num());
            });
  }
  return receiver_map_[channel_name];
}

}  // namespace cybertron
}  // namespace apollo

#endif  // CYBERTRON_NODE_READER_BASE_H_
