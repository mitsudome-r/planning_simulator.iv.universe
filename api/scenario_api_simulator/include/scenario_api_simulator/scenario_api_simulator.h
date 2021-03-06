/*
 * Copyright 2018-2019 Autoware Foundation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SCENARIO_API_SCENARIO_API_SIMULATOR_H_INCLUDED
#define SCENARIO_API_SCENARIO_API_SIMULATOR_H_INCLUDED

#include <dummy_perception_publisher/InitialState.h>
#include <npc_simulator/GetObject.h>
#include <npc_simulator/Object.h>
#include <ros/ros.h>
#include <std_msgs/Bool.h>
#include <tf2/convert.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <unique_id/unique_id.h>
#include <unistd.h>
#include <uuid/uuid.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <scenario_api_simulator/npc_route_manager.h>
#include <scenario_api_utils/scenario_api_utils.h>

class ScenarioAPISimulator
{
public:
  /**
   * @brief constructor
   */
  ScenarioAPISimulator();

  /**
   * @brief destructor
   */
  ~ScenarioAPISimulator();

  // basic API
  bool isAPIReady();
  bool updateState();  // update state //TODO

  // start API
  bool spawnStartPoint(const geometry_msgs::Pose pose);  // spawn vehicle at start point
  bool sendEngage(const bool engage);                    //engage simulator and npc
  bool sendSimulatorEngage(const bool engage);           //engage simulator
  bool sendNPCEngage(const bool engage);                 //engage npc

  // NPC API
  bool getNPC(const std::string & name, npc_simulator::Object & obj);
  bool getNPC(
    const std::string & name, geometry_msgs::Pose & object_pose,
    geometry_msgs::Twist & object_twist, geometry_msgs::Vector3 & object_size,
    std::string & object_name);
  bool getNPCPosition(const std::string & name, geometry_msgs::Pose * pose);
  bool getNPCVelocity(const std::string & name, double * velocity);
  bool getNPCAccel(const std::string & name, double * accel);
  bool getNPCGoal(const std::string & name, geometry_msgs::Pose * pose);
  bool addNPC(
    const std::string & npc_type, const std::string & name, geometry_msgs::Pose pose,
    const double velocity, const bool stop_by_vehicle, const std::string & frame_type);
  bool sendNPCToCheckPoint(
    const std::string & name, const geometry_msgs::Pose checkpoint_pose, const bool wait_ready,
    const std::string frame_type);
  bool sendNPCToGoalPoint(
    const std::string & name, const geometry_msgs::Pose pose, const bool wait_ready,
    const std::string frame_type);
  bool changeNPCVelocity(const std::string & name, const double velocity);
  bool changeNPCVelocityWithoutAccel(const std::string & name, const double velocity);
  bool changeNPCAccelMin(const std::string & name, const double accel);
  bool changeNPCAccelMax(const std::string & name, const double accel);
  bool changeNPCVelocityWithAccel(
    const std::string & name, const double velocity, const double accel);
  bool changeNPCConsiderVehicle(const std::string & name, const bool consider_ego_vehicle);
  bool changeNPCLaneChangeLeft(const std::string & name);
  bool changeNPCLaneChangeRight(const std::string & name);
  bool changeNPCLaneChange(const std::string & name, const int target_lane_id);
  bool changeNPCUturn(const std::string & name);
  bool changeNPCTurnLeft(const std::string & name);
  bool changeNPCTurnRight(const std::string & name);
  bool changeNPCNoTurn(const std::string & name);
  bool changeNPCIgnoreLane(const std::string & name);
  bool checkNPCFinishLaneChange(const std::string & name, bool & lane_change);
  bool checkNPCFinishVelocityChange(const std::string & name, bool & velocity_change);
  bool deleteNPC(const std::string & name);
  std::vector<std::string> getNpcList();

  //NPC API (tools)
  bool shiftNPCPose(
    const geometry_msgs::Pose & pose, const std::string frame_type,
    const npc_simulator::Object & obj, geometry_msgs::Pose * shift_pose);

  // traffic light API
  bool setTrafficLight(int traffic_id, std::string traffic_color);  // future work //TODO

private:
  ros::NodeHandle nh_;                   //!< @brief ros node handle
  ros::NodeHandle pnh_;                  //!< @brief private ros node handle
  ros::ServiceClient client_;            //!< @brief private ros service client
  ros::Publisher pub_object_info_;       //!< @brief topic pubscriber for npc
  ros::Publisher pub_simulator_engage_;  //!< @brief topic pubscriber for vehicle engage
  ros::Publisher pub_npc_engage_;        //!< @brief topic pubscriber for npc simulator engage
  ros::Timer timer_control_;             //!< @brief timer for getting self-position
  std::unordered_map<std::string, uuid_msgs::UniqueID> uuid_map_;
  std::unordered_map<std::string, double> maxacc_map_;
  std::unordered_map<std::string, double> minacc_map_;
  std::shared_ptr<NPCRouteManager> npc_route_manager_;

  void timerCallback(const ros::TimerEvent & te);
  void updateNPC();
  npc_simulator::Object getObjectMsg(
    uint32_t semantic_type, double confidence, uint8_t shape_type, double size_x, double size_y,
    double size_z);
  std::unordered_map<std::string, npc_simulator::Object> getNPCInfo();
  npc_simulator::Object getObjectMsg(std::string npc_name, std::string frame_id = "map");
  bool checkValidNPC(const std::string & name);
  bool changeNPCRoute(const std::string & name, const std::vector<int> route);
  bool targetLaneChangeNPC(const std::string & name, const int target_lane_id);
  bool laneChangeNPC(const std::string & name, const uint8_t lane_change_dir);
  bool changeNPCBehavior(const std::string & name, const uint8_t behavior_mode);
  bool inputNPCLaneFollowState(std::unordered_map<std::string, uint8_t> states);
  bool inputNPCStopState(std::unordered_map<std::string, bool> states);

  //parameter
  const double npc_stop_accel_ = 3.0;
  const double npc_default_accel_ = 1.0;
};

#endif  // SCENARIO_API_SCENARIO_API_SIMULATOR_H_INCLUDED
