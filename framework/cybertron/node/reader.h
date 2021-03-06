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

#ifndef CYBERTRON_NODE_READER_H_
#define CYBERTRON_NODE_READER_H_

#include <algorithm>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "cybertron/common/global_data.h"
#include "cybertron/croutine/routine_factory.h"
#include "cybertron/data/data_visitor.h"
#include "cybertron/node/reader_base.h"
#include "cybertron/proto/topology_change.pb.h"
#include "cybertron/scheduler/scheduler.h"
#include "cybertron/service_discovery/topology_manager.h"
#include "cybertron/time/time.h"
#include "cybertron/transport/transport.h"

namespace apollo {
namespace cybertron {

template <typename M0>
using CallbackFunc = std::function<void(const std::shared_ptr<M0>&)>;

using proto::RoleType;

template <typename MessageT>
class Reader : public ReaderBase {
 public:
  using ReceiverPtr = std::shared_ptr<transport::Receiver<MessageT>>;
  using ChangeConnection =
      typename service_discovery::Manager::ChangeConnection;
  using Iterator =
      typename std::list<std::shared_ptr<MessageT>>::const_iterator;

  Reader(const proto::RoleAttributes& role_attr,
         const CallbackFunc<MessageT>& reader_func = nullptr);
  virtual ~Reader();

  bool Init() override;
  void Shutdown() override;
  void Observe() override;
  void ClearData() override;
  bool HasReceived() const override;
  bool Empty() const override;
  double GetDelaySec() const override;

  virtual void Enqueue(const std::shared_ptr<MessageT>& msg);
  virtual void SetHistoryDepth(const uint32_t& depth);
  virtual uint32_t GetHistoryDepth() const;
  virtual std::shared_ptr<MessageT> GetLatestObserved() const;
  virtual std::shared_ptr<MessageT> GetOldestObserved() const;
  virtual Iterator Begin() const { return observed_queue_.begin(); }
  virtual Iterator End() const { return observed_queue_.end(); }

 protected:
  double latest_recv_time_sec_;
  double second_to_lastest_recv_time_sec_;

 private:
  void JoinTheTopology();
  void LeaveTheTopology();
  void OnChannelChange(const proto::ChangeMsg& change_msg);

  CallbackFunc<MessageT> reader_func_;
  ReceiverPtr receiver_;
  std::string croutine_name_;

  ChangeConnection change_conn_;
  service_discovery::ChannelManagerPtr channel_manager_;

