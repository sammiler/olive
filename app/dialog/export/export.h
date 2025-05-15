#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QComboBox>          // 下拉选择框控件
#include <QDialog>            // QDialog 基类
#include <QDialogButtonBox>   // 标准对话框按钮盒 (可能在 .cpp 中用于按钮)
#include <QLineEdit>          // 单行文本输入框，用于文件名
#include <QProgressBar>       // 进度条 (可能在 .cpp 中用于显示导出进度)
#include <QTabWidget>         // 选项卡控件 (preferences_tabs_)
#include <QCheckBox>          // 复选框控件
#include <QWidget>            // 为了 QWidget* parent 参数
#include <QString>            // Qt 字符串类
#include <QStringList>        // Qt 字符串列表
#include <QEvent>             // 为了 eventFilter
#include <vector>             // 为了 std::vector<EncodingParams>

// Olive 内部头文件
#include "codec/exportcodec.h"  // 包含 ExportCodec 枚举
#include "codec/exportformat.h" // 包含 ExportFormat 枚举
#include "codec/encoder.h"      // 包含 EncodingParams 类 (此文件也定义了 EncodingParams)
#include "dialog/export/exportformatcombobox.h" // 自定义格式选择下拉框
#include "dialog/export/exportaudiotab.h"       // 音频导出设置选项卡
#include "dialog/export/exportsubtitlestab.h"   // 字幕导出设置选项卡
#include "dialog/export/exportvideotab.h"       // 视频导出设置选项卡
#include "task/export/export.h" // 包含 ExportTask 类 (虽然此处是 Export.h，通常代表导出任务类)
#include "widget/nodeparamview/nodeparamviewwidgetbridge.h" // (用途需参照 .cpp)
#include "widget/viewer/viewer.h" // 包含 ViewerOutput, ViewerWidget
#include "common/define.h"      // 包含 olive::rational, ColorManager 等核心类型
#include "node/color/colormanager/colormanager.h" // 包含 ColorManager (如果未被 define.h 或 viewer.h 包含)

// 前向声明 (如果需要)
// class ViewerOutput;
// class EncodingParams;
// class ExportVideoTab;
// class ExportAudioTab;
// class ExportSubtitlesTab;
// class ExportFormatComboBox;
// class ViewerWidget;
// class ColorManager;

namespace olive {

/**
 * @brief 提供媒体导出功能的主对话框类。
 *
 * 此对话框允许用户配置视频、音频和字幕的导出参数，包括格式、编解码器、
 * 分辨率、码率、导出范围等。它还支持加载和保存导出预设，
 * 并提供了一个预览区域以及启动实际导出过程的控件。
 */
class ExportDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 ExportDialog 对象。
   * @param viewer_node 指向 ViewerOutput 对象的指针，用于获取要导出的序列信息和预览。
   * @param stills_only_mode 如果为 true，则对话框配置为仅导出静态图像（例如，禁用音频和字幕选项，范围可能仅限单帧）。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  ExportDialog(ViewerOutput* viewer_node, bool stills_only_mode, QWidget* parent = nullptr);

  /**
   * @brief 构造一个新的 ExportDialog 对象（非仅静态图像模式）。
   * 这是对主构造函数的便捷调用，将 `stills_only_mode` 设置为 false。
   * @param viewer_node 指向 ViewerOutput 对象的指针。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit ExportDialog(ViewerOutput* viewer_node, QWidget* parent = nullptr)
      : ExportDialog(viewer_node, false, parent) {} // 委托构造

  // ~ExportDialog() override; // 默认析构函数通常足够

  /**
   * @brief 获取当前选定的导出时间基准（例如帧率）。
   * @return rational 选定的时间基准。
   */
  [[nodiscard]] rational GetSelectedTimebase() const;

  /**
   * @brief 设置导出的时间基准。
   * @param r 要设置的时间基准。
   */
  void SetSelectedTimebase(const rational& r);

  /**
   * @brief 根据对话框中当前的用户设置生成一个 EncodingParams 对象。
   * 此对象将包含所有配置好的导出参数，用于创建导出任务。
   * @return EncodingParams 包含所有导出设置的参数对象。
   */
  [[nodiscard]] EncodingParams GenerateParams() const;

