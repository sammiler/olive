#ifndef MULTICAMPANEL_H // 防止头文件被重复包含的宏
#define MULTICAMPANEL_H // 定义 MULTICAMPANEL_H 宏

#include "panel/viewer/viewerbase.h"      // 包含查看器面板基类 (或相关时间轴面板基类 TimeBasedPanel) 的定义
                                          // 根据实际继承关系，这里可能是 TimeBasedPanel 而非 ViewerPanelBase
#include "widget/multicam/multicamwidget.h" // 包含 MulticamWidget 控件的定义

// 修正 include 以匹配类声明中的继承
// 如果 MulticamPanel 继承自 TimeBasedPanel，那么应该包含 "panel/timebased/timebased.h"
// 此处注释假设其继承自 TimeBasedPanel，与提供的代码一致。
// 如果实际是 ViewerPanelBase，则 panel/viewer/viewerbase.h 是正确的。
// 根据代码是 TimeBasedPanel:
#include "panel/timebased/timebased.h"

namespace olive { // olive 项目的命名空间

/**
 * @brief MulticamPanel 类代表一个用于多机位编辑的面板。
 *
 * 它继承自 TimeBasedPanel，表明这是一个与时间相关的面板，可能包含时间轴导航功能。
 * 这个面板内部会包含一个 MulticamWidget 控件，该控件负责同时显示多个视频源 (机位)，
 * 并允许用户在播放过程中通过点击不同的机位来切换活动机位，从而快速完成多机位剪辑。
 */
class MulticamPanel : public TimeBasedPanel { // MulticamPanel 继承自 TimeBasedPanel
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  // 构造函数
  MulticamPanel();

  /**
   * @brief 获取内部封装的 MulticamWidget 控件的指针。
   * 通过动态类型转换从基类 (TimeBasedPanel) 提供的 GetTimeBasedWidget() 方法获取。
   * @return 返回 MulticamWidget 指针，如果转换失败或内部控件不是 MulticamWidget 类型，则返回 nullptr。
   */
  [[nodiscard]] MulticamWidget *GetMulticamWidget() const {
    // 将基类提供的 TimeBasedWidget 动态转换为 MulticamWidget
    return dynamic_cast<MulticamWidget *>(GetTimeBasedWidget());
  }

protected:
  /**
   * @brief 重写基类的 Retranslate 方法，用于在语言更改时更新此面板内UI元素的文本。
   * 例如，可能会更新面板标题或内部控件的标签。
   */
  void Retranslate() override;

  // 注意：实际的 MulticamWidget 成员变量通常在构造函数中创建，并通过 TimeBasedPanel
  // (或其父类 PanelWidget) 的机制 (如 setCentralWidget 或类似的) 设置为面板的中心内容。
};

}  // namespace olive

#endif  // MULTICAMPANEL_H