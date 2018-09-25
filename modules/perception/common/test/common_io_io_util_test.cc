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
#include <gtest/gtest.h>

#include "modules/perception/common/io/io_util.h"

namespace apollo {
namespace perception {
namespace common {

TEST(CommonIoTest, read_pose_file) {
  std::string filename;
  Eigen::Affine3d pose;
  int frame_id = 0;
  double time_stamp = 0;

  EXPECT_FALSE(ReadPoseFile(filename, nullptr, nullptr, nullptr));
  EXPECT_FALSE(ReadPoseFile(filename, &pose, &frame_id, &time_stamp));

  filename = "./common_test_data/io/params/pose_file.txt";
  EXPECT_TRUE(ReadPoseFile(filename, &pose, &frame_id, &time_stamp));
}

TEST(CommonIoTest, load_camera_intrinsic) {
  std::string yaml_file;
  base::BrownCameraDistortionModel model;

  EXPECT_FALSE(LoadBrownCameraIntrinsic(yaml_file, nullptr));
  EXPECT_FALSE(LoadBrownCameraIntrinsic(yaml_file, &model));

  yaml_file = "./common_test_data/io/params/empty.yaml";
  EXPECT_FALSE(LoadBrownCameraIntrinsic(yaml_file, &model));

  yaml_file = "./common_test_data/io/params/test.yaml";
  EXPECT_TRUE(LoadBrownCameraIntrinsic(yaml_file, &model));
  EXPECT_EQ(model.width_, 1192);
  EXPECT_EQ(model.height_, 712);
}

TEST(CommonIoTest, load_ocamera_intrinsic) {
  std::string yaml_file;
  base::OmnidirectionalCameraDistortionModel model;

  EXPECT_FALSE(LoadOmnidirectionalCameraIntrinsics(yaml_file, nullptr));
  EXPECT_FALSE(LoadOmnidirectionalCameraIntrinsics(yaml_file, &model));

  yaml_file = "./common_test_data/io/params/empty.yaml";
  EXPECT_FALSE(LoadOmnidirectionalCameraIntrinsics(yaml_file, &model));

  yaml_file = "./common_test_data/io/params/test_ocam.yaml";
  EXPECT_TRUE(LoadOmnidirectionalCameraIntrinsics(yaml_file, &model));
  EXPECT_EQ(model.width_, 1920);
  EXPECT_EQ(model.height_, 1080);

  yaml_file = "./common_test_data/io/params/test_ocam1.yaml";
  EXPECT_FALSE(LoadOmnidirectionalCameraIntrinsics(yaml_file, &model));
  yaml_file = "./common_test_data/io/params/test_ocam2.yaml";
  EXPECT_FALSE(LoadOmnidirectionalCameraIntrinsics(yaml_file, &model));
  yaml_file = "./common_test_data/io/params/test_ocam3.yaml";
  EXPECT_FALSE(LoadOmnidirectionalCameraIntrinsics(yaml_file, &model));
  yaml_file = "./common_test_data/io/params/test_ocam4.yaml";
  EXPECT_FALSE(LoadOmnidirectionalCameraIntrinsics(yaml_file, &model));
  yaml_file = "./common_test_data/io/params/test_ocam5.yaml";
  EXPECT_FALSE(LoadOmnidirectionalCameraIntrinsics(yaml_file, &model));
  yaml_file = "./common_test_data/io/params/test_ocam6.yaml";
  EXPECT_FALSE(LoadOmnidirectionalCameraIntrinsics(yaml_file, &model));
  yaml_file = "./common_test_data/io/params/test_ocam7.yaml";
  EXPECT_FALSE(LoadOmnidirectionalCameraIntrinsics(yaml_file, &model));
  yaml_file = "./common_test_data/io/params/test_ocam8.yaml";
  EXPECT_FALSE(LoadOmnidirectionalCameraIntrinsics(yaml_file, &model));
}

}  // namespace common
}  // namespace perception
}  // namespace apollo
