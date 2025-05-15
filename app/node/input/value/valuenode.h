#ifndef VALUENODE_H
#define VALUENODE_H

#include "node/node.h"

namespace olive {

class ValueNode : public Node {
  Q_OBJECT
 public:
  ValueNode();

  NODE_DEFAULT_FUNCTIONS(ValueNode)

  [[nodiscard]] QString Name() const override { return tr("Value"); }

  [[nodiscard]] QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.value"); }

  [[nodiscard]] QVector<CategoryID> Category() const override { return {kCategoryGenerator}; }

  [[nodiscard]] QString Description() const override {
    return tr("Create a single value that can be connected to various other inputs.");
  }

  static const QString kTypeInput;
  static const QString kValueInput;

  void Retranslate() override;

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

 protected:
  void InputValueChangedEvent(const QString &input, int element) override;

 private:
  static const QVector<NodeValue::Type> kSupportedTypes;
};

}  // namespace olive

#endif  // VALUENODE_H
