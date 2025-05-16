#ifndef PROJECTEXPLORER_H
#define PROJECTEXPLORER_H

#include <QAbstractItemView>      // Qt 抽象项目视图基类 (AddView 参数)
#include <QList>                  // Qt 列表容器
#include <QModelIndex>            // Qt 模型索引类
#include <QSortFilterProxyModel>  // Qt 排序和过滤代理模型类
#include <QStackedWidget>         // Qt 堆叠控件，用于切换不同的视图模式
#include <QTimer>                 // Qt 定时器类 (虽然在此头文件未直接使用，但可能在 .cpp 或相关类中使用)
#include <QTreeView>              // Qt 树形视图控件基类 (ProjectExplorerTreeView 的基类)
#include <QVector>                // Qt 动态数组容器
#include <QWidget>                // Qt 控件基类

#include "node/project.h"                                      // 项目类定义 (包含 Project, Folder, Node, Block 等)
#include "projectviewmodel.h"                                  // 项目视图模型类
#include "undo/undostack.h"                                    // 撤销栈 (MultiUndoCommand 可能需要)
#include "widget/projectexplorer/projectexplorericonview.h"    // 项目浏览器图标视图
#include "widget/projectexplorer/projectexplorerlistview.h"    // 项目浏览器列表视图
#include "widget/projectexplorer/projectexplorernavigation.h"  // 项目浏览器导航栏
#include "widget/projectexplorer/projectexplorertreeview.h"    // 项目浏览器树形视图
#include "widget/projecttoolbar/projecttoolbar.h"              // 项目工具栏 (包含 ViewType 枚举)

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QAction;
// class QEvent;
// class QMouseEvent;
// class QDragEnterEvent;
// class QDragMoveEvent;
// class QDropEvent;
// class QDragLeaveEvent;
// class QResizeEvent;
// class QObject;
// class QString;

// 前向声明项目内部类 (根据用户要求，不添加)
// class MultiUndoCommand; // 已包含 undo/undostack.h

namespace olive {

/**
 * @brief ProjectExplorer 类是一个用于浏览项目结构的控件。
 *
 * ProjectExplorer 自动处理视图<->模型系统，使用 ProjectViewModel。
 * 因此，只需要提供 Project 结构本身。
 *
 * 此控件包含三种视图：树视图、列表视图和图标视图。这些视图可以随时切换。
 */
class ProjectExplorer : public QWidget {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针。
      */
     explicit ProjectExplorer(QWidget* parent);

  /**
   * @brief 获取当前活动的视图类型（树状、列表或图标）。
   * @return 返回 ProjectToolbar::ViewType 枚举值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const ProjectToolbar::ViewType& view_type() const;

  /**
   * @brief 获取当前与此浏览器关联的项目。
   * @return 返回 Project 指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Project* project() const;
  /**
   * @brief 设置此浏览器要显示的项目。
   * @param p 指向 Project 对象的指针。
   */
  void set_project(Project* p);

  /**
   * @brief 获取当前视图的根文件夹。
   * @return 返回 Folder 指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Folder* get_root() const;
  /**
   * @brief 设置当前视图的根文件夹（例如，当导航到子文件夹时）。
   * @param item 指向要设置为根的 Folder 对象的指针。
   */
  void set_root(Folder* item);

  /**
   * @brief 获取当前所有选中的项目项（节点）。
   * @return 返回一个包含选中 Node 指针的 QVector。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QVector<Node*> SelectedItems() const;

  /**
   * @brief 使用启发式方法确定哪个文件夹（如果有的话）被选中。
   *
   * 通常在某些导入/添加过程中，我们假设如果选中了一个文件夹，
   * 用户可能希望在其中创建新对象，而不是在根目录中。
   * 然而，如果选中了多个文件夹，我们无法真正确定任何一个文件夹，
   * 此时会返回根目录。
   *
   * @return
   *
   * 一个通过启发式方法确定为“选中”的 Folder 指针，
   * 如果没有选中文件夹则为根目录，如果未打开项目则为 nullptr。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Folder* GetSelectedFolder() const;

  /**
   * @brief 访问项目的 ViewModel 模型。
   * @return 返回 ProjectViewModel 指针。
   */
  ProjectViewModel* model();

  /**
   * @brief 选中当前视图中的所有项。
   */
  void SelectAll();

  /**
   * @brief 取消选中当前视图中的所有项。
   */
  void DeselectAll();

  /**
   * @brief 删除当前选中的项（用户友好的/可撤销的操作）。
   */
  void DeleteSelected();

  /**
   * @brief 选中指定的项目项。
   * @param n 要选中的 Node 指针。
   * @param deselect_all_first 如果为 true（默认），则在选中此项前先取消所有其他项的选中状态。
   * @return 如果成功选中项，则返回 true。
   */
  bool SelectItem(Node* n, bool deselect_all_first = true);

 public slots:
  /**
   * @brief 设置项目浏览器的视图类型（例如，树状、列表、图标）。
   * @param type ProjectToolbar::ViewType 枚举值。
   */
  void set_view_type(ProjectToolbar::ViewType type);

  /**
   * @brief 编辑指定的项目项（通常是触发重命名操作）。
   * @param item 要编辑的 Node 指针。
   */
  void Edit(Node* item);

  /**
   * @brief 重命名当前选中的项目项。
   */
  void RenameSelectedItem();

  /**
   * @brief 设置搜索过滤器文本。
   *
   * 视图将只显示与过滤文本匹配的项目。
   * @param s 搜索过滤字符串。
   */
  void SetSearchFilter(const QString& s);

