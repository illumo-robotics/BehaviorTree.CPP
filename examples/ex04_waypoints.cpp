#include "behaviortree_cpp/bt_factory.h"
#include "behaviortree_cpp/decorators/loop_node.h"
#include "behaviortree_cpp/loggers/bt_cout_logger.h"
#include <list>

using namespace BT;

/*
 * In this example we will show how a common design pattern could be implemented.
 * We want to iterate through the elements of a queue, for instance a list of waypoints.
 *
 * Two ways to create a "loop" are presented, one using the actions "QueueSize" and "PopFromQueue"
 * and the other using the decorator "ConsumeQueue".
 */

struct Pose2D
{
  double x, y, theta;
};

/**
 * @brief Dummy action that generates a list of poses.
 */
class GenerateWaypoints : public SyncActionNode
{
public:
  GenerateWaypoints(const std::string& name, const NodeConfig& config) :
    SyncActionNode(name, config)
  {}

  NodeStatus tick() override
  {
    SharedQueue<Pose2D> shared_queue = std::make_shared<Queue<Pose2D>>();
    std::lock_guard lk(shared_queue->mutex);

    for (int i = 0; i < 5; i++)
    {
      shared_queue->queue.push_back(Pose2D{double(i), double(i), 0});
    }
    setOutput("waypoints", shared_queue);
    return NodeStatus::SUCCESS;
  }

  static PortsList providedPorts()
  {
    return {OutputPort<SharedQueue<Pose2D>>("waypoints")};
  }
};
//--------------------------------------------------------------
class PrintNumber : public SyncActionNode
{
  public:
  PrintNumber(const std::string& name, const NodeConfig& config) :
        SyncActionNode(name, config)
  {}

  NodeStatus tick() override
  {
    double value;
    if (getInput("value", value)) {
      std::cout << "PrintNumber: " << value << "\n";
      return NodeStatus::SUCCESS;
    }
    return NodeStatus::FAILURE;
  }

  static PortsList providedPorts()
  {
    return {InputPort<double>("value")};
  }
};

//--------------------------------------------------------------

/**
 * @brief Simple Action that uses the output of PopFromQueue<Pose2D> or ConsumeQueue<Pose2D>
 */
class UseWaypoint : public ThreadedAction
{
public:
  UseWaypoint(const std::string& name, const NodeConfig& config) :
    ThreadedAction(name, config)
  {}

  NodeStatus tick() override
  {
    Pose2D wp;
    if (getInput("waypoint", wp))
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      std::cout << "Using waypoint: " << wp.x << "/" << wp.y << std::endl;
      return NodeStatus::SUCCESS;
    }
    else
    {
      return NodeStatus::FAILURE;
    }
  }

  static PortsList providedPorts()
  {
    return {InputPort<Pose2D>("waypoint")};
  }
};

// clang-format off
static const char* xml_tree = R"(
 <root BTCPP_format="4" >
     <BehaviorTree ID="TreeA">
        <Sequence>
            <LoopDoubleQueue queue="1;2;3"  value="{number}">
              <PrintNumber value="{number}" />
            </LoopDoubleQueue>

            <GenerateWaypoints waypoints="{waypoints}" />
            <LoopPoseQueue queue="{waypoints}"  value="{wp}">
              <UseWaypoint waypoint="{wp}" />
            </LoopPoseQueue>
        </Sequence>
     </BehaviorTree>
 </root>
 )";

// clang-format on

int main()
{
  BehaviorTreeFactory factory;

  factory.registerNodeType<LoopNode<Pose2D>>("LoopPoseQueue");
  factory.registerNodeType<LoopNode<double>>("LoopDoubleQueue");

  factory.registerNodeType<UseWaypoint>("UseWaypoint");
  factory.registerNodeType<PrintNumber>("PrintNumber");
  factory.registerNodeType<GenerateWaypoints>("GenerateWaypoints");

  auto tree = factory.createTreeFromText(xml_tree);

  StdCoutLogger logger(tree);
  logger.enableTransitionToIdle(false);

  tree.tickWhileRunning();

  return 0;
}
