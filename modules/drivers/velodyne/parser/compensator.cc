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

#include "modules/drivers/velodyne/parser/compensator.h"

namespace apollo {
namespace drivers {
namespace velodyne {

Compensator::Compensator(Config& velodyne_config) {
  tf2_buffer_ptr_ = Buffer::Instance();
  config_ = velodyne_config;
}

bool Compensator::query_pose_affine_from_tf2(
    const uint64_t& timestamp, void* pose,
    const std::string& child_frame_id) {

  cybertron::Time query_time(timestamp);
  std::string err_string;
  if (!tf2_buffer_ptr_->canTransform("world", child_frame_id, query_time, 0.02,
                                &err_string)) {
    AERROR << "Can not find transform. " << timestamp
               << " frame_id:" << child_frame_id
               << " Error info: " << err_string;
    return false;
  }

  adu::common::TransformStamped stamped_transform;

  try {
    stamped_transform =
        tf2_buffer_ptr_->lookupTransform("world", child_frame_id, query_time);
  }
  catch (tf2::TransformException& ex) {
    AERROR << ex.what();
    return false;
  }

  Eigen::Affine3d* tmp_pose = (Eigen::Affine3d* ) pose;
  *tmp_pose = Eigen::Translation3d(stamped_transform.transform().translation().x(),
                              stamped_transform.transform().translation().y(),
                              stamped_transform.transform().translation().z()) *
         Eigen::Quaterniond(stamped_transform.transform().rotation().qw(),
                            stamped_transform.transform().rotation().qx(),
                            stamped_transform.transform().rotation().qy(),
                            stamped_transform.transform().rotation().qz());
  return true;
}

bool Compensator::motion_compensation(
    const std::shared_ptr<const PointCloud>& msg,
    std::shared_ptr<PointCloud>& msg_compensated) {

  uint64_t start = cybertron::Time::Now().ToNanosecond();
  Eigen::Affine3d pose_min_time;
  Eigen::Affine3d pose_max_time;

  uint64_t timestamp_min = 0;
  uint64_t timestamp_max = 0;
  std::string frame_id = msg->header().frame_id();
  get_timestamp_interval(msg, timestamp_min, timestamp_max);

  msg_compensated->mutable_header()->set_timestamp_sec(cybertron::Time::Now().ToSecond());
  msg_compensated->mutable_header()->set_frame_id(msg->header().frame_id());
  msg_compensated->mutable_header()->set_lidar_timestamp(msg->header().lidar_timestamp());
  msg_compensated->set_measurement_time(msg->measurement_time());
  msg_compensated->set_height(msg->height());
  msg_compensated->set_is_dense(msg->is_dense());
  
  uint64_t new_time = cybertron::Time().Now().ToNanosecond();
  AINFO << "compenstator new msg diff:" << new_time - start
    << ";meta:" << msg->header().lidar_timestamp();
  msg_compensated->mutable_point()->Reserve(140000);

  // compensate point cloud, remove nan point
  if (query_pose_affine_from_tf2(timestamp_min, &pose_min_time, frame_id) &&
      query_pose_affine_from_tf2(timestamp_max, &pose_max_time, frame_id)) {
    uint64_t tf_time = cybertron::Time().Now().ToNanosecond();
    AINFO << "compenstator tf msg diff:" << tf_time - new_time
      << ";meta:" << msg->header().lidar_timestamp();
    motion_compensation(msg, msg_compensated, timestamp_min, timestamp_max,
                        pose_min_time, pose_max_time);
    uint64_t com_time = cybertron::Time().Now().ToNanosecond();

    msg_compensated->set_width(msg_compensated->point_size() / msg->height());
    AINFO << "compenstator com msg diff:" << com_time - tf_time
      << ";meta:" << msg->header().lidar_timestamp();
    return true;
  }
  return false;
}

inline void Compensator::get_timestamp_interval(
    const std::shared_ptr<const PointCloud>& msg,
    uint64_t& timestamp_min, uint64_t& timestamp_max) {

  timestamp_max = 0;
  timestamp_min = std::numeric_limits<uint64_t>::max();

  for (auto& point : msg->point()) {
    uint64_t timestamp = point.timestamp();
    if (timestamp < timestamp_min) {
      timestamp_min = timestamp;
    }

    if (timestamp > timestamp_max) {
      timestamp_max = timestamp;
    }
  }
}

inline bool Compensator::is_valid(Eigen::Vector3d& point) {
  float x = point.x();
  float y = point.y();
  float z = point.z();
  if ( abs(x) > config_.max_range()
      || abs(y) > config_.max_range()
      || abs(z) > config_.max_range()) {
    return false;
  }
  return true;
}

void Compensator::motion_compensation(
    const std::shared_ptr<const PointCloud>& msg,
    std::shared_ptr<PointCloud>& msg_compensated,
    const uint64_t timestamp_min, const uint64_t timestamp_max,
    const Eigen::Affine3d& pose_min_time,
    const Eigen::Affine3d& pose_max_time) {

  using std::abs;
  using std::sin;
  using std::acos;

  Eigen::Vector3d translation =
      pose_min_time.translation() - pose_max_time.translation();
  Eigen::Quaterniond q_max(pose_max_time.linear());
  Eigen::Quaterniond q_min(pose_min_time.linear());
  Eigen::Quaterniond q1(q_max.conjugate() * q_min);
  Eigen::Quaterniond q0(Eigen::Quaterniond::Identity());
  q1.normalize();
  translation = q_max.conjugate() * translation;

  // int total = msg->width * msg->height;

  double d = q0.dot(q1);
  double abs_d = abs(d);
  double f = 1.0 / (timestamp_max - timestamp_min);

  // Threshold for a "significant" rotation from min_time to max_time:
  // The LiDAR range accuracy is ~2 cm. Over 70 meters range, it means an angle
  // of 0.02 / 70 =
  // 0.0003 rad. So, we consider a rotation "significant" only if the scalar
  // part of quaternion is
  // less than cos(0.0003 / 2) = 1 - 1e-8.
  if (abs_d < 1.0 - 1.0e-8) {
    double theta = acos(abs_d);
    double sin_theta = sin(theta);
    double c1_sign = (d > 0) ? 1 : -1;
    // for (int i = 0; i < total; ++i) {
    for (auto& point : msg->point()) {
      float x_scalar = point.x();
      if (std::isnan(x_scalar)) {
        if (config_.organized()) {
          auto* point_new = msg_compensated->add_point();
          point_new->CopyFrom(point);
        } else {
          AERROR << "nan point do not need motion compensation";
        }
        continue;
      }
      float y_scalar = point.y();
      float z_scalar = point.z();
      Eigen::Vector3d p(x_scalar, y_scalar, z_scalar);

      uint64_t tp = point.timestamp();
      double t = (timestamp_max - tp) * f;

      Eigen::Translation3d ti(t * translation);

      double c0 = sin((1 - t) * theta) / sin_theta;
      double c1 = sin(t * theta) / sin_theta * c1_sign;
      Eigen::Quaterniond qi(c0 * q0.coeffs() + c1 * q1.coeffs());

      Eigen::Affine3d trans = ti * qi;
      p = trans * p;

      if (!is_valid(p)){
        if (config_.organized()) {
          auto* point_new = msg_compensated->add_point();
          point_new->CopyFrom(point);
        }
        continue;
      }

      auto* point_new = msg_compensated->add_point();
      point_new->set_intensity(point.intensity());
      point_new->set_timestamp(point.timestamp());
      point_new->set_x(p.x());
      point_new->set_y(p.y());
      point_new->set_z(p.z());
    }
    return;
  }
  // Not a "significant" rotation. Do translation only.
  for (auto& point : msg->point()) {
    float x_scalar = point.x();
    if (std::isnan(x_scalar)) {
      AERROR << "nan point do not need motion compensation";
      continue;
    }
    float y_scalar = point.y();
    float z_scalar = point.z();
    Eigen::Vector3d p(x_scalar, y_scalar, z_scalar);

    uint64_t tp = point.timestamp();
    double t = (timestamp_max - tp) * f;
    Eigen::Translation3d ti(t * translation);

    p = ti * p;

    if (!is_valid(p)){
      AINFO << "invaid point,x:" << p.x() << ";y:" << p.y() << ";z:" << p.z();
      continue;
    }
    auto* point_new = msg_compensated->add_point();
    point_new->set_intensity(point.intensity());
    point_new->set_timestamp(point.timestamp());
    point_new->set_x(p.x());
    point_new->set_y(p.y());
    point_new->set_z(p.z());
  }
}

}  // namespace velodyne_pointcloud
}
}
