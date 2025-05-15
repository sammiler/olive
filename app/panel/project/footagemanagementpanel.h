#ifndef FOOTAGEMANAGEMENTPANEL_H // 防止头文件被重复包含的宏
#define FOOTAGEMANAGEMENTPANEL_H // 定义 FOOTAGEMANAGEMENTPANEL_H 宏

#include <QList> // Qt 列表容器 (虽然在此文件中未直接使用 QList 实例，但可能是相关的间接依赖或编码风格)

#include "node/project/footage/footage.h" // 包含 Footage 节点和 ViewerOutput (可能作为 Footage 的基类或接口) 的定义

namespace olive { // olive 项目的命名空间

/**
 * @brief FootageManagementPanel 类是一个纯虚基类 (接口)。
 *
 * 它定义了一个接口，用于那些需要管理或与选定素材 (Footage) 进行交互的面板。
 * 任何实现了这个接口的面板类，都必须提供 GetSelectedFootage() 方法的实现。
 *
 * 这个接口的作用是提供一个统一的方式来查询不同面板中当前选定的素材。
 * 例如，一个项目素材箱面板 (Project Bin) 和一个素材查看器面板 (FootageViewerPanel)
 * 可能都会实现这个接口，以便其他模块可以从中获取用户当前操作的素材信息。
 */
class FootageManagementPanel {
public:
  /**
   * @brief (纯虚函数) 获取当前在此面板中被选定或管理的素材。
   *
   * 实现此方法的派生类需要返回一个 ViewerOutput 指针的 QVector。
   * ViewerOutput 通常代表一个可以输出视频/音频帧的节点，对于素材而言，
   * 这很可能是 Footage 节点本身或其某个输出代理。
   *
   * @return 返回一个包含指向选定素材的 ViewerOutput 指针的 QVector。
   *         如果当前没有选定的素材，应返回一个空 QVector。
   */
  [[nodiscard]] virtual QVector<ViewerOutput *> GetSelectedFootage() const = 0;
  // `const = 0` 表明这是一个纯虚函数，并且是 const 成员函数。
  // 派生类必须实现这个函数。
};

}  // namespace olive

#endif  // FOOTAGEMANAGEMENTPANEL_H