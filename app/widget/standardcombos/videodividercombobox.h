#ifndef VIDEODIVIDERCOMBOBOX_H
#define VIDEODIVIDERCOMBOBOX_H

#include <QComboBox>  // Qt 组合框控件基类
#include <QVariant>   // Qt 通用数据类型类 (用于 addItem 的 userData)
#include <QWidget>    // Qt 控件基类 (QComboBox 的基类)

#include "render/videoparams.h"  // 视频参数定义 (包含 kSupportedDividers 和 GetNameForDivider)

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QString; // VideoParams::GetNameForDivider 返回类型，addItem 参数

namespace olive {

/**
 * @brief VideoDividerComboBox 类是一个自定义的 QComboBox，用于选择视频分辨率的除数（分频系数）。
 *
 * 例如，用户可以选择 "Full", "1/2", "1/4" 等来降低预览分辨率以提高性能。
 * 它在构造时会从 VideoParams::kSupportedDividers 中获取支持的除数值，
 * 并使用 VideoParams::GetNameForDivider 将其转换为用户可读的字符串，然后添加到下拉列表中。
 */
class VideoDividerComboBox : public QComboBox {
 Q_OBJECT  // Qt 元对象系统宏 (虽然此类中没有显式定义信号或槽，但基类 QComboBox 有)

     public :
     /**
      * @brief 构造函数。
      *
      * 初始化组合框，并从 VideoParams::kSupportedDividers 中获取支持的除数值，
      * 使用 VideoParams::GetNameForDivider 将其转换为用户可读的字符串，然后添加到下拉列表中。
      * 每个列表项的用户数据 (userData) 存储的是原始的 int 除数值。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit VideoDividerComboBox(QWidget* parent = nullptr)
     : QComboBox(parent) {
    // 遍历 VideoParams 中定义的所有支持的除数值
    foreach (int d, VideoParams::kSupportedDividers) {
      // 将 int 类型的除数值转换为用户可读的字符串（例如 "Full", "1/2"），并添加到组合框中
      // 同时，将原始的 int 值作为 QVariant 存储在对应项的用户数据中
      this->addItem(VideoParams::GetNameForDivider(d), d);
    }
  }

  /**
   * @brief 获取当前选中的视频除数值。
   * @return 返回 int 类型的除数值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] int GetDivider() const { return this->currentData().toInt(); }

  /**
   * @brief 根据给定的除数值设置组合框的当前选中项。
   * @param d 要设置的 int 类型的除数值。
   */
  void SetDivider(int d) {
    // 遍历组合框中的所有项
    for (int i = 0; i < this->count(); i++) {
      // 如果当前项的用户数据（即除数值）与给定的 d 匹配
      if (this->itemData(i).toInt() == d) {
        this->setCurrentIndex(i);  // 设置此项为当前选中项
        break;                     // 找到匹配项后即可退出循环
      }
    }
  }
};

}  // namespace olive

#endif  // VIDEODIVIDERCOMBOBOX_H
