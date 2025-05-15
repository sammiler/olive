#ifndef EXPORTSUBTITLESTAB_H
#define EXPORTSUBTITLESTAB_H

#include <QCheckBox> // 复选框控件
#include <QComboBox> // 下拉选择框控件
#include <QLabel>    // 标签控件 (用于显示文本)
#include <QWidget>   // QWidget 基类
#include <QVariant>  // 为了 currentData().toInt()

// Olive 内部头文件
#include "codec/exportformat.h" // 包含 ExportFormat::Format 和 ExportCodec::Codec
#include "common/qtutils.h"     // 包含 QtUtils::SetComboBoxData (用于设置 QComboBox 数据)
#include "dialog/export/exportformatcombobox.h" // 包含自定义的 ExportFormatComboBox
// #include "common/define.h" // 如果需要 common/define.h 中的内容

namespace olive {

/**
 * @brief 导出对话框中用于配置字幕导出选项的选项卡页面。
 *
 * 此类继承自 QWidget (通常会作为 CodecSection 或类似基类的派生类，但这里直接是 QWidget)，
 * 提供了设置字幕导出参数的用户界面元素，例如：
 * - 是否启用边车 (sidecar) 字幕文件输出。
 * - 如果启用边车字幕，选择其文件格式。
 * - 选择字幕流的编解码器（对于嵌入式字幕或某些特定边车格式）。
 */
class ExportSubtitlesTab : public QWidget {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 ExportSubtitlesTab 对象。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit ExportSubtitlesTab(QWidget *parent = nullptr);

  // ~ExportSubtitlesTab() override; // 默认析构函数通常足够

  /**
   * @brief 获取“启用边车字幕”复选框的当前选中状态。
   * @return bool 如果复选框被选中（表示启用边车字幕），则返回 true；否则返回 false。
   */
  [[nodiscard]] bool GetSidecarEnabled() const { return sidecar_checkbox_->isChecked(); }

  /**
   * @brief 设置“启用边车字幕”复选框的启用/禁用状态。
   * 注意：此函数设置的是控件本身的可用性 (enabled/disabled)，而不是其选中状态 (checked/unchecked)。
   * @param e 如果为 true，则启用复选框；如果为 false，则禁用。
   */
  void SetSidecarEnabled(bool e) { sidecar_checkbox_->setEnabled(e); } // 方法名可能是笔误，通常是 setChecked 或 setEnabled

  /**
   * @brief 获取用户在边车字幕格式下拉框中当前选中的格式。
   * @return ExportFormat::Format 选中的边车字幕文件格式枚举值。
   */
  [[nodiscard]] ExportFormat::Format GetSidecarFormat() const { return sidecar_format_combobox_->GetFormat(); }

  /**
   * @brief 设置边车字幕格式下拉框的当前选中项。
   * @param f 要选中的 ExportFormat::Format 枚举值。
   */
  void SetSidecarFormat(ExportFormat::Format f) { sidecar_format_combobox_->SetFormat(f); }

  /**
   * @brief 根据指定的导出文件封装格式 (Format) 来更新此字幕选项卡中可用控件的状态。
   * 例如，某些主封装格式可能不支持嵌入式字幕，此时边车字幕选项可能会被强制启用或UI调整。
   * @param format 当前选定的导出文件封装格式。
   * @return int 通常返回一个状态码或指示操作结果的值。
   * （返回类型为 int，其具体含义需参照实现）。
   */
  int SetFormat(ExportFormat::Format format);

  /**
   * @brief 获取用户在字幕编解码器下拉框中当前选中的编解码器。
   * @return ExportCodec::Codec 选中的字幕编解码器枚举值。
   */
  ExportCodec::Codec GetSubtitleCodec() {
    // 假设 QComboBox 的 itemData 存储的是 ExportCodec::Codec 枚举值对应的整数
    return static_cast<ExportCodec::Codec>(codec_combobox_->currentData().toInt());
  }

  /**
   * @brief 设置字幕编解码器下拉框的当前选中项。
   * 使用 QtUtils::SetComboBoxData 辅助函数来根据数据查找并设置索引。
   * @param c 要选中的 ExportCodec::Codec 枚举值。
   */
  void SetSubtitleCodec(ExportCodec::Codec c) { QtUtils::SetComboBoxData(codec_combobox_, static_cast<int>(c)); } // 确保传入int类型数据

 private:
  /**
   * @brief 指向 QCheckBox 控件的指针，用于让用户选择是否导出边车 (独立的) 字幕文件。
   */
  QCheckBox *sidecar_checkbox_;

  /**
   * @brief 指向 QLabel 控件的指针，用于显示“边车格式”等文本标签。
   */
  QLabel *sidecar_format_label_;
  /**
   * @brief 指向 ExportFormatComboBox (自定义下拉框) 的指针，用于选择边车字幕的文件格式。
   */
  ExportFormatComboBox *sidecar_format_combobox_;

  /**
   * @brief 指向 QComboBox 控件的指针，用于选择字幕流的编解码器
   * (例如，对于嵌入式字幕或某些特定的边车格式如MKV中的ASS/SSA)。
   */
  QComboBox *codec_combobox_;
};

}  // namespace olive

#endif  // EXPORTSUBTITLESTAB_H