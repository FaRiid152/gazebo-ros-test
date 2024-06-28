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

class GazeboBoxSpawner : public rclcpp::Node
{
public:
  GazeboBoxSpawner() : Node("gazebo_box_spawner")
  {
    // Create client for the spawn_entity service
    spawnServiceClient_ = this->create_client<gazebo_msgs::srv::SpawnEntity>("spawn_entity");

    // Create publisher for the box model state
    // boxStatePublisher_ = this->create_publisher<gazebo_msgs::msg::ModelState>("box_model_state", 10);

    // Subscribe to the /gazebo/model_states topic
    //modelStatesSubscriber_ = this->create_subscription<gazebo_msgs::msg::ModelStates>(
    //  "/gazebo/model_states", 10, std::bind(&GazeboBoxSpawner::modelStatesCallback, this, std::placeholders::_1));

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

    // Resolve the path to the model.sdf file using ament_index_cpp
    std::string package_share_directory = ament_index_cpp::get_package_share_directory("box");
    std::string sdf_file_path = package_share_directory + "/models/model.sdf";
    RCLCPP_INFO(this->get_logger(), "Resolved model.sdf path: %s", sdf_file_path.c_str());

    // Read the SDF file
    std::ifstream sdfFile(sdf_file_path);
    if (!sdfFile)
    {
      RCLCPP_ERROR(this->get_logger(), "Failed to open model.sdf file at %s.", sdf_file_path.c_str());
      return;
    }
    std::stringstream sdfBuffer;
    sdfBuffer << sdfFile.rdbuf();
    std::string sdfContent = sdfBuffer.str();
    RCLCPP_INFO(this->get_logger(), "Successfully read model.sdf file.");

    // Create a request to spawn the box model
    auto request = std::make_shared<gazebo_msgs::srv::SpawnEntity::Request>();
    request->name = "box_model";
    request->xml = sdfContent;

    // Send the request asynchronously
    auto resultFuture = spawnServiceClient_->async_send_request(request);

    // Wait for the result
    if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), resultFuture) != rclcpp::FutureReturnCode::SUCCESS)
    {
      RCLCPP_ERROR(this->get_logger(), "Failed to spawn the box model.");
      return;
    }

    RCLCPP_INFO(this->get_logger(), "Box model spawned successfully.");
  }

private:
  /*
  void modelStatesCallback(const gazebo_msgs::msg::ModelStates::SharedPtr msg)
  {
    auto it = std::find(msg->name.begin(), msg->name.end(), "box_model");
    if (it != msg->name.end())
    {
      size_t index = std::distance(msg->name.begin(), it);
      auto boxState = std::make_shared<gazebo_msgs::msg::ModelState>();
      boxState->model_name = "box_model";
      boxState->pose = msg->pose[index];
      boxState->twist = msg->twist[index];
      RCLCPP_INFO(this->get_logger(), "Publishing state for box_model: pose=(%f, %f, %f) orientation=(%f, %f, %f, %f)",
                  boxState->pose.position.x, boxState->pose.position.y, boxState->pose.position.z,
                  boxState->pose.orientation.x, boxState->pose.orientation.y, boxState->pose.orientation.z, boxState->pose.orientation.w);
      boxStatePublisher_->publish(*boxState);
    }
  }
  */
  rclcpp::Client<gazebo_msgs::srv::SpawnEntity>::SharedPtr spawnServiceClient_;
  //rclcpp::Publisher<gazebo_msgs::msg::ModelState>::SharedPtr boxStatePublisher_;
  //rclcpp::Subscription<gazebo_msgs::msg::ModelStates>::SharedPtr modelStatesSubscriber_;
};

int main(int argc, char **argv)
{
  // Initialize ROS 2
  rclcpp::init(argc, argv);

  // Create and run the node
  auto node = std::make_shared<GazeboBoxSpawner>();
  rclcpp::spin(node);

  // Shutdown ROS 2
  rclcpp::shutdown();
  return 0;
}
