#include "behaviortree_cpp/loggers/bt_sqlite_logger.h"
#include "behaviortree_cpp/loggers/contrib/sqlite.hpp"
#include "behaviortree_cpp/xml_parsing.h"

namespace BT {


SqliteLogger::SqliteLogger(const Tree &tree,
                           std::filesystem::path const& file,
                           bool append):
  StatusChangeLogger(tree.rootNode())
{
  enableTransitionToIdle(true);

  db_ = std::make_unique<sqlite::Connection>(file.string());

  sqlite::Statement(*db_,
                    "CREATE TABLE IF NOT EXISTS Transitions ("
                    "timestamp  INTEGER PRIMARY KEY NOT NULL, "
                    "session_id INTEGER NOT NULL, "
                    "uid        INTEGER NOT NULL, "
                    "duration   INTEGER, "
                    "state      INTEGER NOT NULL);");

  sqlite::Statement(*db_,
                    "CREATE TABLE IF NOT EXISTS Definitions ("
                    "session_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "date       TEXT NOT NULL,"
                    "xml_tree   TEXT NOT NULL);");

  sqlite::Statement(*db_, "PRAGMA journal_mode=WAL;");

  if( !append )
  {
    sqlite::Statement(*db_, "DELETE from Transitions;");
    sqlite::Statement(*db_, "DELETE from Definitions;");
  }

  auto tree_xml = WriteTreeToXML(tree, true);
  sqlite::Statement(*db_,
                    "INSERT into Definitions (date, xml_tree) "
                    "VALUES (datetime('now','localtime'),?);",
                    tree_xml);

  auto res = sqlite::Query(*db_, "SELECT MAX(session_id) FROM Definitions LIMIT 1;");

  while(res.Next())
  {
    session_id_ = res.Get(0);
  }
}

SqliteLogger::~SqliteLogger()
{
  flush();
}

void SqliteLogger::callback(Duration timestamp,
                            const TreeNode &node,
                            NodeStatus prev_status,
                            NodeStatus status)
{

  using namespace std::chrono;
  auto tm_usec = duration_cast<microseconds>(timestamp).count();
  monotonic_timestamp_ = std::max( monotonic_timestamp_ + 1, tm_usec);

  long elapsed_time = 0;

  if( prev_status == NodeStatus::IDLE && status == NodeStatus::RUNNING )
  {
    starting_time_[&node] = monotonic_timestamp_;
  }

  if( prev_status == NodeStatus::RUNNING && status != NodeStatus::RUNNING )
  {
    elapsed_time = monotonic_timestamp_;
    auto it = starting_time_.find(&node);
    if( it != starting_time_.end() )
    {
      elapsed_time -= it->second;
    }
  }

  sqlite::Statement(*db_,
                    "INSERT INTO Transitions VALUES (?, ?, ?, ?, ?)",
                    monotonic_timestamp_,
                    session_id_,
                    node.UID(),
                    elapsed_time,
                    static_cast<int>(status));
}

void SqliteLogger::flush()
{
}


}
