#include <builtin_interfaces/msg/time.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <rclcpp/rclcpp.hpp>

class PoseRepublisher : public rclcpp::Node {
public:
  PoseRepublisher() : Node("pose_republisher") {
    // Subscribe to PoseStamped
    pose_subscriber_ = create_subscription<geometry_msgs::msg::PoseStamped>(
        "input_pose", 10,
        std::bind(&PoseRepublisher::poseCallback, this, std::placeholders::_1));

    // Subscribe to /clock
    clock_subscriber_ = create_subscription<builtin_interfaces::msg::Time>(
        "/clock", rclcpp::SensorDataQoS(),
        std::bind(&PoseRepublisher::clockCallback, this,
                  std::placeholders::_1));

    // Publisher for the modified PoseStamped
    pose_publisher_ =
        create_publisher<geometry_msgs::msg::PoseStamped>("goal_pose", 10);
  }

private:
  // Callback for PoseStamped messages
  void poseCallback(const geometry_msgs::msg::PoseStamped::SharedPtr msg) {
    RCLCPP_INFO(get_logger(), "Got pose!");
    auto modified_pose = *msg;
    modified_pose.header.stamp = get_clock()->now();
    pose_publisher_->publish(modified_pose);
    // if (last_clock_time_ != nullptr) {
    //   auto modified_pose = *msg;
    //   modified_pose.header.stamp =
    //       *last_clock_time_; // Update the timestamp with the latest clock
    //       time
    //   pose_publisher_->publish(modified_pose);
    // }
  }

  // Callback for /clock messages
  void clockCallback(const builtin_interfaces::msg::Time::SharedPtr msg) {
    RCLCPP_INFO(get_logger(), "Got clock!");
    last_clock_time_ = std::make_shared<builtin_interfaces::msg::Time>(
        *msg); // Store the latest clock time
  }

  rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr
      pose_subscriber_;
  rclcpp::Subscription<builtin_interfaces::msg::Time>::SharedPtr
      clock_subscriber_;
  rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr pose_publisher_;

  std::shared_ptr<builtin_interfaces::msg::Time>
      last_clock_time_; // To store the latest clock time
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PoseRepublisher>());
  rclcpp::shutdown();
  return 0;
}
