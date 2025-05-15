

#ifndef RENDERTEXTURE_H
#define RENDERTEXTURE_H

#include <QVariant>
#include <memory>
#include <utility>

#include "render/videoparams.h"

namespace olive {

class AcceleratedJob;
class Renderer;

class Texture;
using TexturePtr = std::shared_ptr<Texture>;

class Texture {
 public:
  enum Interpolation { kNearest, kLinear, kMipmappedLinear };

  static const Interpolation kDefaultInterpolation;

  /**
   * @brief Construct a dummy texture with no renderer backend
   */
  explicit Texture(VideoParams param) : renderer_(nullptr), params_(std::move(param)), job_(nullptr) {}

  template <typename T>
  Texture(const VideoParams& p, const T& j) : Texture(p) {
    job_ = new T(j);
  }

  /**
   * @brief Construct a real texture linked to a renderer backend
   */
  Texture(Renderer* renderer, QVariant native, VideoParams param)
      : renderer_(renderer), params_(std::move(param)), id_(std::move(native)), job_(nullptr) {}

  ~Texture();

  [[nodiscard]] QVariant id() const { return id_; }

  [[nodiscard]] const VideoParams& params() const { return params_; }

  template <typename T>
  static TexturePtr Job(const VideoParams& p, const T& j) {
    return std::make_shared<Texture>(p, j);
  }

  template <typename T>
  TexturePtr toJob(const T& job) {
    return Texture::Job(params_, job);
  }

  void Upload(void* data, int linesize);

  void Download(void* data, int linesize);

  [[nodiscard]] bool IsDummy() const { return !renderer_; }

  [[nodiscard]] int width() const { return params_.effective_width(); }

  [[nodiscard]] int height() const { return params_.effective_height(); }

  [[nodiscard]] QVector2D virtual_resolution() const {
    return {static_cast<float>(params_.square_pixel_width()), static_cast<float>(params_.height())};
  }

  [[nodiscard]] PixelFormat format() const { return params_.format(); }

  [[nodiscard]] int channel_count() const { return params_.channel_count(); }

  [[nodiscard]] int divider() const { return params_.divider(); }

  [[nodiscard]] const rational& pixel_aspect_ratio() const { return params_.pixel_aspect_ratio(); }

  [[nodiscard]] Renderer* renderer() const { return renderer_; }

  [[nodiscard]] bool IsJob() const { return job_; }
  [[nodiscard]] AcceleratedJob* job() const { return job_; }

 private:
  Renderer* renderer_;

  VideoParams params_;

  QVariant id_;

  AcceleratedJob* job_;
};

}  // namespace olive

Q_DECLARE_METATYPE(olive::TexturePtr)

#endif  // RENDERTEXTURE_H
