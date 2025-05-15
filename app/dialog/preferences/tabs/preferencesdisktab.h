#ifndef PREFERENCESDISKTAB_H
#define PREFERENCESDISKTAB_H

#include <QCheckBox>   // 引入 QCheckBox 类，用于创建复选框控件
#include <QLineEdit>   // 引入 QLineEdit 类，用于创建单行文本输入框控件
#include <QPushButton> // 引入 QPushButton 类，用于创建按钮控件

#include "dialog/configbase/configdialogbase.h" // 引入配置对话框基类相关定义
#include "render/diskmanager.h"                 // 引入磁盘管理器相关定义，可能用于管理磁盘缓存文件夹
#include "widget/path/pathwidget.h"             // 引入路径选择控件
#include "widget/slider/floatslider.h"          // 引入浮点数滑块控件

namespace olive {

/**
 * @class PreferencesDiskTab
 * @brief “首选项”对话框中的“磁盘”选项卡类。
 *
 * 继承自 ConfigDialogBaseTab，用于管理应用程序磁盘缓存相关的设置，
 * 例如缓存位置、预缓存的时长等。
 */
class PreferencesDiskTab : public ConfigDialogBaseTab {
  Q_OBJECT
 public:
  /**
   * @brief PreferencesDiskTab 构造函数。
   *
   * 初始化磁盘选项卡的用户界面和设置。
   */
  PreferencesDiskTab();

  /**
   * @brief 验证当前选项卡中的设置是否有效。
   *
   * @return 如果设置有效则返回 true，否则返回 false。
   * 此函数会覆盖基类中的同名虚函数。
   */
  bool Validate() override;

  /**
   * @brief 应用当前选项卡中的更改。
   *
   * @param command 指向 MultiUndoCommand 对象的指针，用于记录撤销/重做操作。
   * 此函数会覆盖基类中的同名虚函数。
   */
  void Accept(MultiUndoCommand* command) override;

private:
  /**
   * @brief 指向磁盘缓存位置路径选择控件的指针。
   *
   * 用户可以通过此控件选择或输入磁盘缓存的存储路径。
   */
  PathWidget* disk_cache_location_;

  /**
   * @brief 指向前向缓存时长滑块控件的指针。
   *
   * 用于设置向前预缓存媒体内容的秒数或百分比。
   */
  FloatSlider* cache_ahead_slider_;

  /**
   * @brief 指向后向缓存时长滑块控件的指针。
   *
   * 用于设置向后已播放内容保留在缓存中的秒数或百分比。
   */
  FloatSlider* cache_behind_slider_;

  /**
   * @brief 指向默认磁盘缓存文件夹对象的指针。
   *
   * 可能用于存储和管理默认的磁盘缓存设置。
   */
  DiskCacheFolder* default_disk_cache_folder_;
};

}  // namespace olive

#endif  // PREFERENCESDISKTAB_H