 signals:
  /**
   * @brief 当某个项目项被双击时发出此信号。
   *
   * @param item
   *
   * 被双击的 Node 指针，如果双击的是空白区域则为 nullptr。
   */
  void DoubleClickedItem(Node* item);

  /**
   * @brief 当选择的项目项发生改变时发出此信号。
   * @param selected 包含新选中的 Node 指针的 QVector。
   */
  void SelectionChanged(const QVector<Node*>& selected);

 private:
  /**
   * @brief 获取所有仅依赖于单个输入节点的 Block（剪辑块）。
   *
   * 忽略依赖于多个输入的 Block。
   * @param nodes 要检查的 Node 列表。
   * @return 返回一个包含符合条件的 Block 指针的 QList。
   */
  QList<Block*> GetFootageBlocks(QList<Node*> nodes);

  /**
   * @brief 将一个视图添加到此堆叠控件中的便捷函数。
   *
   * 主要用于构造函数。添加视图，连接其信号/槽，并设置模型。
   *
   * @param view
   *
   * 要添加到堆叠控件的视图指针 (QAbstractItemView*)。
   */
  void AddView(QAbstractItemView* view);

  /**
   * @brief 在模型中浏览到指定的文件夹索引。
   *
   * 仅影响 list_view_ 和 icon_view_。
   *
   * @param index
   *
   * 如果是无效索引，则返回到项目根目录；否则为有效的 Folder 对象的索引。
   */
  void BrowseToFolder(const QModelIndex& index);

  /**
   * @brief 确认是否删除指定的项目项（可能会显示确认对话框）。
   * @param item 要删除的 Node 指针。
   * @return 返回用户的选择结果（例如，QMessageBox::Yes, QMessageBox::No）。
   */
  int ConfirmItemDeletion(Node* item);

  /**
   * @brief 内部函数，实际执行删除项目项的操作。
   * @param selected 包含要删除的 Node 指针的 QVector。
   * @param check_if_item_is_in_use 布尔引用，指示是否需要检查项是否正在被使用。
   * @param command 用于记录删除操作的 MultiUndoCommand 指针（以便撤销）。
   * @return 如果删除操作成功（或部分成功），则返回 true。
   */
  bool DeleteItemsInternal(const QVector<Node*>& selected, bool& check_if_item_is_in_use, MultiUndoCommand* command);

  /**
   * @brief 获取节点的用户可读名称。
   * @param node Node 指针。
   * @return 返回节点的名称字符串。
   */
  static QString GetHumanReadableNodeName(Node* node);

  /**
   * @brief 更新导航栏显示的文本（通常是当前路径）。
   */
  void UpdateNavBarText();

  /**
   * @brief 获取当前活动的 QAbstractItemView（树状、列表或图标视图之一）。
   * @return 返回当前活动视图的指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QAbstractItemView* CurrentView() const;

  QStackedWidget* stacked_widget_;  ///< 用于在不同视图模式（树状、列表、图标）之间切换的堆叠控件。

  ProjectExplorerNavigation* nav_bar_;  ///< 项目浏览器顶部的导航栏，显示当前路径并允许导航。

  ProjectExplorerIconView* icon_view_;  ///< 图标视图模式。
  ProjectExplorerListView* list_view_;  ///< 列表视图模式。
  ProjectExplorerTreeView* tree_view_;  ///< 树状视图模式。

  ProjectToolbar::ViewType view_type_;  ///< 当前活动的视图类型。

  QSortFilterProxyModel sort_model_;  ///< 排序和过滤代理模型，应用于视图以支持排序和搜索。
  ProjectViewModel model_;            ///< 项目数据模型，为所有视图提供数据。

  QVector<Node*> context_menu_items_;  ///< 存储当前上下文菜单操作相关的项目项。

 private slots:
  /**
   * @brief 当视图的空白区域被双击时调用的槽函数。
   */
  void ViewEmptyAreaDoubleClickedSlot();

  /**
   * @brief 当视图中的某个项目项被双击时调用的槽函数。
   * @param index 被双击项的 QModelIndex。
   */
  void ItemDoubleClickedSlot(const QModelIndex& index);

  /**
   * @brief 当图标视图中的图标大小改变时调用的槽函数。
   * @param s 新的图标大小。
   */
  void SizeChangedSlot(int s);

  /**
   * @brief 当导航栏中的“向上”按钮被点击时调用的槽函数，用于返回上一级文件夹。
   */
  void DirUpSlot();

  /**
   * @brief 显示上下文菜单（通常在右键点击项目项或空白区域时）。
   */
  void ShowContextMenu();

  /**
   * @brief 显示选中项目项的属性对话框。
   */
  void ShowItemPropertiesDialog();

  /**
   * @brief 在系统文件浏览器中显示选中的素材文件。
   */
  void RevealSelectedFootage();

  /**
   * @brief 替换选中的素材文件。
   */
  void ReplaceSelectedFootage();

  /**
   * @brief 在新标签页中打开上下文菜单选中的项目项（如果适用）。
   */
  void OpenContextMenuItemInNewTab();

  /**
   * @brief 在新窗口中打开上下文菜单选中的项目项（如果适用）。
   */
  void OpenContextMenuItemInNewWindow();

  /**
   * @brief 处理上下文菜单中与“代理（Proxy）”相关的动作。
   * @param a 被触发的 QAction 指针。
   */
  void ContextMenuStartProxy(QAction* a);

  /**
   * @brief 当任何一个视图中的选择项发生改变时调用的槽函数。
   *
   * 此函数会同步所有视图的选择状态，并发出 ProjectExplorer 的 SelectionChanged 信号。
   */
  void ViewSelectionChanged();
};

}  // namespace olive

#endif  // PROJECTEXPLORER_H
