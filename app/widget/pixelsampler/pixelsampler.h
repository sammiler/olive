#ifndef PIXELSAMPLERWIDGET_H
#define PIXELSAMPLERWIDGET_H

#include <QGroupBox> // Qt 分组框控件基类
#include <QLabel>    // Qt 标签控件
#include <QWidget>   // Qt 控件基类

#include "widget/colorwheel/colorpreviewbox.h" // 自定义颜色预览框控件
#include "render/colorprocessor.h"             // 色彩处理器类 (虽然未直接在此头文件使用，但 Color 类型定义可能依赖它或在 .cpp 中使用)
#include "render/managedcolor.h"               // 带色彩空间管理的颜色类 (虽然未直接在此头文件使用 Color，但 Color 类型定义可能依赖它或在 .cpp 中使用)


// 前向声明项目内部类 (根据用户要求，不添加)
// class Color; // 通常在 render/managedcolor.h 或类似文件中定义

namespace olive {

/**
 * @brief PixelSamplerWidget 类是一个用于显示单个像素颜色采样值的控件。
 *
 * 它通常继承自 QGroupBox，包含一个颜色预览框 (ColorPreviewBox) 来直观显示颜色，
 * 以及一个标签 (QLabel) 来显示颜色的数值表示（例如 RGB 或 Hex 值）。
 */
class PixelSamplerWidget : public QGroupBox {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit PixelSamplerWidget(QWidget* parent = nullptr);

 public slots:
  /**
   * @brief 设置并显示指定的颜色值。
   *
   * 此槽函数会更新内部存储的颜色，并刷新颜色预览框和文本标签的显示。
   * @param color 要显示的 Color 对象。
   */
  void SetValues(const Color& color);

 private:
  /**
   * @brief 内部函数，用于根据当前存储的颜色值更新文本标签的显示。
   */
  void UpdateLabelInternal();

  Color color_; ///< 当前显示的颜色值。

  ColorPreviewBox* box_; ///< 用于显示颜色样本的预览框控件。
  QLabel* label_;        ///< 用于显示颜色数值（如 RGB、Hex）的标签控件。
};

/**
 * @brief ManagedPixelSamplerWidget 类是一个包含两个 PixelSamplerWidget 实例的复合控件。
 *
 * 它通常用于同时显示一个颜色在不同色彩空间或处理阶段的值，
 * 例如，一个“参考”颜色值和一个经过色彩管理转换后的“显示”颜色值。
 */
class ManagedPixelSamplerWidget : public QWidget {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit ManagedPixelSamplerWidget(QWidget* parent = nullptr);

 public slots:
  /**
   * @brief 设置并显示参考颜色和显示颜色。
   * @param reference 参考颜色值 (Color 对象)。
   * @param display 显示颜色值 (Color 对象)。
   */
  void SetValues(const Color& reference, const Color& display);

 private:
  PixelSamplerWidget* reference_view_; ///< 用于显示“参考”颜色的 PixelSamplerWidget 实例。
  PixelSamplerWidget* display_view_;   ///< 用于显示“显示”颜色（通常是经过色彩管理处理后）的 PixelSamplerWidget 实例。
};

}  // namespace olive

#endif  // PIXELSAMPLERWIDGET_H
