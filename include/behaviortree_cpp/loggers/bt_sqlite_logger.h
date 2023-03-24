#ifndef BT_SQLITE_LOGGER_H
#define BT_SQLITE_LOGGER_H

#include "behaviortree_cpp/loggers/abstract_logger.h"

namespace BT
{

class SqliteLogger : public StatusChangeLogger
{
public:
  SqliteLogger(const Tree &tree, const char* filename, bool append = false);

  virtual ~SqliteLogger() override;

  virtual void callback(Duration timestamp,
                        const TreeNode& node,
                        NodeStatus prev_status,
                        NodeStatus status) override;

  virtual void flush() override;

private:
  struct PimplDB;
  std::unique_ptr<PimplDB> p_;

  long monotonic_timestamp_ = 0;
  std::unordered_map<const BT::TreeNode*, long> starting_time_;
};


}

#endif // BT_SQLITE_LOGGER_H
