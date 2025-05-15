#ifndef RENDERPROCESSOR_H
#define RENDERPROCESSOR_H

#include "node/block/clip/clip.h"
#include "node/traverser.h"
#include "render/renderer.h"
#include "rendercache.h"
#include "renderticket.h"

namespace olive {

class RenderProcessor : public NodeTraverser {
 public:
  NodeValueDatabase GenerateDatabase(const Node *node, const TimeRange &range) override;

  static void Process(RenderTicketPtr ticket, Renderer *render_ctx, DecoderCache *decoder_cache,
                      ShaderCache *shader_cache);

  struct RenderedWaveform {
    const ClipBlock *block{};
    AudioVisualWaveform waveform;
    TimeRange range;
    bool silence{};
  };

 protected:
  void ProcessVideoFootage(TexturePtr destination, const FootageJob *stream, const rational &input_time) override;

  void ProcessAudioFootage(SampleBuffer &destination, const FootageJob *stream, const TimeRange &input_time) override;

  void ProcessShader(TexturePtr destination, const Node *node, const ShaderJob *job) override;

  void ProcessSamples(SampleBuffer &destination, const Node *node, const TimeRange &range,
                      const SampleJob &job) override;

  void ProcessColorTransform(TexturePtr destination, const Node *node, const ColorTransformJob *job) override;

  void ProcessFrameGeneration(TexturePtr destination, const Node *node, const GenerateJob *job) override;

  TexturePtr ProcessVideoCacheJob(const CacheJob *val) override;

  TexturePtr CreateTexture(const VideoParams &p) override;

  SampleBuffer CreateSampleBuffer(const AudioParams &params, int sample_count) override {
    return {params, static_cast<size_t>(sample_count)};
  }

  void ConvertToReferenceSpace(TexturePtr destination, TexturePtr source, const QString &input_cs) override;

  [[nodiscard]] bool UseCache() const override;

 private:
  RenderProcessor(RenderTicketPtr ticket, Renderer *render_ctx, DecoderCache *decoder_cache, ShaderCache *shader_cache);

  TexturePtr GenerateTexture(const rational &time, const rational &frame_length);

  FramePtr GenerateFrame(TexturePtr texture, const rational &time);

  void Run();

  DecoderPtr ResolveDecoderFromInput(const QString &decoder_id, const Decoder::CodecStream &stream);

  RenderTicketPtr ticket_;

  Renderer *render_ctx_;

  DecoderCache *decoder_cache_;

  ShaderCache *shader_cache_;
};

}  // namespace olive

Q_DECLARE_METATYPE(olive::RenderProcessor::RenderedWaveform)

#endif  // RENDERPROCESSOR_H
