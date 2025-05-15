

#ifndef SHADERJOB_H
#define SHADERJOB_H

#include <QMatrix4x4>
#include <QVector>

#include "acceleratedjob.h"
#include "render/texture.h"

namespace olive {

class ShaderJob : public AcceleratedJob {
 public:
  ShaderJob() {
    iterations_ = 1;
    iterative_input_ = nullptr;
  }

  explicit ShaderJob(const NodeValueRow& row) : ShaderJob() { Insert(row); }

  [[nodiscard]] const QString& GetShaderID() const { return shader_id_; }

  void SetShaderID(const QString& id) { shader_id_ = id; }

  void SetIterations(int iterations, const NodeInput& iterative_input) {
    SetIterations(iterations, iterative_input.input());
  }

  void SetIterations(int iterations, const QString& iterative_input) {
    iterations_ = iterations;
    iterative_input_ = iterative_input;
  }

  [[nodiscard]] int GetIterationCount() const { return iterations_; }

  [[nodiscard]] const QString& GetIterativeInput() const { return iterative_input_; }

  [[nodiscard]] Texture::Interpolation GetInterpolation(const QString& id) const {
    return interpolation_.value(id, Texture::kDefaultInterpolation);
  }

  [[nodiscard]] const QHash<QString, Texture::Interpolation>& GetInterpolationMap() const { return interpolation_; }

  void SetInterpolation(const NodeInput& input, Texture::Interpolation interp) {
    interpolation_.insert(input.input(), interp);
  }

  void SetInterpolation(const QString& id, Texture::Interpolation interp) { interpolation_.insert(id, interp); }

  void SetVertexCoordinates(const QVector<float>& vertex_coords) { vertex_overrides_ = vertex_coords; }

  const QVector<float>& GetVertexCoordinates() { return vertex_overrides_; }

 private:
  QString shader_id_;

  int iterations_;

  QString iterative_input_;

  QHash<QString, Texture::Interpolation> interpolation_;

  QVector<float> vertex_overrides_;
};

}  // namespace olive

#endif  // SHADERJOB_H
