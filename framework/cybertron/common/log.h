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
 * @log
 */

#ifndef CYBERTRON_COMMON_LOG_H_
#define CYBERTRON_COMMON_LOG_H_

#include <stdarg.h>
#include "cybertron/binary.h"
#include "glog/logging.h"
#include "glog/raw_logging.h"

#define LEFT_BRACKET "["
#define RIGHT_BRACKET "] "
#define DEFAULT_NAME "mainboard"

#ifndef MODULE_NAME
#define MODULE_NAME DEFAULT_NAME
#endif

#define ADEBUG \
  VLOG(4) << LEFT_BRACKET << MODULE_NAME << RIGHT_BRACKET << "[DEBUG] "
#define AINFO ALOG_MODULE(MODULE_NAME, INFO)
#define AWARN ALOG_MODULE(MODULE_NAME, WARN)
#define AERROR ALOG_MODULE(MODULE_NAME, ERROR)
#define AFATAL ALOG_MODULE(MODULE_NAME, ERROR)

#ifndef ALOG_MODULE_STREAM
#define ALOG_MODULE_STREAM(log_severity) ALOG_MODULE_STREAM_##log_severity
#endif

#ifndef ALOG_MODULE
#define ALOG_MODULE(module, log_severity) \
  ALOG_MODULE_STREAM(log_severity)(module)
#endif

#define ALOG_MODULE_STREAM_INFO(module)                                \
  (module != DEFAULT_NAME                                              \
       ? google::LogMessage(__FILE__, __LINE__, google::INFO).stream() \
             << LEFT_BRACKET << module << RIGHT_BRACKET                \
       : google::LogMessage(__FILE__, __LINE__, google::INFO).stream() \
             << LEFT_BRACKET << apollo::cybertron::Binary::GetName()   \
             << RIGHT_BRACKET)

#define ALOG_MODULE_STREAM_WARN(module)                                   \
  (module != DEFAULT_NAME                                                 \
       ? google::LogMessage(__FILE__, __LINE__, google::WARNING).stream() \
             << LEFT_BRACKET << module << RIGHT_BRACKET                   \
       : google::LogMessage(__FILE__, __LINE__, google::WARNING).stream() \
             << LEFT_BRACKET << apollo::cybertron::Binary::GetName()      \
             << RIGHT_BRACKET)

#define ALOG_MODULE_STREAM_ERROR(module)                                \
  (module != DEFAULT_NAME                                               \
       ? google::LogMessage(__FILE__, __LINE__, google::ERROR).stream() \
             << LEFT_BRACKET << module << RIGHT_BRACKET                 \
       : google::LogMessage(__FILE__, __LINE__, google::ERROR).stream() \
             << LEFT_BRACKET << apollo::cybertron::Binary::GetName()    \
             << RIGHT_BRACKET)

#define ALOG_MODULE_STREAM_FATAL(module)                                \
  (module != DEFAULT_NAME                                               \
       ? google::LogMessage(__FILE__, __LINE__, google::FATAL).stream() \
             << LEFT_BRACKET << module << RIGHT_BRACKET                 \
       : google::LogMessage(__FILE__, __LINE__, google::FATAL).stream() \
             << LEFT_BRACKET << apollo::cybertron::Binary::GetName()    \
             << RIGHT_BRACKET)

#define AINFO_IF(cond) ALOG_IF(INFO, cond, MODULE_NAME)
#define AWARN_IF(cond) ALOG_IF(WARN, cond, MODULE_NAME)
#define AERROR_IF(cond) ALOG_IF(ERROR, cond, MODULE_NAME)
#define ALOG_IF(severity, cond, module) \
  !(cond) ? (void)0                     \
          : google::LogMessageVoidify() & ALOG_MODULE(module, severity)

#define ACHECK(cond) CHECK(cond) << LEFT_BRACKET << MODULE_NAME << RIGHT_BRACKET

#define AINFO_EVERY(freq) \
  LOG_EVERY_N(INFO, freq) << LEFT_BRACKET << MODULE_NAME << RIGHT_BRACKET
#define AWARN_EVERY(freq) \
  LOG_EVERY_N(WARNING, freq) << LEFT_BRACKET << MODULE_NAME << RIGHT_BRACKET
#define AERROR_EVERY(freq) \
  LOG_EVERY_N(ERROR, freq) << LEFT_BRACKET << MODULE_NAME << RIGHT_BRACKET

#define RETURN_IF_NULL(ptr)          \
  if (ptr == nullptr) {              \
    AWARN << #ptr << " is nullptr."; \
    return;                          \
  }

#define RETURN_VAL_IF_NULL(ptr, val) \
  if (ptr == nullptr) {              \
    AWARN << #ptr << " is nullptr."; \
    return val;                      \
  }

#define RETURN_IF(condition)               \
  if (condition) {                         \
    AWARN << #condition << " is not met."; \
    return;                                \
  }

#define RETURN_VAL_IF(condition, val)      \
  if (condition) {                         \
    AWARN << #condition << " is not met."; \
    return val;                            \
  }

#endif  // CYBERTRON_COMMON_LOG_H_
