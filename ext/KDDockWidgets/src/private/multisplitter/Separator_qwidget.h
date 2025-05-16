#ifndef KD_MULTISPLITTER_SEPARATORQWIDGET_P_H
#define KD_MULTISPLITTER_SEPARATORQWIDGET_P_H

#ifdef KDDOCKWIDGETS_QTWIDGETS // 仅当定义了 KDDOCKWIDGETS_QTWIDGETS 宏时才编译以下内容 (表示在 Qt Widgets 环境下)

#include "kddockwidgets/docks_export.h"      // 包含导出宏定义
#include "Separator_p.h"                     // 包含 Separator 私有头文件 (SeparatorWidget 的基类之一)
#include "Widget_qwidget.h"                  // 包含 Widget_qwidget.h (SeparatorWidget 和 RubberBand 的基类之一，用于 Qt Widgets 的 Widget 适配)
#include "kddockwidgets/Qt5Qt6Compat_p.h"    // 包含 Qt5/Qt6 兼容性辅助头文件

#include <QRubberBand> // 包含 QRubberBand 类，用于创建橡皮筋选择框

// clazy:excludeall=ctor-missing-parent-argument
// 上一行是一个 clazy (静态分析器) 指令，用于在此文件中排除特定的警告，
// 即关于构造函数缺少父 QObject* 参数的警告。

namespace Layouting { // 布局相关的命名空间

/**
 * @brief SeparatorWidget 类是 Layouting::Separator 在 Qt Widgets 环境下的具体实现。
 *
 * 此类继承自 QWidget，使其能够作为标准的 Qt Widget 在用户界面中显示和交互。
 * 同时，它继承自 Layouting::Separator 来实现分隔条的核心逻辑，
 * 并继承自 Layouting::Widget_qwidget 以便与 KDDockWidgets 的 Qt Widgets Widget 框架集成。
 *
 * 它负责处理鼠标事件以允许用户拖动分隔条来调整相邻布局项的大小，
 * 并在需要时绘制自身。
 */
class DOCKS_EXPORT SeparatorWidget
    : public QWidget,                 // 继承自 QWidget，使其成为一个标准的 Qt 部件
      public Layouting::Separator,     // 继承自 Layouting::Separator，获取分隔条的核心逻辑
      public Layouting::Widget_qwidget // 继承自 Layouting::Widget_qwidget，作为 Qt Widgets 的 Widget 适配器
{
    Q_OBJECT // 启用 Qt 元对象系统特性
public:
    /**
     * @brief 构造一个 SeparatorWidget 对象。
     * @param parent 指向父 Layouting::Widget 对象的指针，默认为 nullptr。
     * 注意：这里的 parent 是逻辑上的父 Widget，而不是 QWidget 的可视化父项。
     * QWidget 的父项会在 Widget_qwidget 基类中或稍后设置。
     */
    explicit SeparatorWidget(Layouting::Widget *parent = nullptr);

protected:
    /**
     * @brief 重写 QWidget::paintEvent()，用于绘制分隔条的视觉外观。
     * @param event 绘制事件对象。
     */
    void paintEvent(QPaintEvent *) override;

    /**
     * @brief 重写 QWidget::enterEvent()，处理鼠标进入分隔条区域的事件。
     *
     * 通常用于改变鼠标光标样式，以提示用户该分隔条可拖动。
     * @param event 鼠标进入事件对象 (使用 Qt5/Qt6 兼容类型)。
     */
    void enterEvent(KDDockWidgets::Qt5Qt6Compat::QEnterEvent *) override;

    /**
     * @brief 重写 QWidget::leaveEvent()，处理鼠标离开分隔条区域的事件。
     *
     * 通常用于恢复默认的鼠标光标样式。
     * @param event 鼠标离开事件对象。
     */
    void leaveEvent(QEvent *) override;

    /**
     * @brief 重写 QWidget::mousePressEvent()，处理鼠标在分隔条上按下的事件。
     *
     * 调用基类 Layouting::Separator::onMousePress() 来启动拖动逻辑。
     * @param event 鼠标按下事件对象。
     */
    void mousePressEvent(QMouseEvent *) override;

    /**
     * @brief 重写 QWidget::mouseMoveEvent()，处理鼠标在分隔条上拖动的事件。
     *
     * 调用基类 Layouting::Separator::onMouseMove() 来更新分隔条位置。
     * @param event 鼠标移动事件对象。
     */
    void mouseMoveEvent(QMouseEvent *) override;

    /**
     * @brief 重写 QWidget::mouseReleaseEvent()，处理在分隔条上释放鼠标按键的事件。
     *
     * 调用基类 Layouting::Separator::onMouseReleased() 来结束拖动逻辑并应用更改。
     * @param event 鼠标释放事件对象。
     */
    void mouseReleaseEvent(QMouseEvent *) override;

    /**
     * @brief 重写 QWidget::mouseDoubleClickEvent()，处理在分隔条上双击鼠标的事件。
     *
     * 调用基类 Layouting::Separator::onMouseDoubleClick()。
     * @param event 鼠标双击事件对象。
     */
    void mouseDoubleClickEvent(QMouseEvent *) override;

    /**
     * @brief 创建一个橡皮筋 (RubberBand) 部件，用于在拖动分隔条时提供视觉反馈。
     *
     * 重写基类 Layouting::Separator::createRubberBand 方法。
     * @param parent 橡皮筋的父 Widget。
     * @return 指向新创建的 Layouting::RubberBand (QRubberBand 的包装器) 指针。
     */
    Widget *createRubberBand(Widget *parent) override;

    /**
     * @brief 返回代表此 SeparatorWidget 实例的 Layouting::Widget 接口。
     *
     * 重写基类 Layouting::Separator::asWidget 方法。
     * @return 指向自身的 Layouting::Widget 指针 (通过 Widget_qwidget 基类实现)。
     */
    Widget *asWidget() override;
};

/**
 * @brief RubberBand 类是 QRubberBand 的一个包装类，同时实现了 Layouting::Widget_qwidget 接口。
 *
 * 这使得标准的 QRubberBand 能够被 KDDockWidgets 的布局系统识别和管理为一个 Layouting::Widget。
 * 它主要用于在拖动分隔条时提供临时的视觉反馈。
 */
class RubberBand : public QRubberBand, public Layouting::Widget_qwidget
{
    Q_OBJECT // 启用 Qt 元对象系统特性
public:
    /**
     * @brief 构造一个 RubberBand 对象。
     * @param parent 指向父 Layouting::Widget 对象的指针。
     * QRubberBand 的实际父 QWidget 会在 Widget_qwidget 基类中或稍后设置。
     */
    explicit RubberBand(Layouting::Widget *parent);
};

} // namespace Layouting

#endif // KDDOCKWIDGETS_QTWIDGETS


#endif // KD_MULTISPLITTER_SEPARATORQWIDGET_P_H
