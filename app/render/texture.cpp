#include "texture.h"

#include "renderer.h"

namespace olive {

const Texture::Interpolation Texture::kDefaultInterpolation = Texture::kMipmappedLinear;

Texture::~Texture() {
  if (renderer_) {
    renderer_->DestroyTexture(this);
  }

  delete job_;
}

void Texture::Upload(void *data, int linesize) {
  if (renderer_) {
    renderer_->UploadToTexture(this->id(), this->params(), data, linesize);
  }
}

void Texture::Download(void *data, int linesize) {
  if (renderer_) {
    renderer_->DownloadFromTexture(this->id(), this->params(), data, linesize);
  }
}

}  // namespace olive
