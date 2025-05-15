#ifndef SEQUENCEDIALOGPRESETTAB_H
#define SEQUENCEDIALOGPRESETTAB_H

#include <QLabel>       // 引入 QLabel 类，用于显示文本或图片
#include <QTreeWidget>  // 引入 QTreeWidget 类，用于创建树形列表控件
#include <QWidget>      // 引入 QWidget 类，所有用户界面对象的基类

#include "presetmanager.h"  // 引入预设管理器模板类
#include "sequencepreset.h" // 引入序列预设类定义

namespace olive {

// 前向声明，避免不必要的头文件包含
class SequencePreset; // 具体预设类型

/**
 * @class SequenceDialogPresetTab
 * @brief 序列对话框中的预设选项卡。
 *
 * 这个类继承自 QWidget 和 PresetManager<SequencePreset>，
 * 用于在序列对话框中提供一个界面来管理和选择序列预设。
 * 它会显示一个包含标准预设和用户自定义预设的树形列表。
 */
class SequenceDialogPresetTab : public QWidget, public PresetManager<SequencePreset> {
  Q_OBJECT
 public:
  /**
   * @brief SequenceDialogPresetTab 构造函数。
   *
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit SequenceDialogPresetTab(QWidget* parent = nullptr);

 public slots:
  /**
   * @brief 公有槽函数：将参数另存为预设。
   *
   * 当从参数选项卡接收到保存请求时，此槽函数被调用，
   * 并使用 PresetManager 的功能来保存提供的预设。
   * @param preset 包含要保存参数的 SequencePreset 对象。
   */
  void SaveParametersAsPreset(const SequencePreset& preset);

 signals:
  /**
   * @brief 信号：当用户在预设列表中选择或更改了一个预设时发出。
   *
   * @param preset 被选中的 SequencePreset 对象。
   */
  void PresetChanged(const SequencePreset& preset);

  /**
   * @brief 信号：当用户接受（例如双击）一个预设时发出。
   *
   * 通常用于确认选择并可能关闭对话框或应用预设。
   */
  void PresetAccepted();

 private:
  /**
   * @brief 私有静态辅助函数：创建一个新的树形控件文件夹项。
   * @param name 文件夹的显示名称。
   * @return QTreeWidgetItem* 指向新创建的文件夹项的指针。
   */
  static QTreeWidgetItem* CreateFolder(const QString& name);

  /**
   * @brief 私有辅助函数：创建并填充一个高清 (HD) 预设文件夹及其子项。
   * @param name 文件夹的显示名称。
   * @param width 视频宽度。
   * @param height 视频高度。
   * @param divider 预览分辨率的除数。
   * @return QTreeWidgetItem* 指向新创建的 HD 预设文件夹项的指针。
   */
  QTreeWidgetItem* CreateHDPresetFolder(const QString& name, int width, int height, int divider);

  /**
   * @brief 私有辅助函数：创建并填充一个标清 (SD) 预设文件夹及其子项。
   * @param name 文件夹的显示名称。
   * @param width 视频宽度。
   * @param height 视频高度。
   * @param frame_rate 帧率。
   * @param standard_par 标准像素宽高比。
   * @param wide_par 宽屏像素宽高比。
   * @param divider 预览分辨率的除数。
   * @return QTreeWidgetItem* 指向新创建的 SD 预设文件夹项的指针。
   */
  QTreeWidgetItem* CreateSDPresetFolder(const QString& name, int width, int height, const rational& frame_rate,
                                        const rational& standard_par, const rational& wide_par, int divider);

  /**
   * @brief 私有辅助函数：获取当前在预设树中选中的项。
   * @return QTreeWidgetItem* 指向当前选中项的指针，如果没有选中项则返回 nullptr。
   */
  QTreeWidgetItem* GetSelectedItem();
  /**
   * @brief 私有辅助函数：获取当前选中的自定义预设项。
   * @return QTreeWidgetItem* 指向当前选中的自定义预设项的指针，如果选中的不是自定义预设或没有选中则返回 nullptr。
   */
  QTreeWidgetItem* GetSelectedCustomPreset();

  /**
   * @brief 私有辅助函数：向指定的文件夹中添加一个标准预设项。
   * @param folder 要添加预设项的父文件夹项。
   * @param preset 要添加的预设对象的共享指针。
   * @param description 可选的预设描述文本。
   */
  void AddStandardItem(QTreeWidgetItem* folder, const PresetPtr& preset, const QString& description = QString());

  /**
   * @brief 私有静态辅助函数：向指定的文件夹中添加一个自定义预设项。
   * @param folder 要添加预设项的父文件夹项。
   * @param preset 要添加的预设对象的共享指针。
   * @param index 预设在自定义预设列表中的索引，用于在 QTreeWidgetItem 中存储额外数据。
   * @param description 可选的预设描述文本。
   */
  static void AddCustomItem(QTreeWidgetItem* folder, const PresetPtr& preset, int index,
                            const QString& description = QString());

  /**
   * @brief 私有静态内部辅助函数：实际执行添加预设项到树形控件的逻辑。
   * @param folder 要添加预设项的父文件夹项。
   * @param preset 要添加的预设对象的共享指针。
   * @param is_custom 标记此预设是否为自定义预设。
   * @param index 如果是自定义预设，则为其在列表中的索引；否则通常为 -1。
   * @param description 可选的预设描述文本。
   */
  static void AddItemInternal(QTreeWidgetItem* folder, const PresetPtr& preset, bool is_custom, int index,
                              const QString& description = QString());

  /**
   * @brief 指向用于显示预设列表的树形控件的指针。
   */
  QTreeWidget* preset_tree_;

  /**
   * @brief 指向预设树中 "我的预设" (用户自定义预设) 文件夹的指针。
   */
  QTreeWidgetItem* my_presets_folder_;

  /**
   * @brief 存储默认/标准预设数据的 QVector 容器。
   */
  QVector<PresetPtr> default_preset_data_;

 private slots:
  /**
   * @brief 私有槽函数：当预设树中的选中项发生改变时调用。
   * @param current 当前选中的项。
   * @param previous 先前选中的项。
   */
  void SelectedItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

  /**
   * @brief 私有槽函数：当预设树中的某个项被双击时调用。
   * @param item 被双击的项。
   * @param column 被双击的列（通常为0）。
   */
  void ItemDoubleClicked(QTreeWidgetItem* item, int column);

  /**
   * @brief 私有槽函数：显示预设树的上下文菜单（右键菜单）。
   */
  void ShowContextMenu();

  /**
   * @brief 私有槽函数：删除当前选中的自定义预设。
   */
  void DeleteSelectedPreset();
};

}  // namespace olive

#endif  // SEQUENCEDIALOGPRESETTAB_H
