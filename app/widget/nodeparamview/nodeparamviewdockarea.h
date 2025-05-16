#ifndef NODEPARAMVIEWDOCKAREA_H
#define NODEPARAMVIEWDOCKAREA_H

#include <QMainWindow> // Qt 主窗口类，此处用于其停靠部件功能

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWidget;
// class QMenu;
// class QDockWidget;

namespace olive {

// 注释解释了为什么继承 QMainWindow：
// This may look weird, but QMainWindow is just a QWidget with a fancy layout that allows
// for docking QDockWidgets
// (这可能看起来很奇怪，但 QMainWindow 只是一个带有高级布局的 QWidget，该布局允许停靠 QDockWidget)

/**
 * @brief NodeParamViewDockArea 类是一个专门用于节点参数视图中组织可停靠参数项的区域。
 *
 * 它继承自 QMainWindow，主要是为了利用 QMainWindow 强大的停靠窗口 (QDockWidget) 管理功能。
 * 在 NodeParamViewContext 中，这个区域会容纳并允许用户自由排列各个参数的编辑控件。
 */
class NodeParamViewDockArea : public QMainWindow {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit NodeParamViewDockArea(QWidget *parent = nullptr);

  /**
   * @brief 重写 QMainWindow 的 createPopupMenu 方法。
   *
   * QMainWindow 通常有一个上下文菜单，列出所有可用的停靠窗口和工具栏。
   * 此处重写可能是为了自定义该菜单，例如，只显示与参数项相关的停靠窗口。
   * @return 返回一个 QMenu 指针，表示创建的弹出式上下文菜单。
   */
  QMenu *createPopupMenu() override;

  /**
   * @brief 向此停靠区域添加一个新的可停靠项（通常是一个包含参数控件的 QDockWidget）。
   * @param item 指向要添加的 QDockWidget 的指针。
   */
  void AddItem(QDockWidget *item);
};

}  // namespace olive

#endif  // NODEPARAMVIEWDOCKAREA_H
