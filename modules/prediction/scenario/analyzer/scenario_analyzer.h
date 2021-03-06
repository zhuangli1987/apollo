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

#ifndef MODULES_PREDICTION_SCENARIO_ANALYZER_SCENARIO_ANALYZER_H_
#define MODULES_PREDICTION_SCENARIO_ANALYZER_SCENARIO_ANALYZER_H_

#include <memory>

#include "modules/prediction/proto/scenario.pb.h"
#include "modules/prediction/common/environment_features.h"
#include "modules/prediction/scenario/scenario_features/scenario_features.h"

namespace apollo {
namespace prediction {

class ScenarioAnalyzer {
 public:
  ScenarioAnalyzer();

  virtual ~ScenarioAnalyzer() = default;

  void Analyze(const EnvironmentFeatures& environment_features);

  const Scenario& scenario() const;

  std::shared_ptr<ScenarioFeatures> GetScenarioFeatures();

 private:
  void BuildCruiseScenarioFeatures(
      const EnvironmentFeatures& environment_features);

 private:
  Scenario scenario_;

  std::shared_ptr<ScenarioFeatures> scenario_features_;
};

}  // namespace prediction
}  // namespace apollo

#endif  // MODULES_PREDICTION_SCENARIO_ANALYZER_SCENARIO_ANALYZER_H_
