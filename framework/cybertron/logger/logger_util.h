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

/**
 * @file
 */

#ifndef CYBERTRON_LOGGER_LOGGER_UTIL_H_
#define CYBERTRON_LOGGER_LOGGER_UTIL_H_

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>
#include <cstdint>
#include <string>
#include <vector>

#include <glog/logging.h>

namespace apollo {
namespace cybertron {
namespace logger {

int64_t CycleClock_Now() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return static_cast<int64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}

static void GetHostName(std::string* hostname) {
  struct utsname buf;
  if (0 != uname(&buf)) {
    *buf.nodename = '\0';
  }
  *hostname = buf.nodename;
}

static std::vector<std::string>* logging_directories_list;
const std::vector<std::string>& GetLoggingDirectories() {
  if (logging_directories_list == NULL) {
    logging_directories_list = new std::vector<std::string>;

    if (!FLAGS_log_dir.empty()) {
      logging_directories_list->push_back(FLAGS_log_dir.c_str());
    } else {
      // GetTempDirectories(logging_directories_list);
      logging_directories_list->push_back("./");
    }
  }
  return *logging_directories_list;
}

static int32_t g_main_thread_pid = getpid();
int32_t GetMainThreadPid() { return g_main_thread_pid; }

bool PidHasChanged() {
  int32_t pid = getpid();
  if (g_main_thread_pid == pid) {
    return false;
  }
  g_main_thread_pid = pid;
  return true;
}

static int32_t MaxLogSize() {
  return (FLAGS_max_log_size > 0 ? FLAGS_max_log_size : 1);
}

}  // namespace logger
}  // namespace cybertron
}  // namespace apollo

#endif  // CYBERTRON_LOGGER_LOGGER_UTIL_H_
