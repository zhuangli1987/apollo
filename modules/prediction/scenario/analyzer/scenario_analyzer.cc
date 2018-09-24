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

#include "modules/prediction/scenario/analyzer/scenario_analyzer.h"
#include "modules/prediction/common/prediction_gflags.h"

namespace apollo {
namespace prediction {

void ScenarioAnalyzer::Analyze(
    const EnvironmentFeatures& environment_features) {
  if (environment_features.has_front_junction() &&
      environment_features.GetFrontJunction().second <
          FLAGS_junction_distance_threshold) {
    scenario_.set_type(Scenario::JUNCTION);
  } else if (environment_features.has_ego_lane()) {
    scenario_.set_type(Scenario::CRUISE);
  }
}

const Scenario& ScenarioAnalyzer::scenario() const {
  return scenario_;
}

std::shared_ptr<ScenarioFeatures>
ScenarioAnalyzer::GetScenarioFeatures() {
  return nullptr;
}

}  // namespace prediction
}  // namespace apollo
