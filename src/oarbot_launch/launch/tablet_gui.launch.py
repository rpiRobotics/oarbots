from launch import LaunchDescription
from launch.actions import RegisterEventHandler, EmitEvent
from launch.event_handlers import OnProcessExit
from launch.events import Shutdown
from launch_ros.actions import Node


def generate_launch_description() -> LaunchDescription:

    tablet_gui_node = Node(
        package="tablet_gui",
        executable="tablet_gui",
        name="tablet_gui",
        output="screen",
        namespace="tablet_gui"
    )

    return LaunchDescription([
        tablet_gui_node,
        Node(
            package="spacenav",
            executable="spacenav_node",
            name="spacenav_node",
            namespace="tablet_gui",
        ),
        RegisterEventHandler(
            OnProcessExit(
                target_action=tablet_gui_node,
                on_exit=[
                    EmitEvent(event=Shutdown(
                        reason="tablet_gui exited"
                    ))
                ]
            )
        )
    ])