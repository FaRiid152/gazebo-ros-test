#include <gazebo_msgs/srv/spawn_entity.hpp>
#include <gazebo_msgs/msg/model_state.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <rclcpp/rclcpp.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <gazebo_msgs/msg/model_states.hpp>


using namespace std::chrono_literals;

class GazeboCameraSpawner : public rclcpp::Node
{
public:
  GazeboCameraSpawner() : Node("gazebo_camera_spawner")
  {
    // Create client for the spawn_entity service
    spawnServiceClient_ = this->create_client<gazebo_msgs::srv::SpawnEntity>("spawn_entity");

    // Wait for Gazebo to be available
    while (!spawnServiceClient_->wait_for_service(1s))
    {
      if (!rclcpp::ok())
      {
        RCLCPP_ERROR(this->get_logger(), "Interrupted while waiting for the service. Exiting.");
        return;
      }
      RCLCPP_INFO(this->get_logger(), "Waiting for Gazebo...");
    }

    // Resolve the path to the URDF file using ament_index_cpp
    std::string package_share_directory = ament_index_cpp::get_package_share_directory("box");
    std::string camera_file_path = package_share_directory + "/models/zed_camera.urdf";
    RCLCPP_INFO(this->get_logger(), "Resolved zed_camera.urdf path: %s", camera_file_path.c_str());

    // Read the URDF file
    std::ifstream urdfFile(camera_file_path);
    if (!urdfFile)
    {
      RCLCPP_ERROR(this->get_logger(), "Failed to open zed_camera.urdf file at %s.", camera_file_path.c_str());
      return;
    }
    std::stringstream urdfBuffer;
    urdfBuffer << urdfFile.rdbuf();
    std::string urdfContent = urdfBuffer.str();
    RCLCPP_INFO(this->get_logger(), "Successfully read zed_camera.urdf file.");

    // Create a request to spawn the ZED camera model
    auto request = std::make_shared<gazebo_msgs::srv::SpawnEntity::Request>();
    request->name = "zed_camera";
    request->xml = urdfContent;
    request->robot_namespace = "";
    request->reference_frame = "world"; // Change this to the appropriate reference frame

    // Send the request asynchronously
    auto resultFuture = spawnServiceClient_->async_send_request(request);

    // Wait for the result
    if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), resultFuture) != rclcpp::FutureReturnCode::SUCCESS)
    {
      RCLCPP_ERROR(this->get_logger(), "Failed to spawn the ZED camera model.");
      return;
    }

    RCLCPP_INFO(this->get_logger(), "ZED camera model spawned successfully.");
  }

private:
  rclcpp::Client<gazebo_msgs::srv::SpawnEntity>::SharedPtr spawnServiceClient_;
};

int main(int argc, char **argv)
{
  // Initialize ROS 2
  rclcpp::init(argc, argv);

  // Create and run the node
  auto node = std::make_shared<GazeboCameraSpawner>();
  rclcpp::spin(node);

  // Shutdown ROS 2
  rclcpp::shutdown();
  return 0;
}