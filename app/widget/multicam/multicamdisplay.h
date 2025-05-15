#ifndef MULTICAMDISPLAY_H
#define MULTICAMDISPLAY_H

#include "node/input/multicam/multicamnode.h"
#include "widget/viewer/viewerdisplay.h"

namespace olive {

class MulticamDisplay : public ViewerDisplayWidget {
  Q_OBJECT
 public:
  explicit MulticamDisplay(QWidget *parent = nullptr);

  void SetMulticamNode(MultiCamNode *n);

 protected:
  void OnPaint() override;

  void OnDestroy() override;

  TexturePtr LoadCustomTextureFromFrame(const QVariant &v) override;

 private:
  static QString GenerateShaderCode(int rows, int cols);

  MultiCamNode *node_;

  QVariant shader_;
  int rows_;
  int cols_;
};

}  // namespace olive

#endif  // MULTICAMDISPLAY_H
