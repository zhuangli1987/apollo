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

#ifndef MODULES_DRIVERS_VELODYNE_DRIVER_DRIVER_H_
#define MODULES_DRIVERS_VELODYNE_DRIVER_DRIVER_H_

#include <memory>
#include <string>

#include "modules/drivers/velodyne/driver/socket_input.h"
#include "modules/drivers/velodyne/proto/config.pb.h"
#include "modules/drivers/velodyne/proto/velodyne.pb.h"

namespace apollo {
namespace drivers {
namespace velodyne {

using apollo::drivers::velodyne::HDL32E;
using apollo::drivers::velodyne::HDL64E_S2;
using apollo::drivers::velodyne::HDL64E_S3D;
using apollo::drivers::velodyne::HDL64E_S3S;
using apollo::drivers::velodyne::VelodynePacket;
using apollo::drivers::velodyne::VelodyneScan;
using apollo::drivers::velodyne::VLP16;
using apollo::drivers::velodyne::VLP32C;
using apollo::drivers::velodyne::VLS128;
using apollo::drivers::velodyne::config::Config;

constexpr int BLOCKS_PER_PACKET = 12;
constexpr int BLOCK_SIZE = 100;

// configuration parameters
// struct Config {
//  std::string frame_id;  ///< tf frame ID
//  std::string model;     ///< device model name
//  std::string topic;
//  int npackets = 0;  ///< number of packets to collect
//  double rpm = 0.0;  ///< device rotation rate (RPMs)
//  int firing_data_port = 0;
//  int positioning_data_port = 0;
//  int prefix_angle = 0;  // prefix angle to recv
//  bool use_sensor_sync = false;
//};

class VelodyneDriver {
 public:
  VelodyneDriver();
  virtual ~VelodyneDriver() {}

  virtual bool poll(std::shared_ptr<VelodyneScan> scan) = 0;
  virtual void init() = 0;

 protected:
  Config config_;
  std::shared_ptr<Input> input_;
  // ros::Publisher output_;
  std::string topic_;

  uint64_t basetime_;
  uint32_t last_gps_time_;

  virtual int poll_standard(std::shared_ptr<VelodyneScan> scan);
  bool set_base_time();
  void set_base_time_from_nmea_time(NMEATimePtr nmea_time, uint64_t *basetime);
  void update_gps_top_hour(uint32_t current_time);
};

class Velodyne64Driver : public VelodyneDriver {
 public:
  explicit Velodyne64Driver(const Config &config);
  virtual ~Velodyne64Driver() {}

  void init();
  bool poll(std::shared_ptr<VelodyneScan> scan);

 private:
  bool check_angle(const VelodynePacket &packet);
  int poll_standard_sync(std::shared_ptr<VelodyneScan> scan);
};

class Velodyne32Driver : public VelodyneDriver {
 public:
  explicit Velodyne32Driver(const Config &config);
  virtual ~Velodyne32Driver() {}
  void init();
  bool poll(std::shared_ptr<VelodyneScan> scan);
  void poll_positioning_packet();

 private:
  std::shared_ptr<Input> positioning_input_;
};

class Velodyne16Driver : public VelodyneDriver {
 public:
  explicit Velodyne16Driver(const Config &config);
  virtual ~Velodyne16Driver() {}

  void init();
  bool poll(std::shared_ptr<VelodyneScan> scan);
  void poll_positioning_packet();

 private:
  std::shared_ptr<Input> positioning_input_;
};

class Velodyne128Driver : public VelodyneDriver {
 public:
  explicit Velodyne128Driver(const Config &config);
  virtual ~Velodyne128Driver() {}

  void init();
  bool poll(std::shared_ptr<VelodyneScan> scan);
  void poll_positioning_packet();

 private:
  std::shared_ptr<Input> positioning_input_;
};

class VelodyneDriverFactory {
 public:
  static VelodyneDriver *create_driver(const Config &config);
};

}  // namespace velodyne
}  // namespace drivers
}  // namespace apollo

#endif  // VELODYNE_DRIVER_H_
