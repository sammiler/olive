#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <QAbstractItemModel> // Qt 抽象项目模型基类
#include <QModelIndex>      // Qt 模型索引类
#include <QVariant>         // Qt 通用数据类型类
#include <QStringList>      // Qt 字符串列表类
#include <QMimeData>        // Qt MIME 数据类 (用于拖放)
#include <QObject>          // Qt 对象模型基类 (ProjectViewModel 的基类)

#include "node/block/block.h" // Block 类定义 (可能在数据角色中使用)
#include "node/project.h"     // Project 类定义 (包含 Project, Folder, Node)
#include "undo/undocommand.h" // 撤销命令基类 (虽然未直接使用，但模型修改通常与撤销栈关联)

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWidget; // ProjectViewModel 构造函数参数 parent 的基类
// class QString; // 已通过 QVariant, QStringList 间接包含

// 前向声明项目内部类 (根据用户要求，不添加)
// class Node; // 已在 node/project.h 中定义
// class Folder; // 已在 node/project.h 中定义

namespace olive {

/**
 * @brief ProjectViewModel 类是一个适配器，它将 Project 中的数据解释为 Qt 项目模型，供 ViewModel 视图使用。
 *
 * 假设一个 Project 当前是“打开的”（即 Project 通过 ProjectViewModel 连接到 ProjectExplorer/ProjectPanel），
 * 最好通过 ProjectViewModel 进行修改（例如添加/删除/重命名），以便视图能够高效且正确地更新。
 * ProjectViewModel 包含几个 Project 和 Item 函数的“包装器”函数，这些函数也会通知任何连接的视图相应地更新。
 */
class ProjectViewModel : public QAbstractItemModel {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief ColumnType 枚举定义了模型中支持的列类型。
   */
  enum ColumnType {
    kName,         ///< 媒体名称列。
    kDuration,     ///< 媒体持续时间列。
    kRate,         ///< 媒体速率（视频为帧率，音频为采样率）列。
    kLastModified, ///< （素材/文件的）最后修改时间列。
    kCreatedTime,  ///< （素材/文件的）创建时间列。
    kColumnCount   ///< 列的总数，用作枚举结束标记和计数。
  };

  /**
   * @brief 自定义数据角色，可能用于获取项的内部文本或其他特定数据。
   */
  static const int kInnerTextRole = Qt::UserRole + 1;

  /**
   * @brief ProjectViewModel 构造函数。
   *
   * @param parent
   * 父对象，用于内存管理。
   */
  explicit ProjectViewModel(QObject *parent);

  /**
   * @brief 获取当前活动的项目。
   *
   * @return
   *
   * 当前活动项目的指针，如果没有则为 nullptr。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Project *project() const;

  /**
   * @brief 设置要适配的项目。
   *
   * 任何附加到此模型的视图都将通过此函数得到更新。
   *
   * @param p
   *
   * 要适配的 Project 指针，可以设置为 nullptr 来“关闭”项目（将显示一个无法修改的空模型）。
   */
  void set_project(Project *p);

  /** Qt QAbstractItemModel 的强制重写方法 */
  /**
   * @brief 返回给定行、列和父索引处的子项的模型索引。
   * @param row 行号。
   * @param column 列号。
   * @param parent 父项的模型索引。对于顶层项，此参数为无效的 QModelIndex()。
   * @return 子项的 QModelIndex。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  /**
   * @brief 返回给定子项索引的父项的模型索引。
   * @param child 子项的 QModelIndex。
   * @return 父项的 QModelIndex。如果 child 是顶层项，则返回无效的 QModelIndex()。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QModelIndex parent(const QModelIndex &child) const override;
  /**
   * @brief 返回给定父项下的行数（子项数量）。
   * @param parent 父项的模型索引。对于顶层项，此参数为无效的 QModelIndex()。
   * @return 子项的数量。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  /**
   * @brief 返回模型中的列数。
   * @param parent 父项的模型索引（通常不影响列数）。
   * @return 列的数量。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  /**
   * @brief 返回给定模型索引和数据角色处的数据。
   * @param index 项的模型索引。
   * @param role 请求的数据角色 (例如 Qt::DisplayRole, Qt::EditRole, Qt::DecorationRole)。
   * @return 返回一个 QVariant，包含请求的数据。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  /** Qt QAbstractItemModel 的可选重写方法 */
  /**
   * @brief 返回指定节（列或行）的表头数据。
   * @param section 节的索引（列号或行号）。
   * @param orientation 表头的方向 (Qt::Horizontal 或 Qt::Vertical)。
   * @param role 请求的数据角色。
   * @return 返回一个 QVariant，包含表头数据。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                    int role = Qt::DisplayRole) const override;
  /**
   * @brief 检查给定的父项是否有子项。
   * @param parent 父项的模型索引。
   * @return 如果父项有子项，则返回 true；否则返回 false。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;
  /**
   * @brief 设置给定模型索引处的数据。
   *
   * 用于支持用户在视图中编辑数据。
   * @param index 要设置数据的项的模型索引。
   * @param value 新的数据值。
   * @param role 要设置数据的角色 (通常是 Qt::EditRole)。
   * @return 如果数据设置成功，则返回 true；否则返回 false。
   */
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
  /**
   * @brief 检查模型是否可以从给定的父项获取更多数据（用于惰性加载）。
   * @param parent 父项的模型索引。
   * @return 如果可以获取更多数据，则返回 true。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool canFetchMore(const QModelIndex &parent) const override;

  /** 拖放支持 */
  /**
   * @brief 返回给定模型索引处项的标志（例如，是否可选、可编辑、可拖放等）。
   * @param index 项的模型索引。
   * @return 返回 Qt::ItemFlags 的组合。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
  /**
   * @brief 返回此模型支持的 MIME 类型列表（用于拖放操作）。
   * @return 返回包含 MIME 类型字符串的 QStringList。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QStringList mimeTypes() const override;
  /**
   * @brief 为指定的索引列表创建 MIME 数据对象，用于拖拽操作。
   * @param indexes 包含要拖拽的项的模型索引的列表。
   * @return 返回一个 QMimeData 指针，包含编码后的项数据。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QMimeData *mimeData(const QModelIndexList &indexes) const override;
  /**
   * @brief 处理拖放操作，将 MIME 数据放到指定的行、列和父项下。
   * @param data 包含要放下的数据的 QMimeData 指针。
   * @param action 执行的拖放动作 (例如 Qt::MoveAction, Qt::CopyAction)。
   * @param row 目标行号。
   * @param column 目标列号。
   * @param drop 父项的模型索引，项将被放到此父项下。
   * @return 如果成功处理了放下操作，则返回 true；否则返回 false。
   */
  bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                    const QModelIndex &drop) override;

