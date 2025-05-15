#ifndef PROJECTPROPERTIESDIALOG_H
#define PROJECTPROPERTIESDIALOG_H

#include <QCheckBox>     // 引入 QCheckBox 类，用于创建复选框控件
#include <QComboBox>     // 引入 QComboBox 类，用于创建下拉列表框控件
#include <QDialog>       // 引入 QDialog 类，对话框窗口的基类
#include <QGridLayout>   // 引入 QGridLayout 类，用于栅格布局
#include <QLineEdit>     // 引入 QLineEdit 类，用于创建单行文本输入框控件
#include <QRadioButton>  // 引入 QRadioButton 类，用于创建单选按钮控件

#include "node/project.h"             // 引入项目类定义
#include "widget/path/pathwidget.h"   // 引入自定义的路径选择控件

namespace olive {

/**
 * @class ProjectPropertiesDialog
 * @brief “项目属性”对话框类。
 *
 * 此对话框用于显示和修改当前项目的各种属性，
 * 例如颜色管理设置 (OCIO)、磁盘缓存路径等。
 */
class ProjectPropertiesDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief ProjectPropertiesDialog 构造函数。
   *
   * @param p 指向当前项目 (Project) 对象的指针。
   * @param parent 父 QWidget 对象指针。
   */
  ProjectPropertiesDialog(Project* p, QWidget* parent);

 public slots:
  /**
   * @brief 重写 QDialog 的 accept() 槽函数。
   *
   * 当用户点击“确定”按钮时调用，用于应用对话框中的更改到项目。
   */
  void accept() override;

 private:
  /**
   * @brief 验证提供的路径是否有效，如果无效则发出警告。
   *
   * @param path 要验证的文件或文件夹路径。
   * @return 如果路径有效则返回 true，否则返回 false。
   */
  bool VerifyPathAndWarnIfBad(const QString& path);

  /**
   * @brief 指向当前正在编辑其属性的项目对象的指针。
   */
  Project* working_project_;

  /**
   * @brief 用于显示和编辑 OCIO 配置文件名的行编辑控件。
   */
  QLineEdit* ocio_filename_;

  /**
   * @brief 用于选择默认输入色彩空间的下拉选择框控件。
   */
  QComboBox* default_input_colorspace_;

  /**
   * @brief 用于选择参考色彩空间的下拉选择框控件。
   */
  QComboBox* reference_space_;

  /**
   * @brief 标记 OCIO 配置是否有效的布尔值。
   */
  bool ocio_config_is_valid_;

  /**
   * @brief 存储 OCIO 配置相关的错误信息。
   */
  QString ocio_config_error_;

  /**
   * @brief 指向自定义缓存路径选择控件的指针。
   */
  PathWidget* custom_cache_path_;

  /**
   * @brief 磁盘缓存选项单选按钮的数量。
   */
  static const int kDiskCacheRadioCount = 3;
  /**
   * @brief 存储磁盘缓存选项的单选按钮数组。
   * 例如：使用项目默认位置、使用全局设置位置、使用自定义位置。
   */
  QRadioButton* disk_cache_radios_[kDiskCacheRadioCount]{};

 private slots:
  /**
   * @brief 私有槽函数：浏览并选择 OCIO 配置文件。
   */
  void BrowseForOCIOConfig();

  /**
   * @brief 私有槽函数：当 OCIO 配置文件名更新时调用。
   *
   * 用于验证新的 OCIO 配置并更新相关的 UI 元素。
   */
  void OCIOFilenameUpdated();

  /**
   * @brief 私有槽函数：打开磁盘缓存设置对话框或相关的首选项页面。
   */
  void OpenDiskCacheSettings();
};

}  // namespace olive

#endif  // PROJECTPROPERTIESDIALOG_H
