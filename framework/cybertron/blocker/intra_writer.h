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

#ifndef CYBERTRON_BLOCKER_INTRA_WRITER_H_
#define CYBERTRON_BLOCKER_INTRA_WRITER_H_

#include <memory>

#include "cybertron/blocker/blocker_manager.h"
#include "cybertron/node/writer.h"

namespace apollo {
namespace cybertron {
namespace blocker {

template <typename MessageT>
class IntraWriter : public apollo::cybertron::Writer<MessageT> {
 public:
  using MessagePtr = std::shared_ptr<MessageT>;
  using BlockerManagerPtr = std::shared_ptr<BlockerManager>;

  explicit IntraWriter(const proto::RoleAttributes& attr);
  virtual ~IntraWriter();

  bool Init() override;
  void Shutdown() override;

  bool Write(const MessageT& msg) override;
  bool Write(const MessagePtr& msg_ptr) override;

 private:
  BlockerManagerPtr blocker_manager_;
};

template <typename MessageT>
IntraWriter<MessageT>::IntraWriter(const proto::RoleAttributes& attr)
    : Writer<MessageT>(attr) {}

template <typename MessageT>
IntraWriter<MessageT>::~IntraWriter() {
  Shutdown();
}

template <typename MessageT>
bool IntraWriter<MessageT>::Init() {
  if (this->init_.exchange(true)) {
    return true;
  }

  blocker_manager_ = BlockerManager::Instance();
  blocker_manager_->GetOrCreateBlocker<MessageT>(
      BlockerAttr(this->role_attr_.channel_name()));
  return true;
}

template <typename MessageT>
void IntraWriter<MessageT>::Shutdown() {
  if (!this->init_.exchange(false)) {
    return;
  }
  blocker_manager_ = nullptr;
}

template <typename MessageT>
bool IntraWriter<MessageT>::Write(const MessageT& msg) {
  if (!this->init_.load()) {
    return false;
  }
  return blocker_manager_->Publish<MessageT>(this->role_attr_.channel_name(),
                                             msg);
}

template <typename MessageT>
bool IntraWriter<MessageT>::Write(const MessagePtr& msg_ptr) {
  if (!this->init_.load()) {
    return false;
  }
  return blocker_manager_->Publish<MessageT>(this->role_attr_.channel_name(),
                                             msg_ptr);
}

}  // namespace blocker
}  // namespace cybertron
}  // namespace apollo

#endif  // CYBERTRON_BLOCKER_INTRA_WRITER_H_
