#ifndef RENDERPROCESSOR_H // 防止头文件被重复包含的宏
#define RENDERPROCESSOR_H // 定义 RENDERPROCESSOR_H 宏

#include "node/block/clip/clip.h" // 包含 ClipBlock (片段块) 相关的定义
#include "node/traverser.h"       // 包含 NodeTraverser 基类的定义
#include "render/renderer.h"      // 包含 Renderer (渲染器抽象基类) 的定义
#include "rendercache.h"          // 包含 DecoderCache 和 ShaderCache 的类型别名定义
#include "renderticket.h"         // 包含 RenderTicket (渲染票据) 的定义

// 假设 AudioVisualWaveform, TexturePtr, FootageJob, ShaderJob, SampleJob,
// ColorTransformJob, GenerateJob, CacheJob, VideoParams, AudioParams,
// TimeRange, rational, NodeValueDatabase, DecoderPtr, Decoder::CodecStream
// 等类型已通过上述 include 或其他方式被间接包含。

namespace olive { // olive 项目的命名空间

/**
 * @brief RenderProcessor 类是 NodeTraverser 的一个具体实现，负责实际执行渲染任务。
 *
 * 当 RenderManager 将一个 RenderTicket 分配给一个 RenderThread 时，RenderThread
 * 内部会创建一个 RenderProcessor 实例来处理这个票据。
 * RenderProcessor 会根据 RenderTicket 中的信息 (例如要渲染的节点、时间、参数等)，
 * 遍历相关的节点图，并调用具体的 Renderer (如 OpenGLRenderer) 来执行实际的
 * 图形API操作 (如着色器运行、纹理创建、数据上传/下载等)。
 *
 * 它重写了 NodeTraverser 中的许多 Process* 虚函数，以将抽象的渲染任务
 * (如 ShaderJob, FootageJob) 转换为对 Renderer 的具体调用。
 * RenderProcessor 还负责与解码器缓存 (DecoderCache) 和着色器缓存 (ShaderCache) 交互，
 * 以获取或存储解码器实例和编译好的着色器程序。
 */
class RenderProcessor : public NodeTraverser { // RenderProcessor 继承自 NodeTraverser
 public:
  /**
   * @brief (重写 NodeTraverser::GenerateDatabase) 为指定节点在给定时间范围内生成值数据库。
   * @param node 目标节点。
   * @param range 计算的时间范围。
   * @return 返回计算得到的 NodeValueDatabase。
   */
  NodeValueDatabase GenerateDatabase(const Node *node, const TimeRange &range) override;

  /**
   * @brief (静态) 处理一个渲染票据 (RenderTicket)。
   * 这是执行渲染任务的入口点。它会创建一个 RenderProcessor 实例并调用其 Run() 方法。
   * @param ticket 指向要处理的 RenderTicket 的共享指针。
   * @param render_ctx 指向 Renderer 实例的指针。
   * @param decoder_cache 指向共享的解码器缓存的指针。
   * @param shader_cache 指向共享的着色器缓存的指针。
   */
  static void Process(RenderTicketPtr ticket, Renderer *render_ctx, DecoderCache *decoder_cache,
                      ShaderCache *shader_cache);

  // 结构体，用于存储已渲染的波形数据及其元数据
  struct RenderedWaveform {
    const ClipBlock *block{};     // (可选) 与此波形关联的 ClipBlock (片段)
    AudioVisualWaveform waveform; // 实际的音频/视觉波形数据
    TimeRange range;              // 波形对应的时间范围
    bool silence{};               // 标记此范围是否为静音
  };

 protected:
  // --- 重写 NodeTraverser 中的 Process* 虚函数，以实现具体的渲染逻辑 ---