  mutable std::mutex mutex_;
  uint32_t history_depth_ = 10;
  std::list<std::shared_ptr<MessageT>> history_queue_;
  std::list<std::shared_ptr<MessageT>> observed_queue_;
};

template <typename MessageT>
Reader<MessageT>::Reader(const proto::RoleAttributes& role_attr,
                         const CallbackFunc<MessageT>& reader_func)
    : ReaderBase(role_attr),
      latest_recv_time_sec_(-1.0),
      second_to_lastest_recv_time_sec_(-1.0),
      reader_func_(reader_func),
      receiver_(nullptr),
      croutine_name_(""),
      channel_manager_(nullptr) {}

template <typename MessageT>
Reader<MessageT>::~Reader() {
  Shutdown();
}

template <typename MessageT>
void Reader<MessageT>::Enqueue(const std::shared_ptr<MessageT>& msg) {
  second_to_lastest_recv_time_sec_ = latest_recv_time_sec_;
  latest_recv_time_sec_ = Time::Now().ToSecond();
  std::lock_guard<std::mutex> lg(mutex_);
  history_queue_.push_front(msg);
  while (history_queue_.size() > history_depth_) {
    history_queue_.pop_back();
  }
}

template <typename MessageT>
void Reader<MessageT>::Observe() {
  std::lock_guard<std::mutex> lg(mutex_);
  observed_queue_ = history_queue_;
}

template <typename MessageT>
bool Reader<MessageT>::Init() {
  if (init_.exchange(true)) {
    return true;
  }

  std::function<void(const std::shared_ptr<MessageT>&)> func;
  if (reader_func_ != nullptr) {
    func = [this](const std::shared_ptr<MessageT>& msg) {
      this->Enqueue(msg);
      this->reader_func_(msg);
    };
  } else {
    func = [this](const std::shared_ptr<MessageT>& msg) { this->Enqueue(msg); };
  }
  auto sched = scheduler::Scheduler::Instance();
  croutine_name_ = role_attr_.node_name() + "_" + role_attr_.channel_name();
  auto dv = std::make_shared<data::DataVisitor<MessageT>>(
      role_attr_.channel_id(), role_attr_.qos_profile().depth());
  // Using factory to wrap templates.
  croutine::RoutineFactory factory =
      croutine::CreateRoutineFactory<MessageT>(std::move(func), dv);
  if (!sched->CreateTask(factory, croutine_name_)) {
    AERROR << "Create Failed!";
    init_.exchange(false);
    return false;
  }

  receiver_ = ReceiverManager<MessageT>::Instance()->GetReceiver(role_attr_);
  this->role_attr_.set_id(receiver_->id().HashValue());

  channel_manager_ =
      service_discovery::TopologyManager::Instance()->channel_manager();
  JoinTheTopology();

  // TODO more check
  return true;
}

template <typename MessageT>
void Reader<MessageT>::Shutdown() {
  // TODO: delete task and reset transport reader
  if (!init_.exchange(false)) {
    return;
  }
  LeaveTheTopology();
  receiver_ = nullptr;
  channel_manager_ = nullptr;

  if (!croutine_name_.empty()) {
    scheduler::Scheduler::Instance()->RemoveTask(croutine_name_);
  }
}

template <typename MessageT>
void Reader<MessageT>::JoinTheTopology() {
  // add listener
  change_conn_ = channel_manager_->AddChangeListener(std::bind(
      &Reader<MessageT>::OnChannelChange, this, std::placeholders::_1));

  // get peer writers
  const std::string& channel_name = this->role_attr_.channel_name();
  std::vector<proto::RoleAttributes> writers;
  channel_manager_->GetWritersOfChannel(channel_name, &writers);
  for (auto& writer : writers) {
    receiver_->Enable(writer);
  }
  channel_manager_->Join(this->role_attr_, proto::RoleType::ROLE_READER);
}

template <typename MessageT>
void Reader<MessageT>::LeaveTheTopology() {
  channel_manager_->RemoveChangeListener(change_conn_);
  channel_manager_->Leave(this->role_attr_, proto::RoleType::ROLE_READER);
}

template <typename MessageT>
void Reader<MessageT>::OnChannelChange(const proto::ChangeMsg& change_msg) {
  if (change_msg.role_type() != proto::RoleType::ROLE_WRITER) {
    return;
  }

  auto& writer_attr = change_msg.role_attr();
  if (writer_attr.channel_name() != this->role_attr_.channel_name()) {
    return;
  }

  auto operate_type = change_msg.operate_type();
  if (operate_type == proto::OperateType::OPT_JOIN) {
    receiver_->Enable(writer_attr);
  } else {
    receiver_->Disable(writer_attr);
  }
}

template <typename MessageT>
bool Reader<MessageT>::HasReceived() const {
  std::lock_guard<std::mutex> lg(mutex_);
  return !history_queue_.empty();
}

template <typename MessageT>
bool Reader<MessageT>::Empty() const {
  std::lock_guard<std::mutex> lg(mutex_);
  return observed_queue_.empty();
}

template <typename MessageT>
double Reader<MessageT>::GetDelaySec() const {
  if (latest_recv_time_sec_ < 0) {
    return -1.0;
  }
  if (second_to_lastest_recv_time_sec_ < 0) {
    return Time::Now().ToSecond() - latest_recv_time_sec_;
  }
  return std::max((Time::Now().ToSecond() - latest_recv_time_sec_),
                  (latest_recv_time_sec_ - second_to_lastest_recv_time_sec_));
}

template <typename MessageT>
std::shared_ptr<MessageT> Reader<MessageT>::GetLatestObserved() const {
  std::lock_guard<std::mutex> lg(mutex_);
  if (observed_queue_.empty()) {
    return nullptr;
  }
  return observed_queue_.front();
}

template <typename MessageT>
std::shared_ptr<MessageT> Reader<MessageT>::GetOldestObserved() const {
  std::lock_guard<std::mutex> lg(mutex_);
  if (observed_queue_.empty()) {
    return nullptr;
  }
  return observed_queue_.back();
}

template <typename MessageT>
void Reader<MessageT>::ClearData() {
  std::lock_guard<std::mutex> lg(mutex_);
  history_queue_.clear();
  observed_queue_.clear();
}

template <typename MessageT>
void Reader<MessageT>::SetHistoryDepth(const uint32_t& depth) {
  std::lock_guard<std::mutex> lg(mutex_);
  history_depth_ = depth;
  while (history_queue_.size() > history_depth_) {
    history_queue_.pop_back();
  }
}

template <typename MessageT>
uint32_t Reader<MessageT>::GetHistoryDepth() const {
  return history_depth_;
}

}  // namespace cybertron
}  // namespace apollo

#endif  // CYBERTRON_NODE_READER_H_
