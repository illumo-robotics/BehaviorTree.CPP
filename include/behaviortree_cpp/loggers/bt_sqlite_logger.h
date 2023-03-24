#ifndef BT_SQLITE_LOGGER_H
#define BT_SQLITE_LOGGER_H

#include <filesystem>
#include "behaviortree_cpp/loggers/abstract_logger.h"

namespace sqlite{
class Connection;
}

namespace BT
{

class SqliteLogger : public StatusChangeLogger
{
public:
  SqliteLogger(const Tree &tree,
               std::filesystem::path const& file,
               bool append = false);

  virtual ~SqliteLogger() override;

  virtual void callback(Duration timestamp,
                        const TreeNode& node,
                        NodeStatus prev_status,
                        NodeStatus status) override;

  virtual void flush() override;

private:
  std::unique_ptr<sqlite::Connection> db_;

  long monotonic_timestamp_ = 0;
  std::unordered_map<const BT::TreeNode*, long> starting_time_;

  int session_id_ = -1;

};


}

#endif // BT_SQLITE_LOGGER_H
