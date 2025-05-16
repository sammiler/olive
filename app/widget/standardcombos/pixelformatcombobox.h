#ifndef PIXELFORMATCOMBOBOX_H
#define PIXELFORMATCOMBOBOX_H

#include <QComboBox>  // Qt 组合框控件基类
#include <QVariant>   // Qt 通用数据类型类 (用于 addItem 的 userData)
#include <QWidget>    // Qt 控件基类 (QComboBox 的基类)

#include "render/videoparams.h"  // 视频参数定义 (包含 PixelFormat 枚举和 GetFormatName)

namespace olive {

/**
 * @brief PixelFormatComboBox 类是一个自定义的 QComboBox，用于选择像素格式。
 *
 * 它在构造时会根据 VideoParams 中定义的 PixelFormat 枚举来填充选项。
 * 可以选择只显示浮点类型的像素格式。
 */
class PixelFormatComboBox : public QComboBox {
 Q_OBJECT  // Qt 元对象系统宏 (虽然此类中没有显式定义信号或槽，但基类 QComboBox 有)

     public :
     /**
      * @brief 构造函数。
      *
      * 初始化组合框，并根据 float_only 参数填充支持的像素格式。
      * @param float_only 如果为 true，则只添加浮点类型的像素格式；否则添加所有支持的格式。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit PixelFormatComboBox(bool float_only, QWidget* parent = nullptr)
     : QComboBox(parent) {
    // 设置预览格式
    // 遍历 PixelFormat 枚举中定义的所有格式
    for (int i = 0; i < PixelFormat::COUNT; i++) {
      // 将整数 i 转换为 PixelFormat::Format 枚举类型
      auto pix_fmt = PixelFormat(static_cast<PixelFormat::Format>(i));

      // 如果不需要只显示浮点格式，或者当前格式是浮点格式
      if (!float_only || pix_fmt.is_float()) {
        // 将像素格式的名称（通过 VideoParams::GetFormatName 获取）和原始枚举值添加到组合框中
        // 枚举值作为 QVariant 存储在项的用户数据中
        this->addItem(VideoParams::GetFormatName(pix_fmt), static_cast<PixelFormat::Format>(i));
      }
    }
  }

  /**
   * @brief 获取当前选中的像素格式。
   * @return 返回 PixelFormat 对象。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] PixelFormat GetPixelFormat() const {
    // 从当前选中项的用户数据中获取整数值，并将其转换为 PixelFormat::Format 枚举类型，
    // 然后构造并返回一个 PixelFormat 对象。
    return PixelFormat(static_cast<PixelFormat::Format>(this->currentData().toInt()));
  }

  /**
   * @brief 根据给定的像素格式枚举值设置组合框的当前选中项。
   * @param fmt 要设置的 PixelFormat::Format 枚举值。
   */
  void SetPixelFormat(PixelFormat::Format fmt) {
    // 遍历组合框中的所有项
    for (int i = 0; i < this->count(); i++) {
      // 如果当前项的用户数据（即像素格式的枚举整数值）与给定的 fmt 匹配
      if (this->itemData(i).toInt() == static_cast<int>(fmt)) {
        this->setCurrentIndex(i);  // 设置此项为当前选中项
        break;                     // 找到匹配项后即可退出循环
      }
    }
  }
};

}  // namespace olive

#endif  // PIXELFORMATCOMBOBOX_H
