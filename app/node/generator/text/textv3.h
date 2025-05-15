

#ifndef TEXTGENERATORV3_H
#define TEXTGENERATORV3_H

#include "node/generator/shape/shapenodebase.h"
#include "node/gizmo/text.h"

namespace olive {

class TextGeneratorV3 : public ShapeNodeBase {
  Q_OBJECT
 public:
  TextGeneratorV3();

  NODE_DEFAULT_FUNCTIONS(TextGeneratorV3)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  void GenerateFrame(FramePtr frame, const GenerateJob &job) const override;

  void UpdateGizmoPositions(const NodeValueRow &row, const NodeGlobals &globals) override;

  enum VerticalAlignment { kVAlignTop, kVAlignMiddle, kVAlignBottom };

  [[nodiscard]] VerticalAlignment GetVerticalAlignment() const {
    return static_cast<VerticalAlignment>(GetStandardValue(kVerticalAlignmentInput).toInt());
  }

  static Qt::Alignment GetQtAlignmentFromOurs(VerticalAlignment v);
  static VerticalAlignment GetOurAlignmentFromQts(Qt::Alignment v);

  static const QString kTextInput;
  static const QString kVerticalAlignmentInput;
  static const QString kUseArgsInput;
  static const QString kArgsInput;

  static QString FormatString(const QString &input, const QStringList &args);

 protected:
  void InputValueChangedEvent(const QString &input, int element) override;

 private:
  TextGizmo *text_gizmo_;

  bool dont_emit_valign_;

 private slots:
  void GizmoActivated();
  void GizmoDeactivated();
  void SetVerticalAlignmentUndoable(Qt::Alignment a);
};

}  // namespace olive

#endif  // TEXTGENERATORV3_H
