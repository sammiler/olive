#pragma once // 保证此头文件在一次编译中仅被包含一次

#include <QWindow> // Qt 窗口基类

#ifdef KDDOCKWIDGETS_XLIB // 条件编译：仅当定义了 KDDOCKWIDGETS_XLIB 时才编译以下 X11 相关代码

#include "DockRegistry_p.h" // KDDockWidgets 内部停靠小部件注册表私有头文件

#include <QtGui/qpa/qplatformnativeinterface.h> // Qt 平台原生接口，用于访问底层平台资源

#include <X11/Xlib.h> // Xlib 库头文件，用于与 X Window 系统交互
#include <algorithm>  // C++ 标准库算法头文件 (例如 std::find_if)
#include <QVector>    // Qt 动态数组容器

namespace KDDockWidgets {

/**
 * @brief (静态函数) 递归地遍历 X11 窗口树，以确定窗口的 Z 顺序。
 *
 * 此函数从给定的当前窗口开始，查询其子窗口，并与 `remaining` 列表中的窗口进行匹配。
 * 匹配到的窗口会按照它们在 X11 窗口树中从下到上（通常对应于从后到前）的顺序被添加到 `result` 列表中。
 *
 * @param current 当前正在查询的 X11 窗口 ID (WId)。
 * @param disp 指向 X11 Display 结构的指针。
 * @param remaining 一个 QWindow 指针的 QVector，按引用传递。包含尚未排序的 KDDW 顶层窗口。
 * 在此函数执行过程中，找到的窗口会从此列表中移除。
 * @param result 一个 QWindow 指针的 QVector，按引用传递。用于存储按 Z 顺序排列的窗口。
 * 窗口会按照从 Z 轴底部到顶部的顺序被添加到此列表。
 */
static void travelTree(WId current, Display *disp, QVector<QWindow *> &remaining, QVector<QWindow *> &result)
{
    if (remaining.isEmpty()) // 如果所有待处理的窗口都已找到，则返回
        return;

    Window parent_return, root_return, *children_return; // 用于 XQueryTree 的输出参数
    unsigned int nchildren_return;                      // 子窗口的数量

    // 查询当前窗口的子窗口信息
    if (!XQueryTree(disp, current, &root_return, &parent_return,
                    &children_return, &nchildren_return)) {
        return; // 查询失败则返回
    }

    if (!children_return) // 如果没有子窗口，则返回
        return;

    // 遍历所有子窗口
    for (int i = 0; i < int(nchildren_return); ++i) {
        // XQueryTree 返回的子窗口可能远多于我们关心的顶层窗口，
        // 因此需要在 remaining 列表中查找匹配的窗口。
        // std::find_if 用于查找 remaining 列表中窗口 ID 与当前 X11 子窗口 ID 匹配的项。
        auto it = std::find_if(remaining.begin(), remaining.end(), [i, children_return](QWindow *window) {
            return window->winId() == children_return[i];
        });

        if (it != remaining.end()) { // 如果找到了匹配的 KDDW 窗口
            result << *it;           // 将其添加到结果列表
            remaining.erase(it);     // 从待处理列表中移除
        }

        // 递归地遍历当前子窗口的子树
        travelTree(children_return[i], disp, remaining, result);
    }

    // XQueryTree 分配的子窗口列表需要被释放
    if (children_return) {
        XFree(children_return);
    }
}

/**
 * @brief (静态函数) 获取当前的 X11 Display 指针。
 * @return 指向 X11 Display 结构的指针，如果获取失败则可能为 nullptr。
 */
static Display *x11Display()
{
    auto nativeInterface = qApp->platformNativeInterface(); // 获取 Qt 平台原生接口
    // 从原生接口获取名为 "display" 的原生资源，这在 X11 平台上通常是 Display*
    void *disp = nativeInterface->nativeResourceForIntegration(QByteArrayLiteral("display"));
    return reinterpret_cast<Display *>(disp); // 将 void* 转换为 Display*
}

/**
 * @brief (静态函数) 返回 KDDW 顶层窗口 (MainWindow 和浮动小部件) 按 Z 顺序排列的列表。
 * 向量的前面是 Z 轴较低（即更靠后）的窗口。
 * 此函数仅在链接了 Xlib 时有效。
 * @param ok 按引用输出，如果成功获取并排序所有窗口，则为 true；否则为 false。
 * @return 按 Z 顺序排列的 QWindow 指针的 QVector。
 */
static QVector<QWindow *> orderedWindows(bool &ok)
{
    ok = true; // 默认操作成功
    // 从 DockRegistry 获取所有 KDDW 的顶层窗口
    QVector<QWindow *> windows = DockRegistry::self()->topLevels();
    if (windows.isEmpty()) // 如果没有顶层窗口，直接返回空列表
        return {};

    QVector<QWindow *> orderedResult; // 用于存储排序后的结果
    Display *disp = x11Display();     // 获取 X11 Display 指针
    if (!disp) { // 如果获取 Display 失败
        ok = false;
        return {};
    }

    // 从根窗口开始遍历 X11 窗口树
    // windows 列表和 orderedResult 列表都通过引用传递，在 travelTree 中会被修改
    travelTree(DefaultRootWindow(disp), disp, /*by-ref*/ windows, /*by-ref*/ orderedResult);

    // 如果 travelTree 成功处理了 windows 列表中的所有窗口 (即 windows 变为空)，则操作成功
    ok = windows.isEmpty();
    return orderedResult; // 返回排序后的窗口列表
}
} // namespace KDDockWidgets

#else // 如果没有定义 KDDOCKWIDGETS_XLIB (例如在 Windows 或 macOS 等非 X11 平台)

namespace KDDockWidgets {
/**
 * @brief (静态函数) 占位函数，在没有链接 Xlib 的情况下被调用。
 * 仅用于使代码在 Windows 等平台上编译通过，避免更多的 #ifdef。
 * 此函数不应被实际调用，因此包含 Q_UNREACHABLE()。
 * @param ok 按引用输出，未使用。
 * @return 返回一个空的 QWindow 指针 QVector。
 */
static QVector<QWindow *> orderedWindows(bool &ok)
{
    Q_UNUSED(ok);      // 标记参数 ok 未被使用
    Q_UNREACHABLE();   // 标记此代码路径不应被执行
    return {};         // 返回空列表
}
} // namespace KDDockWidgets

#endif // KDDOCKWIDGETS_XLIB
