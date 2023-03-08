#pragma once

#include <array>
#include <future>
#include "behaviortree_cpp/loggers/abstract_logger.h"

namespace BT
{
class Groot2Publisher : public StatusChangeLogger
{
  static std::mutex used_ports_mutex;
  static std::set<unsigned> used_ports;

  public:
  Groot2Publisher(const BT::Tree& tree, unsigned server_port = 1667);

  ~Groot2Publisher() override;

  private:

  struct Breakpoint
  {
    using Ptr = std::shared_ptr<Breakpoint>;

    // used to enable/disable the breakpoint
    bool enabled = true;

    uint16_t node_uid = 0;

    // interactive breakpoints are unblucked using unlockBreakpoint()
    bool is_interactive = true;

    // used by interactive breakpoints to wait for unlocking
    std::condition_variable wakeup;

    std::mutex mutex;

    // set to true to unlock an interactive breakpoint
    bool ready = false;

    // once finished self-destroy
    bool remove_when_done = false;

    // result to be returned
    NodeStatus desired_result = NodeStatus::SKIPPED;
  };

  void callback(Duration timestamp,
                const TreeNode& node,
                NodeStatus prev_status,
                NodeStatus status) override;

  void flush() override;

  void serverLoop();

  void heartbeatLoop();

  void updateStatusBuffer();

  std::vector<uint8_t> generateBlackboardsDump(const std::string& bb_list);

  bool insertBreakpoint(std::shared_ptr<Breakpoint> breakpoint);

  bool unlockBreakpoint(uint16_t node_uid, NodeStatus result, bool remove);

  bool removeBreakpoint(uint16_t node_uid);

  void removeAllBreakpoints();

  Breakpoint::Ptr getBreakpoint(uint16_t node_uid);

  unsigned server_port_ = 0;
  std::string server_address_;
  std::string publisher_address_;

  std::string tree_xml_;

  std::atomic_bool active_server_;
  std::thread server_thread_;

  std::mutex status_mutex_;

  std::unordered_map<uint16_t, char*> buffer_ptr_;
  std::string status_buffer_;

  // weak reference to the tree.
  std::unordered_map<std::string, std::weak_ptr<BT::Tree::Subtree>> subtrees_;
  std::unordered_map<uint16_t, std::weak_ptr<BT::TreeNode>> nodes_by_uid_;

  std::mutex breakpoints_map_mutex_;
  std::unordered_map<uint16_t, std::shared_ptr<Breakpoint>> pre_breakpoints_;


  std::chrono::system_clock::time_point last_heartbeat_;

  std::thread heartbeat_thread_;

  struct Pimpl;
  Pimpl* zmq_;
};
}   // namespace BT

