#ifndef NODEVALUEDATABASE_H
#define NODEVALUEDATABASE_H

#include "param.h"
#include "value.h"

namespace olive {

class NodeValueDatabase {
 public:
  NodeValueDatabase() = default;

  NodeValueTable& operator[](const QString& input_id) { return tables_[input_id]; }

  void Insert(const QString& key, const NodeValueTable& value) { tables_.insert(key, value); }

  NodeValueTable Take(const QString& key) { return tables_.take(key); }

  [[nodiscard]] NodeValueTable Merge() const;

  using Tables = QHash<QString, NodeValueTable>;
  using const_iterator = Tables::const_iterator;
  using iterator = Tables::iterator;

  [[nodiscard]] inline const_iterator cbegin() const { return tables_.cbegin(); }

  [[nodiscard]] inline const_iterator cend() const { return tables_.cend(); }

  inline iterator begin() { return tables_.begin(); }

  inline iterator end() { return tables_.end(); }

  [[nodiscard]] inline bool contains(const QString& s) const { return tables_.contains(s); }

 private:
  Tables tables_;
};

}  // namespace olive

Q_DECLARE_METATYPE(olive::NodeValueDatabase)

#endif  // NODEVALUEDATABASE_H
