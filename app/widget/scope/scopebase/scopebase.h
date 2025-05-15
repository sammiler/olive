#ifndef SCOPEBASE_H
#define SCOPEBASE_H

#include "codec/frame.h"
#include "render/colorprocessor.h"
#include "widget/manageddisplay/manageddisplay.h"

namespace olive {

class ScopeBase : public ManagedDisplayWidget {
 public:
  explicit ScopeBase(QWidget* parent = nullptr);

  MANAGEDDISPLAYWIDGET_DEFAULT_DESTRUCTOR(ScopeBase)

 public slots:
  void SetBuffer(TexturePtr frame);

 protected slots:
  void OnInit() override;

  void OnPaint() override;

  void OnDestroy() override;

 protected:
  void showEvent(QShowEvent* e) override;

  virtual ShaderCode GenerateShaderCode() = 0;

  /**
   * @brief Draw function
   *
   * Override this if your sub-class scope needs extra drawing.
   */
  virtual void DrawScope(TexturePtr managed_tex, QVariant pipeline);

 private:
  QVariant pipeline_;

  TexturePtr texture_;

  TexturePtr managed_tex_;

  bool managed_tex_up_to_date_;
};

}  // namespace olive

#endif  // SCOPEBASE_H
