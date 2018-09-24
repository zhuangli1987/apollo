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

#include "cybertron/logger/async_logger.h"
#include "cybertron/logger/log_file_object.h"

#include <stdlib.h>
#include <string>
#include <thread>
#include <unordered_map>

namespace apollo {
namespace cybertron {
namespace logger {

static std::unordered_map<std::string, LogFileObject*> moduleLoggerMap;

AsyncLogger::AsyncLogger(google::base::Logger* wrapped, int max_buffer_bytes)
    : max_buffer_bytes_(max_buffer_bytes),
      wrapped_(wrapped),
      active_buf_(new Buffer()),
      flushing_buf_(new Buffer()) {
  if (max_buffer_bytes_ <= 0) {
    max_buffer_bytes_ = 2 * 1024 * 1024;
  }
}

AsyncLogger::~AsyncLogger() {
  Stop();
  for (auto itr = moduleLoggerMap.begin(); itr != moduleLoggerMap.end();
       ++itr) {
    delete itr->second;
  }
}

void AsyncLogger::Start() {
  CHECK_EQ(state_, INITTED);
  state_ = RUNNING;
  thread_ = std::thread(&AsyncLogger::RunThread, this);
  // std::cout << "Async Logger Start!" << std::endl;
}

void AsyncLogger::Stop() {
  {
    std::unique_lock<std::mutex> lock(mutex_);
    CHECK_EQ(state_, RUNNING);
    state_ = STOPPED;
    wake_flusher_cv_.notify_one();
  }
  thread_.join();
  CHECK(active_buf_->messages.empty());
  CHECK(flushing_buf_->messages.empty());
  // std::cout << "Async Logger Stop!" << std::endl;
}

void AsyncLogger::Write(bool force_flush, time_t timestamp, const char* message,
                        int message_len) {
  {
    std::unique_lock<std::mutex> lock(mutex_);
    if (state_ != RUNNING) {
      // std::cout << "Async Logger not running!" << std::endl;
      return;
    }

    // origin implementation
    // while (BufferFull(*active_buf_)) {
    //  free_buffer_cv_.wait(lock);
    //}

    // drop message when acitve buffer is full
    if (BufferFull(*active_buf_)) {
      return;
    }

    if (message_len <= 0) {
      return;
    }

    switch (message[0]) {
      case 'F': {
        active_buf_->add(Msg(timestamp, std::string(message, message_len), 3),
                         force_flush);
        break;
      }
      case 'E': {
        active_buf_->add(Msg(timestamp, std::string(message, message_len), 2),
                         force_flush);
        break;
      }
      case 'W': {
        active_buf_->add(Msg(timestamp, std::string(message, message_len), 1),
                         force_flush);
        break;
      }
      case 'I': {
        active_buf_->add(Msg(timestamp, std::string(message, message_len), 0),
                         force_flush);
        break;
      }
      default: {
        active_buf_->add(Msg(timestamp, std::string(message, message_len), -1),
                         force_flush);
      }
    }
    wake_flusher_cv_.notify_one();
  }
}

void AsyncLogger::Flush() {
  std::unique_lock<std::mutex> lock(mutex_);
  if (state_ != RUNNING) {
    // std::cout << "Async Logger not running!" << std::endl;
    return;
  }

  // Wake up the writer thread at least twice.
  // This ensures that it has completely flushed both buffers.
  uint64_t orig_flush_count = flush_count_;
  while (flush_count_ < (orig_flush_count + 2) && state_ == RUNNING) {
    active_buf_->flush = true;
    wake_flusher_cv_.notify_one();
    flush_complete_cv_.wait(lock);
  }
}

uint32_t AsyncLogger::LogSize() { return wrapped_->LogSize(); }

void AsyncLogger::RunThread() {
  std::unique_lock<std::mutex> lock(mutex_);
  while (state_ == RUNNING || active_buf_->needs_flush_or_write()) {
    while (!active_buf_->needs_flush_or_write() && state_ == RUNNING) {
      if (wake_flusher_cv_.wait_for(lock, std::chrono::seconds(2)) ==
          std::cv_status::timeout) {
        active_buf_->flush = true;
      }
    }

    active_buf_.swap(flushing_buf_);
    lock.unlock();

    for (const auto& msg : flushing_buf_->messages) {
      std::string module_name;
      int lpos = msg.message.find("[");
      int rpos = msg.message.find("]", lpos);
      if (lpos != std::string::npos && rpos != std::string::npos) {
        module_name = msg.message.substr(lpos + 1, rpos - lpos - 1);
      }
      if (module_name.empty()) {
        module_name = "unknown";
      }
      LogFileObject* fileobject = nullptr;
      if (moduleLoggerMap.find(module_name) != moduleLoggerMap.end()) {
        fileobject = moduleLoggerMap[module_name];
      } else {
        fileobject = new LogFileObject(google::INFO, module_name.c_str());
        fileobject->SetSymlinkBasename(module_name.c_str());
        moduleLoggerMap[module_name] = fileobject;
      }
      if (fileobject) {
        const bool should_flush = msg.level > 0;
        fileobject->Write(should_flush, msg.ts, msg.message.data(),
                          msg.message.size());
      }
    }

    if (flushing_buf_->flush) {
      wrapped_->Flush();
    }
    flushing_buf_->clear();

    lock.lock();
    flush_count_++;
    flush_complete_cv_.notify_all();
  }
}

bool AsyncLogger::BufferFull(const Buffer& buf) const {
  // We evenly divide our total buffer space between the two buffers.
  return buf.size > (max_buffer_bytes_ / 2);
}

}  // namespace logger
}  // namespace cybertron
}  // namespace apollo
