#ifndef PROJECTIMPORTMANAGER_H  // 防止头文件被重复包含的预处理器指令
#define PROJECTIMPORTMANAGER_H  // 定义 PROJECTIMPORTMANAGER_H 宏

#include <QFileInfoList>  // 包含了 Qt 文件信息列表类，用于处理文件和目录信息
#include <QUndoCommand>   // 包含了 Qt 撤销命令基类，用于实现撤销/重做功能

#include "codec/decoder.h"                            // 包含了编解码器相关的定义，可能用于获取导入文件的信息
#include "task/task.h"                                // 包含了任务基类的定义，ProjectImportTask 是一个后台任务
#include "widget/projectexplorer/projectviewmodel.h"  // 包含了项目浏览器视图模型相关的定义，可能与项目结构和节点操作有关

// 在 olive 命名空间内声明的类，其完整定义依赖于包含的头文件
// 例如 Folder, Node, Footage, MultiUndoCommand 等类型预期从上述 #include 中获得

namespace olive {  // olive 项目的命名空间

/**
 * @brief ProjectImportTask 类定义，继承自 Task 类。
 *
 * 此类负责处理将外部文件异步导入到项目中的任务。它可以处理单个或多个文件，
 * 能够识别并处理图像序列，对导入文件进行有效性验证，
 * 并将成功导入的素材（Footage）添加到项目指定的文件夹（Folder）中。
 * 此外，该任务还会生成一个 MultiUndoCommand，以便用户可以撤销整个导入操作。
 */
class ProjectImportTask : public Task {
 Q_OBJECT  // Qt 对象的宏，用于启用信号和槽机制等 Qt 特性
     public :
     /**
      * @brief ProjectImportTask 的构造函数。
      * @param folder 指向目标文件夹 (Folder 类型) 的指针，新导入的素材将被添加到这个文件夹下。
      * @param filenames 一个字符串列表，包含所有待导入文件的完整路径。
      */
     ProjectImportTask(Folder* folder, const QStringList& filenames);

  /**
   * @brief 获取计划导入的文件总数。
   *
   * 这个数量是在任务开始时根据传入的文件名列表计算得出的。
   * @return const int& 对文件总数的常量引用。
   */
  [[nodiscard]] const int& GetFileCount() const;

  /**
   * @brief 获取与此导入任务关联的撤销命令对象。
   *
   * 这个命令对象封装了所有因导入操作而产生的项目结构变更，允许用户撤销导入。
   * @return MultiUndoCommand* 指向 MultiUndoCommand 对象的指针。如果任务未开始或未成功生成命令，可能返回 nullptr。
   */
  [[nodiscard]] MultiUndoCommand* GetCommand() const { return command_; }

  /**
   * @brief 获取在导入过程中被识别为无效或无法处理的文件列表。
   * @return const QStringList& 对包含无效文件路径的字符串列表的常量引用。
   */
  [[nodiscard]] const QStringList& GetInvalidFiles() const { return invalid_files_; }

  /**
   * @brief 检查在导入过程中是否遇到了任何无效文件。
   * @return 如果 `invalid_files_` 列表不为空，则返回 true，表示存在无效文件；否则返回 false。
   */
  [[nodiscard]] bool HasInvalidFiles() const { return !invalid_files_.isEmpty(); }

  /**
   * @brief 获取在此任务中成功导入的所有素材（Footage）对象的列表。
   * @return const QVector<Footage*>& 对包含已导入 Footage 对象指针的 QVector 的常量引用。
   */
  [[nodiscard]] const QVector<Footage*>& GetImportedFootage() const { return imported_footage_; }

 protected:
  /**
   * @brief 执行项目导入任务的核心逻辑。
   *
   * 此方法重写自基类 Task 的 Run 方法。当任务被调度执行时，此函数会被调用。
   * 它会初始化撤销命令，遍历 `filenames_` 列表中的每个文件，
   * 调用私有的 Import 方法来处理实际的导入过程。
   * @return 任务执行完毕后返回 true（注意：即使部分文件导入失败，只要任务流程本身没有崩溃，也可能返回 true）。
   * 如果发生无法恢复的严重错误，则可能返回 false。
   */
  bool Run() override;

