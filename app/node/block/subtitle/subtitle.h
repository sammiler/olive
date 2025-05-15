#ifndef SUBTITLEBLOCK_H
#define SUBTITLEBLOCK_H

#include "node/block/clip/clip.h" // 应该是 "node/block/clipblock.h" 或者类似 ClipBlock 的头文件路径
                                   // "node/block/clip/clip.h" 看起来可能是笔误或项目结构中的特定路径
                                   // 但根据类继承 `SubtitleBlock : public ClipBlock`，它需要 ClipBlock 的定义

namespace olive {

/**
 * @brief 代表字幕片段的节点。
 * 继承自 ClipBlock，用于在时间线上显示文本字幕。
 */
class SubtitleBlock : public ClipBlock {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief SubtitleBlock 构造函数。
   */
  SubtitleBlock();

  NODE_DEFAULT_FUNCTIONS(SubtitleBlock) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此 SubtitleBlock 的名称。
   * @return QString 类型的节点名称（例如，可能会返回 "字幕"）。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此 SubtitleBlock 的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此 SubtitleBlock 的描述信息。
   * @return QString 类型的节点描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本。
   * 例如，更新节点名称或描述的翻译。
   */
  void Retranslate() override;

  static const QString kTextIn; ///< "TextIn" - 用于存储字幕文本内容的输入参数的键名。

  /**
   * @brief 获取当前字幕块的文本内容。
   * @return QString 类型的字幕文本。
   */
  [[nodiscard]] QString GetText() const { return GetStandardValue(kTextIn).toString(); }

  /**
   * @brief 设置字幕块的文本内容。
   * @param text QString 类型的字幕文本。
   */
  void SetText(const QString &text) { SetStandardValue(kTextIn, text); }
};

}  // namespace olive

#endif  // SUBTITLEBLOCK_H