  /**
   * @brief 从 Item 对象创建 QModelIndex 的便捷函数。
   * @param item 指向 Node (项目中的项) 对象的指针。
   * @param column 目标列号，默认为 0。
   * @return 返回对应的 QModelIndex。
   */
  QModelIndex CreateIndexFromItem(Node *item, int column = 0);

 private:
  /**
   * @brief 获取 `item` 在其父项中的索引。
   *
   * 此函数将根据当前活动的任何排序算法返回指定项在其父项中的索引。
   *
   * @return
   *
   * 指定项的索引；如果项是根项（此时它没有父项），则返回 -1。
   */
  static int IndexOfChild(Node *item);

  /**
   * @brief 从给定的索引检索 Item 对象。
   *
   * 用于检索 Item 对象的便捷函数。如果索引无效，则此函数返回根 Item (通常是项目的根文件夹)。
   * @param index 要从中获取对象的 QModelIndex。
   * @return 返回对应的 Node 指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Node *GetItemObjectFromIndex(const QModelIndex &index) const;

  /**
   * @brief 检查一个 Item 是否是另一个 Child Item 的父项。
   *
   * 检查 `child` 的整个“父项层级结构”，看 `parent` 是否是其父项之一。
   * @param parent 可能是父项的 Folder 指针。
   * @param child 要检查其父项层级的 Node 指针。
   * @return 如果 parent 是 child 的父项（或祖先），则返回 true。
   */
  static bool ItemIsParentOfChild(Folder *parent, Node *child);

  /**
   * @brief 连接指定节点发出的信号到此模型的槽函数，以保持同步。
   * @param n 要连接其信号的 Node 指针。
   */
  void ConnectItem(Node *n);

  /**
   * @brief 断开指定节点信号与此模型槽函数的连接。
   * @param n 要断开其信号的 Node 指针。
   */
  void DisconnectItem(Node *n);

  Project *project_; ///< 指向当前适配的项目 (Project) 对象的指针。

 private slots:
  /**
   * @brief 当文件夹即将插入新项时调用的槽函数。
   *
   * 在模型中发出 beginInsertRows() 信号。
   * @param n 被插入的父文件夹 Node 指针 (通常是 Folder*)。
   * @param insert_index 新项将被插入的行索引。
   */
  void FolderBeginInsertItem(Node *n, int insert_index);

  /**
   * @brief 当文件夹完成插入新项后调用的槽函数。
   *
   * 在模型中发出 endInsertRows() 信号。
   */
  void FolderEndInsertItem();

  /**
   * @brief 当文件夹即将移除项时调用的槽函数。
   *
   * 在模型中发出 beginRemoveRows() 信号。
   * @param n 被移除项的父文件夹 Node 指针 (通常是 Folder*)。
   * @param child_index 被移除项的行索引。
   */
  void FolderBeginRemoveItem(Node *n, int child_index);

  /**
   * @brief 当文件夹完成移除项后调用的槽函数。
   *
   * 在模型中发出 endRemoveRows() 信号。
   */
  void FolderEndRemoveItem();

  /**
   * @brief 当项目中的某个项被重命名时调用的槽函数。
   *
   * 触发模型数据改变的信号 (dataChanged)。
   */
  void ItemRenamed();
};

}  // namespace olive

#endif  // VIEWMODEL_H