  /**
   * @brief 使用提供的 EncodingParams 对象来配置对话框的UI控件。
   * 这通常用于加载预设或恢复之前的导出设置。
   * @param e 包含要应用的导出设置的 EncodingParams 对象。
   */
  void SetParams(const EncodingParams& e);

  /**
   * @brief 事件过滤器，用于拦截和处理特定对象的事件。
   * 可能用于例如在特定控件上实现自定义键盘导航或行为。
   * @param o 接收事件的对象。
   * @param e 发生的事件。
   * @return bool 如果事件被处理且不应进一步传递，则返回 true；否则返回 false。
   */
  bool eventFilter(QObject* o, QEvent* e) override;

 public slots:
  /**
   * @brief 重写 QDialog::done() 槽函数。
   * 当对话框以某种结果代码关闭时（例如通过 accept() 或 reject()），此函数会被调用。
   * @param r 对话框的结果代码 (例如 QDialog::Accepted, QDialog::Rejected)。
   */
  void done(int r) override;

 signals:
  /**
   * @brief 当用户勾选“导出后导入文件”并成功完成导出时，请求将导出的文件导入到项目中。
   * @param s 导出的文件的完整路径。
   */
  void RequestImportFile(const QString& s);

 private:
  /**
   * @brief 向主选项卡控件 (`preferences_tabs_`) 中添加一个新的配置页面。
   * @param inner_widget 指向要作为新选项卡页面内容的 QWidget 的指针。
   * @param title 新选项卡的标题。
   */
  void AddPreferencesTab(QWidget* inner_widget, const QString& title);

  /**
   * @brief 加载可用的导出预设到 `preset_combobox_`。
   */
  void LoadPresets();
  /**
   * @brief 根据当前序列或项目设置一个默认的输出文件名。
   */
  void SetDefaultFilename();

  /**
   * @brief 检查当前要导出的序列是否包含字幕轨道。
   * @return bool 如果序列包含字幕则返回 true，否则返回 false。
   */
  [[nodiscard]] bool SequenceHasSubtitles() const;

  /**
   * @brief 将对话框中的所有控件设置为默认值。
   * 这通常在对话框首次显示或用户请求重置设置时调用。
   */
  void SetDefaults();

  /**
   * @brief 指向 ViewerOutput 节点的指针，代表要导出的序列或视图。
   */
  ViewerOutput* viewer_node_;

  /**
   * @brief 存储之前选中的导出格式。
   * 用于在格式更改时检测变化并可能执行一些更新操作。
   */
  ExportFormat::Format previously_selected_format_;

  /**
   * @brief 获取当前配置的导出总长度（时间）。
   * @return rational 导出长度。
   */
  [[nodiscard]] rational GetExportLength() const;
  /**
   * @brief 以当前时间基准为单位获取导出总长度。
   * @return int64_t 以时间基准单位表示的导出长度（例如总帧数）。
   */
  [[nodiscard]] int64_t GetExportLengthInTimebaseUnits() const;

  /**
   * @brief 定义导出范围选择的枚举。
   */
  enum RangeSelection {
    kRangeEntireSequence, ///< @brief 导出整个序列。
    kRangeInToOut         ///< @brief 仅导出标记的入点到出点之间的范围。
  };

  /**
   * @brief 定义预设下拉框中特殊项目的枚举。
   */
  enum AutoPreset {
    kPresetDefault = -1,  ///< @brief 代表“默认设置”或“当前设置”（非特定预设）。
    kPresetLastUsed = -2, ///< @brief 代表“上次使用的设置”。
  };

  /**
   * @brief 指向 QTabWidget 的指针，用于容纳视频、音频、字幕等不同类别的导出设置选项卡。
   */
  QTabWidget* preferences_tabs_;

  /**
   * @brief 指向 QComboBox 的指针，用于选择导出预设。
   */
  QComboBox* preset_combobox_;
  /**
   * @brief 指向 QComboBox 的指针，用于选择导出范围 (例如 "整个序列", "入点到出点")。
   */
  QComboBox* range_combobox_;
  /**
   * @brief 存储已加载的导出预设 (EncodingParams 对象) 的向量。
   */
  std::vector<EncodingParams> presets_;

  /**
   * @brief 指向 QCheckBox 的指针，用于启用或禁用视频流的导出。
   */
  QCheckBox* video_enabled_;
  /**
   * @brief 指向 QCheckBox 的指针，用于启用或禁用音频流的导出。
   */
  QCheckBox* audio_enabled_;
  /**
   * @brief 指向 QCheckBox 的指针，用于启用或禁用字幕流的导出。
   */
  QCheckBox* subtitles_enabled_;

