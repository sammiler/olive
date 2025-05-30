#include "colorprocessor.h"

#include <utility>

#include "common/define.h"
#include "common/ocioutils.h"
#include "node/color/colormanager/colormanager.h"

namespace olive {

ColorProcessor::ColorProcessor(ColorManager *config, const QString &input, const ColorTransform &transform,
                               Direction direction) {
  const QString &output = (transform.output().isEmpty()) ? config->GetDefaultDisplay() : transform.output();

  if (transform.is_display()) {
    const QString &view = (transform.view().isEmpty()) ? config->GetDefaultView(output) : transform.view();

    auto display_transform = OCIO::DisplayViewTransform::Create();

    display_transform->setSrc(input.toUtf8());
    display_transform->setDisplay(output.toUtf8());
    display_transform->setView(view.toUtf8());
    display_transform->setDirection(direction == kNormal ? OCIO::TRANSFORM_DIR_FORWARD : OCIO::TRANSFORM_DIR_INVERSE);

    if (transform.look().isEmpty()) {
      processor_ = config->GetConfig()->getProcessor(display_transform);
    } else {
      auto group = OCIO::GroupTransform::Create();

      const char *out_cs = OCIO::LookTransform::GetLooksResultColorSpace(
          config->GetConfig(), config->GetConfig()->getCurrentContext(), transform.look().toUtf8());

      auto lt = OCIO::LookTransform::Create();
      lt->setSrc(input.toUtf8());
      lt->setDst(out_cs);
      lt->setLooks(transform.look().toUtf8());
      lt->setSkipColorSpaceConversion(false);
      group->appendTransform(lt);

      display_transform->setSrc(out_cs);
      group->appendTransform(display_transform);

      processor_ = config->GetConfig()->getProcessor(group);
    }

  } else {
    try {
      if (direction == kNormal) {
        processor_ = config->GetConfig()->getProcessor(input.toUtf8(), output.toUtf8());
      } else {
        processor_ = config->GetConfig()->getProcessor(output.toUtf8(), input.toUtf8());
      }
    } catch (OCIO::Exception &e) {
      qWarning() << "ColorProcessor exception:" << e.what();
    }
  }

  cpu_processor_ = processor_->getDefaultCPUProcessor();
}

ColorProcessor::ColorProcessor(OCIO::ConstProcessorRcPtr processor) {
  processor_ = std::move(processor);
  cpu_processor_ = processor_->getDefaultCPUProcessor();
}

void ColorProcessor::ConvertFrame(Frame *f) {
  OCIO::BitDepth ocio_bit_depth = OCIOUtils::GetOCIOBitDepthFromPixelFormat(f->format());

  if (ocio_bit_depth == OCIO::BIT_DEPTH_UNKNOWN) {
    qCritical() << "Tried to color convert frame with no format";
    return;
  }

  OCIO::PackedImageDesc img(f->data(), f->width(), f->height(), f->channel_count(), ocio_bit_depth, OCIO::AutoStride,
                            OCIO::AutoStride, f->linesize_bytes());

  cpu_processor_->apply(img);
}

Color ColorProcessor::ConvertColor(const Color &in) {
  // I've been bamboozled
  float c[4] = {float(in.red()), float(in.green()), float(in.blue()), float(in.alpha())};

  cpu_processor_->applyRGBA(c);

  return Color(c[0], c[1], c[2], c[3]);
}

ColorProcessorPtr ColorProcessor::Create(ColorManager *config, const QString &input, const ColorTransform &transform,
                                         Direction direction) {
  return std::make_shared<ColorProcessor>(config, input, transform, direction);
}

ColorProcessorPtr ColorProcessor::Create(const OCIO::ConstProcessorRcPtr &processor) {
  return std::make_shared<ColorProcessor>(processor);
}

OCIO::ConstProcessorRcPtr ColorProcessor::GetProcessor() { return processor_; }

void ColorProcessor::ConvertFrame(const FramePtr &f) { ConvertFrame(f.get()); }

}  // namespace olive
