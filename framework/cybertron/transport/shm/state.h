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

#ifndef CYBERTRON_TRANSPORT_SHM_STATE_H_
#define CYBERTRON_TRANSPORT_SHM_STATE_H_

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>

namespace apollo {
namespace cybertron {
namespace transport {

class State {
 public:
  explicit State(const uint64_t& ceiling_msg_size);
  virtual ~State();

  void set_need_remap(bool need) { need_remap_.store(need); }

  bool need_remap() { return need_remap_; }

  uint64_t ceiling_msg_size() { return ceiling_msg_size_.load(); }

  void IncreaseWroteNum() { wrote_num_.fetch_add(1); }

  void ResetWroteNum() { wrote_num_.store(0); }

  uint32_t wrote_num() { return wrote_num_.load(); }

  void IncreaseReferenceCounts() { reference_count_.fetch_add(1); }

  void DecreaseReferenceCounts() {
    uint32_t current_reference_count = reference_count_.load();
    do {
      if (current_reference_count == 0) {
        return;
      }
    } while (!reference_count_.compare_exchange_strong(
        current_reference_count, current_reference_count - 1));
  }

  uint32_t reference_counts() { return reference_count_.load(); }

 private:
  std::atomic<bool> need_remap_ = {false};
  std::atomic<uint64_t> ceiling_msg_size_;
  std::atomic<uint32_t> wrote_num_ = {0};
  std::atomic<uint32_t> reference_count_ = {0};
};

}  // namespace transport
}  // namespace cybertron
}  // namespace apollo

#endif  // CYBERTRON_TRANSPORT_SHM_STATE_H_
