#pragma once // 确保该头文件在一次编译中仅被包含一次

#include "kddockwidgets/docks_export.h"      // 包含导出宏定义，用于库的导出和导入
#include "kddockwidgets/KDDockWidgets.h" // 包含 KDDockWidgets 库的主头文件
#include "Item_p.h"                          // 包含 Item 私有头文件 (Config 类可能与之有交互或作为友元)

#include <qglobal.h> // 包含 Qt 全局定义，例如 Q_DISABLE_COPY

namespace Layouting { // 布局相关的命名空间

class Separator; // 前向声明 Separator 类
class Widget;    // 前向声明 Widget 类 (可能是 KDDockWidgets::Widget 的别名或特定于布局的 Widget)

/**
 * @brief 定义一个函数指针类型 SeparatorFactoryFunc。
 *
 * 该函数指针用于创建 Separator 对象。
 * @param parent Separator 的父 Widget。
 * @return 指向新创建的 Separator 对象的指针。
 */
typedef Separator *(*SeparatorFactoryFunc)(Layouting::Widget *parent);

/**
 * @brief Config 类用于管理 KDDockWidgets 布局系统的全局配置。
 *
 * 这是一个单例类，提供对各种配置选项的访问，例如分隔条的厚度、
 * 分隔条的创建工厂函数以及行为标志。
 */
class DOCKS_EXPORT_FOR_UNIT_TESTS Config
{
public:
    /**
     * @brief 枚举类，定义配置标志。
     *
     * 这些标志可以用来控制布局系统的某些行为。
     */
    enum class Flag {
        None = 0,       ///< 无标志。
        LazyResize = 1  ///< 延迟调整大小标志。当设置时，某些调整大小的操作可能会被延迟执行以优化性能。
    };
    Q_DECLARE_FLAGS(Flags, Flag) // 为 Config::Flag 声明一个 Qt 标志类型 Config::Flags

    /**
     * @brief 返回 Config 类的单例实例。
     *
     * 这是获取 Config 对象以访问或修改配置的唯一方法。
     * @return 对 Config 单例对象的引用。
     */
    static Config &self();

    /**
     * @brief 析构函数。
     *
     * 在应用程序关闭时调用。由于是默认析构函数，它将执行标准的清理操作。
     */
    ~Config() = default;

    /**
     * @brief 返回分隔条的厚度（单位：像素）。
     *
     * 默认值为 5 像素。
     * @return 分隔条的厚度。
     */
    [[nodiscard]] static int separatorThickness();

    /**
     * @brief 设置分隔条的厚度。
     *
     * @warning 注意：此函数应仅在应用程序启动时，在创建任何布局项 (Item) 之前调用。
     * 在布局系统初始化后更改此值可能会导致不可预期的行为。
     * @param value 新的分隔条厚度值（单位：像素）。
     */
    static void setSeparatorThickness(int value);

    /**
     * @brief 设置内部用于创建分隔条的工厂函数。
     *
     * 这允许用户自定义分隔条的创建过程，例如使用自定义的 Separator 子类。
     * @param func 指向分隔条工厂函数的指针。
     */
    void setSeparatorFactoryFunc(SeparatorFactoryFunc);

    /**
     * @brief 返回当前配置的用于创建分隔条的工厂函数。
     *
     * 如果未设置自定义工厂函数，则默认为 nullptr。
     * @return 指向分隔条工厂函数的指针，如果未设置则为 nullptr。
     */
    [[nodiscard]] SeparatorFactoryFunc separatorFactoryFunc() const;

    /**
     * @brief 返回当前设置的配置标志。
     * @return 当前的 Config::Flags。
     */
    [[nodiscard]] Config::Flags flags() const;

    /**
     * @brief 设置配置标志。
     *
     * @warning 注意：此函数应仅在创建任何布局项 (Item) 之前调用。
     * @param flags 要设置的新标志。
     */
    void setFlags(Flags);

private:
    friend class Item;             // Item 类是友元类，可以访问 Config 的私有成员
    friend class ItemBoxContainer; // ItemBoxContainer 类是友元类

    /**
     * @brief 私有构造函数，确保 Config 类只能通过 self() 单例方法创建。
     */
    Config();

    /**
     * @brief 根据配置的工厂函数（如果存在）或默认方式创建 Separator 对象。
     * @param parent Separator 的父 Widget。
     * @return 指向新创建的 Separator 对象的指针。
     */
    Separator *createSeparator(Layouting::Widget *) const;

    /**
     * @brief 注册 QML 类型。
     *
     * 此函数用于在 QML 引擎中注册 KDDockWidgets 相关的类型，以便在 QML 中使用它们。
     * 通常在库初始化时调用。
     */
    void registerQmlTypes();

    SeparatorFactoryFunc m_separatorFactoryFunc = nullptr; ///< 存储分隔条工厂函数的指针。
    Flags m_flags = Flag::None;                          ///< 存储当前的配置标志。

    Q_DISABLE_COPY(Config) // 禁止拷贝构造函数和赋值操作符，以强制单例模式
};

} // namespace Layouting
