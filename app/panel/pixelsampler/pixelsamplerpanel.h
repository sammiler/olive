#ifndef PIXELSAMPLERPANEL_H  // 防止头文件被重复包含的宏
#define PIXELSAMPLERPANEL_H  // 定义 PIXELSAMPLERPANEL_H 宏

#include "panel/panel.h"                       // 包含 PanelWidget 基类的定义
#include "widget/pixelsampler/pixelsampler.h"  // 包含 PixelSampler 相关控件的定义
                                               // (可能是 ManagedPixelSamplerWidget 或其基类 PixelSampler)

namespace olive {  // olive 项目的命名空间

/**
 * @brief PixelSamplerPanel 类代表一个用于显示像素颜色采样器信息的面板。
 *
 * 它继承自 PanelWidget，是 Olive 编辑器中的一个标准面板。
 * 这个面板通常会显示从图像或视频查看器中拾取的像素的颜色值。
 * 它可能会显示多种色彩空间下的颜色值 (例如，原始值、参考空间值、显示空间值)
 * 以及像素的坐标等信息。
 *
 * 内部会包含一个像素采样器相关的UI控件 (例如 ManagedPixelSamplerWidget)。
 */
class PixelSamplerPanel : public PanelWidget {  // PixelSamplerPanel 继承自 PanelWidget
 Q_OBJECT                                       // 声明此类使用 Qt 的元对象系统

     public :
     // 构造函数
     PixelSamplerPanel();

 public slots:  // Qt 公有槽函数
  /**
   * @brief 设置面板中显示的颜色值。
   * 通常由查看器或其他颜色拾取工具在用户拾取像素后调用此槽函数来更新显示。
   * @param reference 像素在参考色彩空间中的颜色值。
   * @param display 像素在显示色彩空间中的颜色值 (经过显示变换后的颜色)。
   */
  void SetValues(const Color& reference, const Color& display);

 private:
  /**
   * @brief 重写基类的 Retranslate 方法，用于在语言更改时更新此面板内UI元素的文本。
   * 例如，可能会更新面板标题或内部标签文本 (如 "参考值:", "显示值:")。
   */
  void Retranslate() override;

  ManagedPixelSamplerWidget* sampler_widget_;  // 指向内部的像素采样器UI控件的指针
  // 这个控件负责实际显示颜色值和相关信息。
};

}  // namespace olive

#endif  // PIXELSAMPLERPANEL_H