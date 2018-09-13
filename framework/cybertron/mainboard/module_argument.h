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

#ifndef CYBERTRON_MAINBOARD_MODULE_ARGUMENT_H_
#define CYBERTRON_MAINBOARD_MODULE_ARGUMENT_H_

#include <list>
#include <string>

#include "cybertron/common/global_data.h"
#include "cybertron/common/log.h"
#include "cybertron/common/types.h"

namespace apollo {
namespace cybertron {
namespace mainboard {

static const char DEFAULT_process_name_[] = "mainboard_default";

class ModuleArgument {
 public:
  ModuleArgument();
  virtual ~ModuleArgument();
  void DisplayUsage();
  bool ParseArgument(int argc, char* const argv[]);
  void GetOptions(const int& argc, char* const argv[]);
  inline std::string GetBinaryName() const { return binary_name_; }
  inline std::string GetProcessName() const { return process_name_; }
  inline std::list<std::string> GetDAGConfList() const {
    return dag_conf_list_;
  }

 private:
  std::list<std::string> dag_conf_list_;
  std::string binary_name_;
  std::string process_name_;
};

}  // namespace mainboard
}  // namespace cybertron
}  // namespace apollo

#endif  // CYBERTRON_MAINBOARD_MODULE_ARGUMENT_H_
