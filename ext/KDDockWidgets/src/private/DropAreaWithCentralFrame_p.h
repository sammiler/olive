#ifndef KD_DROP_AREA_WITH_CENTRAL_FRAME_P_H
#define KD_DROP_AREA_WITH_CENTRAL_FRAME_P_H

#include "DropArea_p.h" // 包含 DropArea 私有头文件

#include "kddockwidgets/QWidgetAdapter.h" // 包含 QWidget 和 QQuickItem 的适配器类

namespace KDDockWidgets {

/**
 * @brief 一个特殊的 DropArea，它包含一个中央框架 (Central Frame)。
 *
 * 通常用于 MainWindow，允许在中心区域放置一个小部件，并且周围可以停靠其他小部件。
 * 继承自 DropArea，并添加了对中央框架的特定管理。
 */
class DOCKS_EXPORT DropAreaWithCentralFrame : public DropArea
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数。
     * @param parent 父 QWidgetOrQuick 对象，默认为空。
     * @param options 主窗口选项，用于确定中央框架的行为，默认为 MainWindowOption_HasCentralFrame。
     */
    explicit DropAreaWithCentralFrame(QWidgetOrQuick *parent = {}, MainWindowOptions options = MainWindowOption_HasCentralFrame);
    /**
     * @brief 析构函数。
     */
    ~DropAreaWithCentralFrame() override;

    /**
     * @brief 创建一个中央框架。
     *
     * 这是一个静态辅助函数，根据给定的主窗口选项来创建并配置中央框架。
     * @param options 主窗口选项，用于配置中央框架的特性。
     * @return 指向新创建的 Frame 对象的指针，该 Frame 作为中央框架。
     */
    static Frame *createCentralFrame(MainWindowOptions options);

private:
    // 声明友元类，允许它们访问此类的私有成员
    friend class MainWindowBase; // 主窗口基类
    friend class Frame;          // 框架类
    friend class ::TestDocks;    // 测试类

    Frame *const m_centralFrame = nullptr; ///< 指向中央框架的常量指针。一旦创建，此指针不应再指向其他框架。
};

} // namespace KDDockWidgets

#endif // KD_DROP_AREA_WITH_CENTRAL_FRAME_P_H