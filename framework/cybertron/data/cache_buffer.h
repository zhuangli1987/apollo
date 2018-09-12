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

#ifndef CYBERTRON_DATA_CACHE_BUFFER_H_
#define CYBERTRON_DATA_CACHE_BUFFER_H_

#include <mutex>
#include <vector>

namespace apollo {
namespace cybertron {
namespace data {

template <typename T>
class CacheBuffer {
 public:
  using value_type = T;
  using size_type = std::size_t;

  explicit CacheBuffer(uint32_t size) {
    capacity_ = size + 1;
    buffer_.resize(capacity_);
  }

  CacheBuffer(CacheBuffer&& rhs) {
    head_ = rhs.head_;
    tail_ = rhs.head_;
    buffer_ = rhs.buffer_;
    capacity_ = rhs.capacity_;
  }

  T& operator[](uint64_t pos) { return buffer_[GetIndex(pos)]; }
  const T& at(uint64_t pos) { return buffer_[GetIndex(pos)]; }

  uint64_t Head() { return head_ + 1; }
  uint64_t Tail() { return tail_; }
  uint64_t Size() { return tail_ - head_; }

  const T& Front() { return buffer_[GetIndex(head_ + 1)]; }
  const T& Back() { return buffer_[GetIndex(tail_)]; }

  bool Empty() { return tail_ == 0; }
  bool Full() { return capacity_ - 1 == tail_ - head_; }

  void Fill(const T& value) {
    if (Full()) {
      buffer_[GetIndex(head_)] = value;
      ++head_;
      ++tail_;
    } else {
      buffer_[GetIndex(tail_ + 1)] = value;
      ++tail_;
    }
  }

  std::mutex& Mutex() { return mutex_; }

 private:
  CacheBuffer(const CacheBuffer& other) = delete;
  CacheBuffer& operator=(const CacheBuffer& other) = delete;
  uint64_t GetIndex(uint64_t pos) { return pos % capacity_; }
  std::vector<T> buffer_;
  uint64_t head_ = 0;
  uint64_t tail_ = 0;
  uint64_t capacity_ = 0;
  std::mutex mutex_;
};

}  // namespace data
}  // namespace cybertron
}  // namespace apollo

#endif  // CYBERTRON_DATA_CACHE_BUFFER_H_
