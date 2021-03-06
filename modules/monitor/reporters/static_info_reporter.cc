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

#include "modules/monitor/reporters/static_info_reporter.h"

#include "cybertron/common/log.h"
#include "gflags/gflags.h"
#include "modules/common/adapters/adapter_gflags.h"
#include "modules/data/util/info_collector.h"
#include "modules/monitor/common/monitor_manager.h"
#include "modules/routing/proto/routing.pb.h"

DEFINE_string(static_info_reporter_name, "StaticInfoReporter",
              "Static info reporter name.");

DEFINE_double(static_info_report_interval, 40,
              "Static info reporting interval (s).");

namespace apollo {
namespace monitor {

using apollo::data::InfoCollector;

StaticInfoReporter::StaticInfoReporter()
    : RecurrentRunner(FLAGS_static_info_reporter_name,
                      FLAGS_static_info_report_interval) {
  InfoCollector::Init(CHECK_NOTNULL(MonitorManager::CurrentNode()));
}

void StaticInfoReporter::RunOnce(const double current_time) {
  static auto writer = MonitorManager::CreateWriter<apollo::data::StaticInfo>(
      FLAGS_static_info_topic);
  AINFO << "Reported static info.";
  writer->Write(InfoCollector::GetStaticInfo());
}

}  // namespace monitor
}  // namespace apollo
