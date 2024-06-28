import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node

def generate_launch_description():
    package_name = 'box'
    world_file_name = 'empty.world'
    world = os.path.join(get_package_share_directory(package_name), 'models', world_file_name)

    return LaunchDescription([
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource([os.path.join(
                get_package_share_directory('gazebo_ros'), 'launch', 'gazebo.launch.py'
            )]),
            launch_arguments={'world': world}.items(),
        ),

        Node(
            package='box',
            executable='spawn_box',
            name='spawn_box',
            output='screen'
        )
    ])