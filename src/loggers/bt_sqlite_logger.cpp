#include "behaviortree_cpp/loggers/bt_sqlite_logger.h"

namespace BT {
/*
struct SqliteLogger::PimplDB
{
  PimplDB(const char *filename): db(filename, SQLite::OPEN_READWRITE |SQLite::OPEN_CREATE)
  {}

  SQLite::Database db;
};

SqliteLogger::SqliteLogger(const Tree &tree,
                           const char *filename,
                           bool append):
      StatusChangeLogger(tree.rootNode()),
      p_(new SqliteLogger::PimplDB(filename))
{
  enableTransitionToIdle(true);

  p_->db.exec("CREATE TABLE IF NOT EXISTS Transitions ("
              "timestamp  INTEGER PRIMARY KEY, "
              "uid        INTEGER, "
              "duration   INTEGER, "
              "prev_state TEXT NOT NULL, "
              "state      TEXT NOT NULL)");

  p_->db.exec("CREATE TABLE IF NOT EXISTS Nodes ("
              "uid        INTEGER PRIMARY KEY UNIQUE, "
              "node_type  TEXT NOT NULL, "
              "node_name  TEXT NOT NULL, "
              "instance   TEXT)");

  p_->db.exec("PRAGMA journal_mode=WAL");

  if( !append )
  {
    p_->db.exec("DELETE from Transitions;");
  }
  p_->db.exec("DELETE from Nodes;");

  for(const auto& node: tree.nodes)
  {
    SQLite::Statement   query(p_->db, "INSERT INTO Nodes VALUES (?, ?, ?, ?)");
    query.bind(1, node->UID());
    query.bind(2, toStr(node->type()));
    query.bind(3, node->registrationName());
    query.bind(4, node->name());
    query.exec();
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

  auto ToChar = [](NodeStatus stat) ->const char *
  {
    if( stat == NodeStatus::RUNNING ) return "R";
    if( stat == NodeStatus::SUCCESS ) return "S";
    if( stat == NodeStatus::FAILURE ) return "F";
    return "I";
  };

  SQLite::Statement query(p_->db, "INSERT INTO Transitions VALUES (?, ?, ?, ?, ?)");
  query.bind(1, monotonic_timestamp_);
  query.bind(2, node.UID());
  query.bind(3, elapsed_time);
  query.bind(4, ToChar(prev_status));
  query.bind(5, ToChar(status));

  query.exec();

}

void SqliteLogger::flush()
{
  p_->db.flush();
}*/

}
