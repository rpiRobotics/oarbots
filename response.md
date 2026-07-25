Let me look at the relevant launch files and MoveIt config.## What "NO PLANNING LIBRARY LOADED" actually means

The *Context* tab of the MotionPlanning display fills its "Planning Library" dropdown by calling move_group's `query_planner_interface` service (and reading its pipeline parameters). You get that error whenever RViz **can't reach the move_group node's services** — either because move_group didn't load any planner, or because RViz is looking in the wrong namespace. You have a bit of both going on.

## Problem 1: Your RViz launch never tells the plugin where move_group is

In `oarbot_silver_moveit_rviz.launch.py`:

- `"move_group_namespace"` **is not a real RViz/MoveIt parameter** — the MotionPlanning display has a *display property* called **Move Group Namespace** (set in the `.rviz` file or in the Displays panel), not a node parameter. Setting it as an rviz2 node parameter silently does nothing.
- You never pass `-d .../moveit.rviz`, so RViz starts empty and any MotionPlanning display you add manually defaults to the **root** namespace — where there is no move_group, hence the error.
- The `('/tf', '/tf')` remaps are no-ops. Because rviz2 is in the `oarbot_silver` namespace, it actually subscribes to `/oarbot_silver/tf`. If your robot_state_publisher publishes on the global `/tf`, the match side must be relative: `('tf', '/tf')`.

Fix the launch file:

```python
from launch import LaunchDescription
from launch.substitutions import PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from moveit_configs_utils import MoveItConfigsBuilder

def generate_launch_description() -> LaunchDescription:
    moveit_config = MoveItConfigsBuilder("oarbot_silver", package_name="oarbot_silver_moveit").to_moveit_configs()
    rviz_config = PathJoinSubstitution([
        FindPackageShare("oarbot_silver_moveit"), "config", "moveit.rviz",
    ])
    return LaunchDescription([
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            namespace='oarbot_silver',
            arguments=['-d', rviz_config],
            parameters=[
                moveit_config.robot_description,
                moveit_config.robot_description_semantic,
                moveit_config.robot_description_kinematics,
                moveit_config.planning_pipelines,
                moveit_config.joint_limits,
            ],
            remappings=[
                ('tf', '/tf'),
                ('tf_static', '/tf_static'),
            ],
        )
    ])
```


And set the namespace in the saved RViz config so the display points at `/oarbot_silver/move_group`:

```yaml
# ... existing code ...
    - Class: moveit_rviz_plugin/MotionPlanning
      Name: MotionPlanning
      Move Group Namespace: /oarbot_silver
      Planned Path:
        Loop Animation: true
# ... existing code ...
```


Note that move_group itself (launched with `PushRosNamespace`) also uses namespaced `tf`/`tf_static`. If your state publisher broadcasts on global `/tf`, add the same relative remaps in `oarbot_silver_moveit.launch.py`, otherwise move_group won't see the robot's transforms either.

## Problem 2: It fails even without namespaces → check move_group itself

If launching the stock `move_group.launch.py` + `moveit_rviz.launch.py` (which wire everything up correctly, including the `-d` flag) *still* shows the error, then move_group is not offering any planners. Debug it directly:

1. Watch move_group's console output. You should see a line like `Using planning interface 'OMPL'`. Errors such as *"Failed to load planner plugin"* or *"Parameter 'planning_plugins' not declared"* pinpoint the cause.
2. Verify the service exists:
```shell script
ros2 service list | grep query_planner_interface
   ros2 param get /move_group planning_pipelines   # or /oarbot_silver/move_group
```

3. Make sure the OMPL planner plugin is actually installed — `moveit_planners` is only an `exec_depend`, so on a slim install it can be missing:
```shell script
sudo apt install ros-jazzy-moveit-planners-ompl
```

   A missing/failed pluginlib load of `ompl_interface/OMPLPlanner` is the classic cause of an empty planner list [[2]](https://github.com/moveit/moveit2/issues/1782)[[7]](https://github.com/moveit/moveit2_tutorials/issues/670).
4. Also confirm move_group didn't die earlier during startup (e.g., xacro/SRDF parsing) — if the node crashes, RViz shows the same message.

Your `ompl_planning.yaml` itself is fine for Jazzy (it already uses the new `planning_plugins:` list and `default_planning_request_adapters/...` names), so I wouldn't suspect the config file format.

## Summary

- The namespaced setup fails because the MotionPlanning display defaults to the root namespace; fix it with the **Move Group Namespace** property in `moveit.rviz` and load that config with `-d`.
- The non-namespaced test failing points to move_group not exposing planners at all — check its log for OMPL plugin errors and confirm `ros-jazzy-moveit-planners-ompl` is installed.
- Fix the `tf` remappings (relative match side) so namespaced nodes see the global `/tf`, or the robot model/planning scene will misbehave once planning works.