  void ProcessVideoFootage(TexturePtr destination, const FootageJob *stream, const rational &input_time) override;
  void ProcessAudioFootage(SampleBuffer &destination, const FootageJob *stream, const TimeRange &input_time) override;
  void ProcessShader(TexturePtr destination, const Node *node, const ShaderJob *job) override;
  void ProcessSamples(SampleBuffer &destination, const Node *node, const TimeRange &range,
                      const SampleJob &job) override;
  void ProcessColorTransform(TexturePtr destination, const Node *node, const ColorTransformJob *job) override;
  void ProcessFrameGeneration(TexturePtr destination, const Node *node, const GenerateJob *job) override;
  TexturePtr ProcessVideoCacheJob(const CacheJob *val) override;

  // (重写 NodeTraverser::CreateTexture) 使用当前的 render_ctx_ 创建纹理
  TexturePtr CreateTexture(const VideoParams &p) override;

  // (重写 NodeTraverser::CreateSampleBuffer) 创建音频样本缓冲区
  SampleBuffer CreateSampleBuffer(const AudioParams &params, int sample_count) override {
    // 直接使用 AudioParams 和样本数量构造 SampleBuffer
    return {params, static_cast<size_t>(sample_count)};
  }

  // (重写 NodeTraverser::ConvertToReferenceSpace) 使用当前的 render_ctx_ 进行颜色空间转换
  void ConvertToReferenceSpace(TexturePtr destination, TexturePtr source, const QString &input_cs) override;

  // (重写 NodeTraverser::UseCache) 指示在当前渲染过程中是否应使用缓存
  // (具体逻辑取决于 ticket_ 中的设置)
  [[nodiscard]] bool UseCache() const override;

 private:
  /**
   * @brief 私有构造函数。RenderProcessor 实例通常通过静态 Process 方法创建。
   * @param ticket 要处理的渲染票据。
   * @param render_ctx 渲染器上下文。
   * @param decoder_cache 解码器缓存。
   * @param shader_cache 着色器缓存。
   */
  RenderProcessor(RenderTicketPtr ticket, Renderer *render_ctx, DecoderCache *decoder_cache, ShaderCache *shader_cache);

  /**
   * @brief 根据指定时间和帧长度生成一个纹理 (TexturePtr)。
   * 内部可能会调用 GenerateFrame 来获取帧数据，然后创建纹理。
   * @param time 要生成纹理的时间点。
   * @param frame_length 帧的持续时间 (可能用于运动模糊或特定效果)。
   * @return 返回生成的纹理的共享指针。
   */
  TexturePtr GenerateTexture(const rational &time, const rational &frame_length);

  /**
   * @brief 将给定的纹理数据转换为一个 CPU 可访问的帧数据 (FramePtr)。
   * 这可能涉及到从 GPU 下载数据。
   * @param texture 包含源图像数据的 TexturePtr。
   * @param time 帧对应的时间点。
   * @return 返回包含帧数据的 FramePtr。
   */
  FramePtr GenerateFrame(TexturePtr texture, const rational &time);

  /**
   * @brief 执行渲染票据中定义的渲染任务。
   * 这是 RenderProcessor 的核心逻辑，它会根据票据类型 (视频、音频)
   * 调用相应的生成方法 (GenerateTexture, GenerateFrame, 或处理音频的逻辑)。
   */
  void Run();

  /**
   * @brief 根据解码器ID和媒体流信息从解码器缓存中获取或创建一个解码器实例。
   * @param decoder_id 解码器的名称或标识符。
   * @param stream 描述媒体流的 Decoder::CodecStream 对象。
   * @return 返回指向解码器实例的 DecoderPtr。
   */
  DecoderPtr ResolveDecoderFromInput(const QString &decoder_id, const Decoder::CodecStream &stream);

  RenderTicketPtr ticket_; // 当前正在处理的渲染票据

  Renderer *render_ctx_; // 指向实际执行渲染操作的 Renderer 实例 (例如 OpenGLRenderer)

  DecoderCache *decoder_cache_; // 指向共享的解码器缓存
  ShaderCache *shader_cache_;   // 指向共享的着色器缓存
};

}  // namespace olive

// 声明 RenderProcessor::RenderedWaveform 类型为元类型，以便在 QVariant 中使用或在信号槽中传递
Q_DECLARE_METATYPE(olive::RenderProcessor::RenderedWaveform)

#endif  // RENDERPROCESSOR_H