 private:
  /**
   * @brief 实际执行文件导入到指定文件夹的内部方法。
   *
   * 此方法处理单个或多个文件（特别是图像序列的识别和组合）。
   * 对于每个成功导入的项，它会创建一个对应的 Node (通常是 Footage)，
   * 并将其添加到目标 `folder` 中，同时记录相应的撤销操作到 `parent_command`。
   * @param folder 指向目标文件夹的指针，导入的项将添加到此文件夹。
   * @param import 一个 QFileInfoList 对象，包含待处理的文件信息。
   * @param counter 一个整数引用，用于在外部跟踪已处理的文件数量。
   * @param parent_command 指向父 MultiUndoCommand 的指针，所有与此次导入相关的原子撤销操作都将作为子命令添加到其中。
   */
  void Import(Folder* folder, QFileInfoList import, int& counter, MultiUndoCommand* parent_command);

  /**
   * @brief 验证并处理图像序列。
   *
   * 当检测到可能的图像序列时，此方法被调用以确认序列的连续性和有效性，
   * 并相应地调整 `footage` 对象的属性（如帧范围、持续时间等）。
   * @param footage 指向一个已创建的 Footage 对象的指针，该对象可能代表一个图像序列的起始帧。
   * @param info_list 包含潜在图像序列中所有文件信息的列表。
   * @param index 当前处理的文件在 `info_list` 中的索引。
   */
  void ValidateImageSequence(Footage* footage, QFileInfoList& info_list, int index);

  /**
   * @brief 将一个项目项（Node）添加到指定的文件夹，并记录撤销命令。
   * @param folder 指向目标文件夹的指针。
   * @param item 指向要添加的 Node 对象的指针。
   * @param command 指向 MultiUndoCommand 的指针，用于记录添加操作的撤销步骤。
   */
  void AddItemToFolder(Folder* folder, Node* item, MultiUndoCommand* command);

  /**
   * @brief 静态辅助函数，判断一个 Footage 对象是否仅代表单个静态图像。
   * @param footage 指向要检查的 Footage 对象的指针。
   * @return 如果该 Footage 对象代表的是单个静态图像（而非视频或图像序列），则返回 true；否则返回 false。
   */
  static bool ItemIsStillImageFootageOnly(Footage* footage);

  /**
   * @brief 静态辅助函数，比较一个 Footage 对象的尺寸是否与给定的 QSize 相同。
   *
   * 主要用于图像序列的验证，确保序列中的所有图像具有一致的尺寸。
   * @param footage 指向要检查的 Footage 对象的指针。
   * @param sz 要比较的目标 QSize 对象。
   * @return 如果 Footage 的尺寸与 `sz` 相同，则返回 true；否则返回 false。
   */
  static bool CompareStillImageSize(Footage* footage, const QSize& sz);

  /**
   * @brief 静态辅助函数，获取图像序列的帧数限制。
   *
   * 根据起始文件名、起始编号以及搜索方向（向上或向下），确定图像序列的有效帧数。
   * @param start_fn 图像序列的起始文件名。
   * @param start 起始编号。
   * @param up 如果为 true，则向上搜索序列；否则向下搜索。
   * @return 返回图像序列的帧数限制（int64_t）。
   */
  static int64_t GetImageSequenceLimit(const QString& start_fn, int64_t start, bool up);

  MultiUndoCommand* command_;  ///< @brief 指向 MultiUndoCommand 对象的指针，用于聚合所有导入操作的撤销命令。

  Folder* folder_;  ///< @brief 指向目标文件夹的指针，所有成功导入的素材都将添加到此文件夹中。

  QFileInfoList filenames_;  ///< @brief 存储了所有待导入文件的 QFileInfo 对象列表。

  int file_count_;  ///< @brief 记录了初始请求导入的文件总数。

  QStringList invalid_files_;  ///< @brief 存储导入失败或被识别为无效的文件路径列表。

  QList<QString> image_sequence_ignore_files_;  ///< @brief 在图像序列检测过程中，需要忽略的文件名列表。
                                                ///< 这可以防止将不相关的、但名称相似的文件错误地识别为序列的一部分。

  QVector<Footage*> imported_footage_;  ///< @brief 存储成功导入并创建的 Footage 对象指针的向量。
};

}  // namespace olive

#endif  // PROJECTIMPORTMANAGER_H // 结束预处理器指令 #ifndef PROJECTIMPORTMANAGER_H