  /**
   * @brief 指向 ViewerWidget 的指针，用于在导出对话框中显示一个小的预览。
   */
  ViewerWidget* preview_viewer_;
  /**
   * @brief 指向 QLineEdit 的指针，用于输入或显示输出文件名。
   */
  QLineEdit* filename_edit_;
  /**
   * @brief 指向 ExportFormatComboBox (自定义下拉框) 的指针，用于选择输出文件格式。
   */
  ExportFormatComboBox* format_combobox_;

  /**
   * @brief 指向 ExportVideoTab 控件的指针，包含视频相关的导出设置。
   */
  ExportVideoTab* video_tab_;
  /**
   * @brief 指向 ExportAudioTab 控件的指针，包含音频相关的导出设置。
   */
  ExportAudioTab* audio_tab_;
  /**
   * @brief 指向 ExportSubtitlesTab 控件的指针，包含字幕相关的导出设置。
   */
  ExportSubtitlesTab* subtitle_tab_;

  /**
   * @brief 存储源视频的宽高比，可能用于预览或尺寸调整计算。
   */
  double video_aspect_ratio_;

  /**
   * @brief 指向应用程序的颜色管理器实例的指针。
   */
  ColorManager* color_manager_;

  /**
   * @brief 指向容纳主要首选项设置区域的 QWidget 的指针。
   */
  QWidget* preferences_area_;
  /**
   * @brief 指向 QCheckBox 的指针，用于选择是否在后台执行导出任务。
   */
  QCheckBox* export_bkg_box_;
  /**
   * @brief 指向 QCheckBox 的指针，用于选择导出完成后是否自动将生成的文件导入到项目中。
   */
  QCheckBox* import_file_after_export_;

  /**
   * @brief 标记对话框是否处于“仅导出静态图像”模式。
   */
  bool stills_only_mode_;

  /**
   * @brief 标记当前是否正在加载预设。
   * 用于在加载预设时临时禁止某些UI更新或信号处理。
   */
  bool loading_presets_;

 private slots:
  /**
   * @brief 当用户点击“浏览”按钮选择输出文件名和路径时调用的槽函数。
   */
  void BrowseFilename();

  /**
   * @brief 当 `format_combobox_` 中选择的导出格式发生变化时调用的槽函数。
   * 此函数可能会更新可用的编解码器选项和相关的UI元素。
   * @param current_format 新选中的导出文件格式。
   */
  void FormatChanged(ExportFormat::Format current_format);

  /**
   * @brief 当视频分辨率相关的设置（例如在 `video_tab_` 中）发生变化时调用的槽函数。
   * 可能需要更新预览区域的尺寸或宽高比。
   */
  void ResolutionChanged();

  /**
   * @brief 更新预览查看器 (`preview_viewer_`) 的尺寸以匹配当前的导出分辨率和宽高比。
   */
  void UpdateViewerDimensions();

  /**
   * @brief 开始导出过程。
   * 此槽函数通常连接到“导出”按钮的点击信号。它会收集所有参数，
   * 创建一个导出任务 (ExportTask)，并启动该任务。
   */
  void StartExport();

  /**
   * @brief 当导出任务完成（成功或失败）时调用的槽函数。
   * 可能会更新UI，例如显示完成消息或错误提示。
   */
  void ExportFinished(); // 通常连接到 ExportTask 的完成信号

  /**
   * @brief 当视频选项卡中的“图像序列”复选框状态改变时调用的槽函数。
   * 这可能会影响文件名模式、范围选择以及其他相关UI的可用性。
   * @param e 复选框新的选中状态。
   */
  void ImageSequenceCheckBoxChanged(bool e);

  /**
   * @brief 保存当前的导出设置为一个新的预设。
   * 通常会弹出一个对话框让用户输入预设名称。
   */
  void SavePreset();

  /**
   * @brief 当 `preset_combobox_` 中选择的预设发生变化时调用的槽函数。
   * 此函数会加载选中预设的参数并更新对话框中的所有UI控件。
   */
  void PresetComboBoxChanged();
};

}  // namespace olive

#endif  // EXPORTDIALOG_H