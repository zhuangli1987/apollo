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

#ifndef CYBERTRON_TRANSPORT_SHM_SEGMENT_H_
#define CYBERTRON_TRANSPORT_SHM_SEGMENT_H_

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "cybertron/transport/shm/block.h"
#include "cybertron/transport/shm/shm_conf.h"
#include "cybertron/transport/shm/state.h"

namespace apollo {
namespace cybertron {
namespace transport {

class Segment;
using SegmentPtr = std::shared_ptr<Segment>;

enum ReadWriteMode {
  READ_ONLY = 0,
  WRITE_ONLY,
};

class Segment {
 public:
  Segment(uint64_t channel_id, const ReadWriteMode& mode);
  virtual ~Segment();

  bool Write(const std::string& msg, const std::string& msg_info,
             uint32_t* block_index);
  bool Read(uint32_t block_index, std::string* msg, std::string* msg_info);

 private:
  bool Init();
  bool OpenOrCreate();
  bool OpenOnly();
  bool Remove();
  bool Destroy();
  void Reset();
  bool Remap();
  bool Recreate();

  uint32_t GetNextWritableBlockIndex();

  bool init_;
  uint64_t id_;
  ReadWriteMode mode_;
  ShmConf conf_;

  State* state_;
  Block* blocks_;
  std::unordered_map<uint32_t, uint8_t*> block_buf_addrs_;
  void* managed_shm_ = nullptr;
};

}  // namespace transport
}  // namespace cybertron
}  // namespace apollo

#endif  // CYBERTRON_TRANSPORT_SHM_SEGMENT_H_
