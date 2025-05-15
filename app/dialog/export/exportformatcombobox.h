#ifndef EXPORTFORMATCOMBOBOX_H
#define EXPORTFORMATCOMBOBOX_H

#include <QComboBox>     // QComboBox 基类
#include <QWidgetAction> // 用于在 QMenu 中嵌入 QWidget (可能用于创建标题项)
#include <QWidget>       // 为了 QWidget* parent 参数
#include <QIcon>         // 为了 CreateHeader 的 icon 参数
#include <QString>       // 为了 CreateHeader 的 title 参数
#include <QAction>       // 为了 HandleIndexChange 的参数

// Olive 内部头文件
#include "codec/exportformat.h" // 包含 ExportFormat::Format 枚举
#include "node/output/track/track.h" // 包含 Track::Type 枚举 (用于 PopulateType)
#include "widget/menu/menu.h"   // 包含自定义的 Menu 类 (custom_menu_)
// #include "common/define.h" // 如果需要 common/define.h 中的内容

namespace olive {

/**
 * @brief 一个自定义的组合框 (ComboBox) 类，用于选择导出文件格式。
 *
 * 此类继承自 QComboBox，提供了对导出格式进行分类显示（例如按视频、音频、字幕）
 * 和选择的功能。它使用一个自定义的弹出菜单 (Menu) 来展示选项，
 * 而不是标准的 QComboBox 下拉列表。
 * 可以通过不同的模式 (Mode) 来控制下拉列表中显示的格式类型。
 */
class ExportFormatComboBox : public QComboBox {
  Q_OBJECT
 public:
  /**
   * @brief 定义组合框显示格式的模式。
   */
  enum Mode {
    kShowAllFormats,    ///< @brief 显示所有支持的导出格式。
    kShowAudioOnly,     ///< @brief 仅显示音频相关的导出格式。
    kShowVideoOnly,     ///< @brief 仅显示视频相关的导出格式（可能包括同时支持音视频的格式）。
    kShowSubtitlesOnly  ///< @brief 仅显示字幕相关的导出格式。
  };

  /**
   * @brief 构造一个新的 ExportFormatComboBox 对象。
   * @param mode 组合框的显示模式，决定了哪些类型的格式会被列出。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit ExportFormatComboBox(Mode mode, QWidget *parent = nullptr);

  /**
   * @brief 构造一个新的 ExportFormatComboBox 对象，默认模式为 kShowAllFormats。
   * 这是对主构造函数的便捷调用。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit ExportFormatComboBox(QWidget *parent = nullptr) : ExportFormatComboBox(kShowAllFormats, parent) {}

  // ~ExportFormatComboBox() override; // 默认析构函数通常足够

  /**
   * @brief 获取当前在组合框中选中的导出格式。
   * @return ExportFormat::Format 当前选中的导出格式枚举值。
   */
  [[nodiscard]] ExportFormat::Format GetFormat() const { return current_; }

  /**
   * @brief 重写 QComboBox::showPopup() 方法。
   * 此方法被调用以显示自定义的弹出菜单 (`custom_menu_`)，而不是标准的下拉列表。
   */
  void showPopup() override;

 signals:
  /**
   * @brief 当用户通过自定义菜单选择了一个新的导出格式时发出此信号。
   * @param fmt 新选中的 ExportFormat::Format 枚举值。
   */
  void FormatChanged(ExportFormat::Format fmt);

 public slots:
  /**
   * @brief 以编程方式设置组合框当前选中的导出格式。
   * 这会更新组合框显示的文本以及内部的 `current_` 状态，并可能触发 `FormatChanged` 信号。
   * @param fmt 要设置的 ExportFormat::Format 枚举值。
   */
  void SetFormat(ExportFormat::Format fmt);

 private slots:
  /**
   * @brief 当用户在自定义弹出菜单中选择一个 QAction 时调用的槽函数。
   * 此函数会根据选中的 QAction (通常其 data() 中存储了 ExportFormat::Format 值)
   * 来更新当前选中的格式，并发出 FormatChanged 信号。
   * @param a 用户点击的 QAction 对象指针。
   */
  void HandleIndexChange(QAction *a);

 private:
  /**
   * @brief 根据指定的轨道类型 (视频、音频、字幕) 填充自定义菜单中的一部分。
   * @param type 要为其填充格式选项的轨道类型 (Track::Type)。
   */
  void PopulateType(Track::Type type);

  /**
   * @brief 创建一个用作菜单中分组标题的 QWidgetAction。
   * @param icon 标题的图标。
   * @param title 标题的文本。
   * @return QWidgetAction* 指向新创建的 QWidgetAction 对象的指针。调用者可能需要管理其生命周期，或者它会被添加到菜单中由菜单管理。
   */
  QWidgetAction *CreateHeader(const QIcon &icon, const QString &title);

  /**
   * @brief 指向自定义弹出菜单 (Menu) 对象的指针。
   * 此菜单用于替代 QComboBox 的标准下拉列表。
   */
  Menu *custom_menu_;

  /**
   * @brief 存储当前选中的导出格式 (ExportFormat::Format)。
   */
  ExportFormat::Format current_;
};

}  // namespace olive

#endif  // EXPORTFORMATCOMBOBOX_H