#ifndef COLORPREVIEWBOX_H
#define COLORPREVIEWBOX_H

#include <QWidget>  // Qt 控件基类

#include "render/colorprocessor.h"  // 引入色彩处理器类

// 前向声明 Qt 类
class QPaintEvent;

namespace olive {

/**
 * @brief ColorPreviewBox 类是一个用于显示颜色样本的自定义 Qt 控件。
 *
 * 该控件能够接收一个颜色值，并使用指定的色彩处理器 (ColorProcessor)
 * 将其转换为适合显示的颜色，然后在控件区域内绘制出来。
 * 它可以配置两个色彩处理器，一个用于参考色彩空间，一个用于显示色彩空间。
 */
class ColorPreviewBox : public QWidget {
 Q_OBJECT  // Qt 元对象系统宏，用于支持信号和槽机制

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit ColorPreviewBox(QWidget* parent = nullptr);

  /**
   * @brief 设置用于颜色转换的色彩处理器。
   * @param to_ref 指向用于将输入颜色转换为参考色彩空间的色彩处理器的智能指针。
   * @param to_display 指向用于将参考颜色转换为显示色彩空间的色彩处理器的智能指针。
   */
  void SetColorProcessor(ColorProcessorPtr to_ref, ColorProcessorPtr to_display);

 public slots:
  /**
   * @brief 设置要在预览框中显示的颜色。
   * @param c 要显示的 Color 对象。此颜色将通过配置的色彩处理器进行转换后显示。
   */
  void SetColor(const Color& c);

 protected:
  /**
   * @brief 绘制事件处理函数。
   *
   * 在此函数中，使用当前颜色和色彩处理器来绘制颜色样本。
   * @param e 绘制事件指针。
   */
  void paintEvent(QPaintEvent* e) override;

 private:
  Color color_;  ///< 存储当前要显示的原始颜色值。

  ColorProcessorPtr to_ref_processor_;      ///< 指向将输入颜色转换为参考色彩空间的色彩处理器的智能指针。
  ColorProcessorPtr to_display_processor_;  ///< 指向将参考颜色转换为最终显示色彩空间的色彩处理器的智能指针。
};

}  // namespace olive

#endif  // COLORPREVIEWBOX_H
