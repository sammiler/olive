/**
 * @file
 * @brief 用于保存和恢复停靠小部件布局的类。
 *
 * @author Sérgio Martins <sergio.martins@kdab.com>
 */

#ifndef KD_LAYOUTSAVER_H
#define KD_LAYOUTSAVER_H

#include "docks_export.h" // 导入导出宏定义，用于库的符号可见性

#include "KDDockWidgets.h" // KDDockWidgets 公共头文件，包含核心枚举和类型定义

QT_BEGIN_NAMESPACE
// 前向声明 Qt 类
class QByteArray; // Qt 字节数组类，用于内存中的序列化数据
QT_END_NAMESPACE

// 前向声明，用于测试目的
class TestDocks;

namespace KDDockWidgets {

// 前向声明 KDDockWidgets 内部或公共类
class DockWidgetBase; // 停靠小部件基类


/**
 * @brief LayoutSaver 类允许保存或恢复布局。
 *
 * 您可以将布局保存到文件或字节数组。
 * JSON 被用作序列化格式。
 *
 * 示例:
 * LayoutSaver saver;
 *
 * // 保存到文件:
 * saver.saveToFile(filename);
 *
 * saveToFile() 的对应操作是 restoreFromFile()。
 *
 * 您也可以使用 serializeLayout() 将布局保存到 QByteArray。
 * serializeLayout() 的对应操作是 restoreLayout()。
 */
class DOCKS_EXPORT LayoutSaver
{
public:
    /**
     * @brief 构造函数。建议在栈上构造。
     * @param options 恢复布局时使用的选项，默认为 RestoreOption_None。
     */
    explicit LayoutSaver(RestoreOptions options = RestoreOption_None);

    /**
     * @brief 析构函数。
     */
    ~LayoutSaver();

    /**
     * @brief (静态方法) 返回恢复操作 (@ref restoreLayout) 当前是否正在进行中。
     * @return 如果正在恢复布局，则返回 true；否则返回 false。
     */
    static bool restoreInProgress();

    /**
     * @brief 将布局保存到 JSON 文件。
     * @param jsonFilename 将布局保存到的文件名。
     * @return 如果成功保存，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool saveToFile(const QString &jsonFilename) const;

    /**
     * @brief 从 JSON 文件恢复布局。
     * @param jsonFilename 包含已保存布局的文件名。
     * @return 如果成功恢复，则返回 true；否则返回 false。
     */
    bool restoreFromFile(const QString &jsonFilename);

    /**
     * @brief 将布局序列化为字节数组。
     * @return 包含序列化布局数据的 QByteArray (JSON格式)。
     */
    [[nodiscard]] QByteArray serializeLayout() const;

    /**
     * @brief 从字节数组恢复布局。
     * 在调用此函数之前，所有相关的 MainWindow 和 DockWidget 都应该已经被创建。
     *
     * 如果不能预先创建所有 DockWidget，请确保通过 Config::setDockWidgetFactoryFunc()
     * 设置一个 DockWidget 工厂函数。
     *
     * @sa Config::setDockWidgetFactoryFunc()
     * @param jsonData 包含序列化布局数据的 QByteArray。
     * @return 如果成功恢复，则返回 true；否则返回 false。
     */
    bool restoreLayout(const QByteArray &jsonData);

    /**
     * @brief (静态方法) 返回自上次调用 @ref restoreLayout() 或 @ref restoreFromFile() 以来已恢复的停靠小部件列表。
     *
     * 这很有用，因为某些停靠小部件可能是新的，因此未包含在上次保存的布局中。
     * @return 包含已恢复 DockWidgetBase 指针的 QVector。
     */
    [[nodiscard]] static QVector<DockWidgetBase *> restoredDockWidgets();

    /**
     * @brief 设置将对其应用恢复和保存操作的亲和性名称列表。
     * 允许仅保存/恢复窗口的子集。
     * 默认情况下为空，所有窗口都将参与保存/恢复。
     * 任何具有空亲和性的窗口也将参与保存/恢复，无论 @p affinityNames 如何设置。
     * @param affinityNames 要应用操作的亲和性名称列表。
     */
    void setAffinityNames(const QStringList &affinityNames);

    /// @internal 内部使用：返回私有实现类的指针。不应在公共 API 中使用。
    class Private; // PIMPL 私有实现类的前向声明
    /**
     * @internal
     * @brief 获取指向私有实现类 (PIMPL) 的指针。
     * @return 指向 Private 实例的指针。
     */
    [[nodiscard]] Private *dptr() const;

    // 以下是用于序列化/反序列化的嵌套数据结构声明。
    // 它们的定义通常在 LayoutSaver_p.h 中。
    struct Layout;         ///< 代表整个应用程序布局的结构体。
    struct MainWindow;     ///< 代表单个主窗口状态的结构体。
    struct FloatingWindow; ///< 代表单个浮动窗口状态的结构体。
    struct DockWidget;     ///< 代表单个停靠小部件状态的结构体。
    struct Position;       ///< 代表停靠小部件位置信息的结构体。
    struct MultiSplitter;  ///< 代表多重分割器布局状态的结构体。
    struct Frame;          ///< 代表框架（停靠小部件容器）状态的结构体。
    struct Placeholder;    ///< 代表布局中占位符信息的结构体。
    struct ScalingInfo;    ///< 代表窗口缩放信息的结构体。
    struct ScreenInfo;     ///< 代表屏幕信息的结构体。

private:
    Q_DISABLE_COPY(LayoutSaver) ///< 禁止拷贝构造函数和拷贝赋值操作符。
    friend class ::TestDocks;   ///< 声明测试类为友元，允许访问私有成员。

    Private *const d; ///< 指向私有实现类的常量指针。
};
} // namespace KDDockWidgets

#endif // KD_LAYOUTSAVER_H
