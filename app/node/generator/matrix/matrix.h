

#ifndef MATRIXGENERATOR_H
#define MATRIXGENERATOR_H

#include <QVector2D>

#include "node/inputdragger.h"
#include "node/node.h"

namespace olive {

class MatrixGenerator : public Node {
  Q_OBJECT
 public:
  MatrixGenerator();

  NODE_DEFAULT_FUNCTIONS(MatrixGenerator)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString ShortName() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  static const QString kPositionInput;
  static const QString kRotationInput;
  static const QString kScaleInput;
  static const QString kUniformScaleInput;
  static const QString kAnchorInput;

 protected:
  [[nodiscard]] static QMatrix4x4 GenerateMatrix(const NodeValueRow &value, bool ignore_anchor, bool ignore_position,
                                                 bool ignore_scale, const QMatrix4x4 &mat);
  static QMatrix4x4 GenerateMatrix(const QVector2D &pos, const float &rot, const QVector2D &scale, bool uniform_scale,
                                   const QVector2D &anchor, QMatrix4x4 mat);

  void InputValueChangedEvent(const QString &input, int element) override;
};

}  // namespace olive

#endif  // TRANSFORMDISTORT_H
