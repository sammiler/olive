#ifndef COLORPROCESSOR_H
#define COLORPROCESSOR_H

#include "codec/frame.h"
#include "common/ocioutils.h"
#include "render/colortransform.h"

namespace olive {

class ColorManager;

class ColorProcessor;
using ColorProcessorPtr = std::shared_ptr<ColorProcessor>;

class ColorProcessor {
 public:
  enum Direction { kNormal, kInverse };

  ColorProcessor(ColorManager* config, const QString& input, const ColorTransform& transform,
                 Direction direction = kNormal);
  explicit ColorProcessor(OCIO::ConstProcessorRcPtr processor);

  DISABLE_COPY_MOVE(ColorProcessor)

  static ColorProcessorPtr Create(ColorManager* config, const QString& input, const ColorTransform& transform,
                                  Direction direction = kNormal);
  static ColorProcessorPtr Create(const OCIO::ConstProcessorRcPtr& processor);

  OCIO::ConstProcessorRcPtr GetProcessor();

  void ConvertFrame(const FramePtr& f);
  void ConvertFrame(Frame* f);

  Color ConvertColor(const Color& in);

  [[nodiscard]] const char* id() const { return processor_->getCacheID(); }

 private:
  OCIO::ConstProcessorRcPtr processor_;

  OCIO::ConstCPUProcessorRcPtr cpu_processor_;
};

using ColorProcessorChain = QVector<ColorProcessorPtr>;

}  // namespace olive

Q_DECLARE_METATYPE(olive::ColorProcessorPtr)

#endif  // COLORPROCESSOR_H
