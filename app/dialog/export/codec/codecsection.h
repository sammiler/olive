#ifndef CODECSECTION_H
#define CODECSECTION_H

#include <QWidget> // QWidget 基类

// 假设 encoder.h 声明了 EncodingParams 类
#include "codec/encoder.h" // 包含 EncodingParams
// #include "common/define.h" // 如果需要 common/define.h 中的内容

namespace olive {

/**
 * @brief 导出对话框中特定编解码器选项区域的抽象基类。
 *
 *此类作为所有具体编解码器设置UI区域 (例如 H264Section, AV1Section, CineformSection 等)
 * 的共同父类。它提供了一个统一的接口，允许导出对话框以通用方式
 * 与这些特定于编解码器的设置部分进行交互，主要是为了收集用户配置的
 * 编码选项 (`AddOpts`) 和从现有参数中加载设置到UI (`SetOpts`)。
 */
class CodecSection : public QWidget {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 CodecSection 对象。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit CodecSection(QWidget* parent = nullptr);

  // 通常，作为基类，拥有一个虚析构函数是良好的实践，以确保派生类能正确销毁。
  /**
   * @brief 虚析构函数。
   * 确保派生类对象能够被正确销毁。
   */
  ~CodecSection() override = default;

  /**
   * @brief 将此编解码器部分中用户配置的选项添加到 EncodingParams 对象中。
   *
   * 派生类应覆盖此方法，以将其UI控件中的当前设置值
   * 填充到传入的 `params` 对象中，供后续的编码过程使用。
   *
   * @param params 指向 EncodingParams 对象的指针，特定于此编解码器的选项将被添加到此对象。
   * 基类实现为空，使用 Q_UNUSED 标记参数未使用，以避免编译器警告。
   */
  virtual void AddOpts(EncodingParams* params) { Q_UNUSED(params) }

  /**
   * @brief 根据提供的 EncodingParams 对象中的设置来配置此编解码器部分的UI控件。
   *
   * 派生类应覆盖此方法，以从传入的 `p` 对象中读取与此编解码器相关的设置，
   * 并相应地更新其UI控件的状态和值。
   * 这通常在加载编码预设或从已有配置初始化对话框时使用。
   *
   * @param p 指向包含要应用的编码参数的 EncodingParams 对象的常量指针。
   * 基类实现为空，使用 Q_UNUSED 标记参数未使用。
   */
  virtual void SetOpts(const EncodingParams* p) { Q_UNUSED(p) }
};

}  // namespace olive

#endif  // CODECSECTION_H