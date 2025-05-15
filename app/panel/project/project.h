#ifndef PROJECT_PANEL_H // 防止头文件被重复包含的宏
#define PROJECT_PANEL_H // 定义 PROJECT_PANEL_H 宏

#include "footagemanagementpanel.h"         // 包含 FootageManagementPanel 接口的定义
#include "node/project.h"                   // 包含 Project 类的定义
#include "panel/panel.h"                    // 包含 PanelWidget 基类的定义
#include "widget/projectexplorer/projectexplorer.h" // 包含 ProjectExplorer 控件的定义

namespace olive { // olive 项目的命名空间

/**
 * @brief ProjectPanel 类是一个 PanelWidget 的包装器，用于封装和管理一个项目浏览器 (ProjectExplorer)
 *        以及一个项目工具栏 (ProjectToolbar，虽然工具栏未在此头文件中直接体现，但描述中提及)。
 *
 * 这个面板是用户与项目内容 (如素材、序列、文件夹) 进行交互的主要界面之一，通常被称为“项目箱”或“素材库”。
 * 用户可以在这里查看项目结构、创建新项目项、组织内容到文件夹、以及对项目项执行各种操作。
 * 它实现了 FootageManagementPanel 接口，表明它可以提供当前选定的素材信息。
 */
class ProjectPanel : public PanelWidget, public FootageManagementPanel { // ProjectPanel 继承自 PanelWidget 并实现 FootageManagementPanel 接口
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  /**
   * @brief 构造函数。
   * @param unique_name 面板的唯一名称，可能用于窗口布局保存和恢复。
   */
  explicit ProjectPanel(const QString &unique_name);

  /**
   * @brief 获取此面板当前关联的项目。
   * @return 返回 Project 指针。
   */
  [[nodiscard]] Project *project() const;
  /**
   * @brief 设置此面板关联的项目。
   * 当用户打开或切换项目时，会调用此方法。
   * @param p 要设置的 Project 指针。
   */
  void set_project(Project *p);

  /**
   * @brief 获取项目浏览器中显示的根文件夹。
   * @return 返回根 Folder 指针。
   */
  [[nodiscard]] Folder *get_root() const;

  /**
   * @brief 设置项目浏览器中显示的根文件夹。
   * 通常用于显示项目的根目录或某个特定文件夹的内容。
   * @param item 要设置为根的 Folder 指针。
   */
  void set_root(Folder *item);

  /**
   * @brief 获取在项目浏览器中当前选中的所有项目项 (节点)。
   * @return 返回一个包含选中 Node 指针的 QVector。
   */
  [[nodiscard]] QVector<Node *> SelectedItems() const;

  /**
   * @brief 获取在项目浏览器中当前选中的文件夹。
   * 如果选中的不是文件夹，或者选中了多个项，行为可能不同 (例如返回 nullptr 或第一个选中的文件夹)。
   * @return 返回选中的 Folder 指针，如果没有选中文件夹则返回 nullptr。
   */
  [[nodiscard]] Folder *GetSelectedFolder() const;

  /**
   * @brief (重写自 FootageManagementPanel) 获取在项目浏览器中当前选中的素材。
   * @return 返回一个包含选中素材 (作为 ViewerOutput 指针) 的 QVector。
   */
  [[nodiscard]] QVector<ViewerOutput *> GetSelectedFootage() const override;

  /**
   * @brief 获取项目浏览器内部使用的视图模型 (ProjectViewModel)。
   * 视图模型负责提供项目数据给视图 (ProjectExplorer) 进行展示。
   * @return 返回 ProjectViewModel 指针。
   */
  [[nodiscard]] ProjectViewModel *model() const;

  /**
   * @brief 在项目浏览器中选择指定的项目项。
   * @param n 要选择的 Node 指针。
   * @param deselect_all_first (可选) 是否在选择前先取消所有其他选择，默认为 true。
   * @return 如果选择成功，返回 true。
   */
  bool SelectItem(Node *n, bool deselect_all_first = true) { return explorer_->SelectItem(n, deselect_all_first); }

  // --- 重写 PanelWidget 或通用编辑接口的方法 ---

  /**
   * @brief 全选项目浏览器中的所有可见项。
   */
  void SelectAll() override;
  /**
   * @brief 取消选择项目浏览器中的所有项。
   */
  void DeselectAll() override;

  /**
   * @brief 删除项目浏览器中当前选中的项。
   */
  void DeleteSelected() override;

  /**
   * @brief 重命名项目浏览器中当前选中的项。
   */
  void RenameSelected() override;

 public slots: // Qt 公有槽函数
  /**
   * @brief “编辑”指定的项目项。
   * 具体的编辑行为取决于项目项的类型 (例如，打开序列到时间轴，打开素材到查看器)。
   * @param item 要编辑的 Node 指针。
   */
  void Edit(Node *item);

 signals: // Qt 信号声明
  /**
   * @brief 当项目名称发生改变时发出的信号。
   */
  void ProjectNameChanged();

  /**
   * @brief 当项目浏览器中的选择项发生改变时发出的信号。
   * @param selected 当前所有被选中的 Node 指针列表。
   */
  void SelectionChanged(const QVector<Node *> &selected);

 private:
  /**
   * @brief 重写基类的 Retranslate 方法，用于在语言更改时更新此面板内UI元素的文本。
   */
  void Retranslate() override;

  ProjectExplorer *explorer_; // 指向内部的项目浏览器控件的指针
                              // ProjectExplorer 负责显示项目内容并处理用户交互。

 private slots: // Qt 私有槽函数
  /**
   * @brief (静态槽函数) 当项目浏览器中的某个项被双击时的处理槽函数。
   * @param item 被双击的 Node 指针。
   */
  static void ItemDoubleClickSlot(Node *item);

  /**
   * @brief 显示“新建”菜单的槽函数 (例如，新建序列、新建文件夹等)。
   */
  void ShowNewMenu();

  /**
   * @brief 更新字幕相关信息的槽函数 (具体功能需看实现)。
   */
  void UpdateSubtitle();

  /**
   * @brief (静态槽函数) 保存当前连接 (或关联) 的项目。
   */
  static void SaveConnectedProject();
};

}  // namespace olive

#endif  // PROJECT_PANEL_H