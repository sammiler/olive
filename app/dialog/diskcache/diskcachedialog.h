#ifndef DISKCACHEDIALOG_H
#define DISKCACHEDIALOG_H

#include <QCheckBox>    // 复选框控件
#include <QDialog>      // QDialog 基类
#include <QPushButton>  // 按钮控件
#include <QWidget>      // 为了 QWidget* parent 参数
#include <QString>      // 为了路径参数

// 假设 diskmanager.h 声明了 DiskCacheFolder 类
#include "render/diskmanager.h"
// 假设 floatslider.h 声明了 FloatSlider 类
#include "widget/slider/floatslider.h"
// #include "common/define.h" // 如果需要 common/define.h 中的内容

namespace olive {

/**
 * @brief 用于配置特定磁盘缓存文件夹属性的对话框类。
 *
 * 此对话框允许用户查看和修改与特定磁盘缓存文件夹相关的设置，
 * 例如最大缓存大小限制，并提供手动清理该缓存文件夹的功能。
 */
class DiskCacheDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 DiskCacheDialog 对象。
   * @param folder 指向要配置的 DiskCacheFolder 对象的指针。对话框将显示并修改此文件夹的设置。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit DiskCacheDialog(DiskCacheFolder* folder, QWidget* parent = nullptr);

  // 默认析构函数通常足够，因为 Qt 的父子对象机制会处理内部的UI控件指针
  // ~DiskCacheDialog() override;

  /**
   * @brief 静态辅助函数，用于立即清理指定路径的磁盘缓存。
   *
   * 此函数可能会显示一个确认对话框，并在用户确认后执行清理操作。
   * @param path 要清理的磁盘缓存目录的路径。
   * @param parent 调用此函数的父 QWidget，用于模态对话框或消息提示。
   * @param clear_btn (可选) 指向触发此清理操作的 QPushButton 的指针。
   * 在清理过程中，此按钮可能会被临时禁用。
   */
  static void ClearDiskCache(const QString& path, QWidget* parent, QPushButton* clear_btn = nullptr);

 public slots:
  /**
   * @brief 重写 QDialog::accept() 槽函数。
   * 当用户点击“确定”或等效按钮以应用更改时调用。
   * 此函数会将对话框中设置的最大缓存大小等值保存到关联的 `folder_` 对象中。
   */
  void accept() override;

 private:
  /**
   * @brief 指向当前对话框正在配置的 DiskCacheFolder 对象的指针。
   */
  DiskCacheFolder* folder_;

  /**
   * @brief 指向 FloatSlider 控件的指针，用于设置最大缓存大小。
   */
  FloatSlider* maximum_cache_slider_;

  /**
   * @brief 指向 QCheckBox 控件的指针，可能用于在接受对话框时触发缓存清理。
   * (或者也可能是一个独立的“立即清理”选项，具体取决于UI设计)
   */
  QCheckBox* clear_disk_cache_;

  /**
   * @brief 指向“立即清理缓存” QPushButton 控件的指针。
   */
  QPushButton* clear_cache_btn_;

 private slots:
  /**
   * @brief 当用户点击 `clear_cache_btn_` 按钮时调用的槽函数。
   * 此函数将调用静态的 `ClearDiskCache` 方法来清理与 `folder_` 关联的磁盘缓存。
   */
  void ClearDiskCache();
};

}  // namespace olive

#endif  // DISKCACHEDIALOG_H