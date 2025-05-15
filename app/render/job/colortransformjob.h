

#ifndef COLORTRANSFORMJOB_H
#define COLORTRANSFORMJOB_H

#include <QMatrix4x4>
#include <QString>
#include <utility>

#include "acceleratedjob.h"
#include "render/alphaassoc.h"
#include "render/colorprocessor.h"
#include "render/texture.h"

namespace olive {

class Node;

class ColorTransformJob : public AcceleratedJob {
 public:
  ColorTransformJob() {
    processor_ = nullptr;
    custom_shader_src_ = nullptr;
    input_alpha_association_ = kAlphaNone;
    clear_destination_ = true;
    force_opaque_ = false;
  }

  explicit ColorTransformJob(const NodeValueRow &row) : ColorTransformJob() { Insert(row); }

  [[nodiscard]] QString id() const {
    if (id_.isEmpty()) {
      return processor_->id();
    } else {
      return id_;
    }
  }

  void SetOverrideID(const QString &id) { id_ = id; }

  [[nodiscard]] const NodeValue &GetInputTexture() const { return input_texture_; }
  void SetInputTexture(const NodeValue &tex) { input_texture_ = tex; }
  void SetInputTexture(const TexturePtr &tex) {
    Q_ASSERT(!tex->IsDummy());
    input_texture_ = NodeValue(NodeValue::kTexture, tex);
  }

  [[nodiscard]] ColorProcessorPtr GetColorProcessor() const { return processor_; }
  void SetColorProcessor(ColorProcessorPtr p) { processor_ = std::move(p); }

  [[nodiscard]] const AlphaAssociated &GetInputAlphaAssociation() const { return input_alpha_association_; }
  void SetInputAlphaAssociation(const AlphaAssociated &e) { input_alpha_association_ = e; }

  [[nodiscard]] const Node *CustomShaderSource() const { return custom_shader_src_; }
  [[nodiscard]] const QString &CustomShaderID() const { return custom_shader_id_; }
  void SetNeedsCustomShader(const Node *node, const QString &id = QString()) {
    custom_shader_src_ = node;
    custom_shader_id_ = id;
  }

  [[nodiscard]] bool IsClearDestinationEnabled() const { return clear_destination_; }
  void SetClearDestinationEnabled(bool e) { clear_destination_ = e; }

  [[nodiscard]] const QMatrix4x4 &GetTransformMatrix() const { return matrix_; }
  void SetTransformMatrix(const QMatrix4x4 &m) { matrix_ = m; }

  [[nodiscard]] const QMatrix4x4 &GetCropMatrix() const { return crop_matrix_; }
  void SetCropMatrix(const QMatrix4x4 &m) { crop_matrix_ = m; }

  [[nodiscard]] const QString &GetFunctionName() const { return function_name_; }
  void SetFunctionName(const QString &function_name = QString()) { function_name_ = function_name; };

  [[nodiscard]] bool GetForceOpaque() const { return force_opaque_; }
  void SetForceOpaque(bool e) { force_opaque_ = e; }

 private:
  ColorProcessorPtr processor_;
  QString id_;

  NodeValue input_texture_;

  const Node *custom_shader_src_;
  QString custom_shader_id_;

  AlphaAssociated input_alpha_association_;

  bool clear_destination_;

  QMatrix4x4 matrix_;

  QMatrix4x4 crop_matrix_;

  QString function_name_;

  bool force_opaque_;
};

}  // namespace olive

#endif  // COLORTRANSFORMJOB_H
