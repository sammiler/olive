#ifndef MAINWINDOWLAYOUTINFO_H
#define MAINWINDOWLAYOUTINFO_H

#include <QByteArray>        // 引入 QByteArray，用于存储原始的停靠布局状态
#include <QHash>             // 引入 QHash，用于 fromXml 中的节点指针映射
#include <QMetaType>         // 引入 QMetaType，用于 Q_DECLARE_METATYPE
#include <QXmlStreamReader>  // 引入 QXmlStreamReader，用于从 XML 反序列化布局信息
#include <QXmlStreamWriter>  // 引入 QXmlStreamWriter，用于将布局信息序列化为 XML
#include <map>               // 引入 std::map，用于存储面板数据
#include <vector>            // 引入 std::vector，用于存储打开的文件夹、序列和查看器列表

#include "node/project/folder/folder.h"      // 引入 Folder 类的定义
#include "node/project/sequence/sequence.h"  // 引入 Sequence 类的定义
#include "panel/panel.h"  // 引入 PanelWidget::Info 的定义 (假设 PanelWidget 在 panel.h 中定义或间接包含)

// 前向声明 Node 和 ViewerOutput，如果它们的完整定义已在上述头文件中，则可能非必需
// namespace olive { namespace core { class Node; } }
// namespace olive { class ViewerOutput; } // ViewerOutput 可能继承自 Node 或是一个独立类型

namespace olive {

// 如果 PanelWidget::Info 未在 panel.h 中完全定义，可能需要前向声明
// namespace olive { class PanelWidget { public: struct Info; }; }

/**
 * @brief 存储和管理主窗口布局信息的类。
 *
 * MainWindowLayoutInfo 用于序列化和反序列化主窗口的整体布局状态，
 * 包括KDDockWidgets的停靠状态、当前打开的文件夹、序列、查看器列表，
 * 以及各个面板的特定配置数据。
 */
class MainWindowLayoutInfo {
 public:
  /**
   * @brief 默认构造函数。
   */
  MainWindowLayoutInfo() = default;

  /**
   * @brief 将布局信息序列化到 XML 流。
   * @param writer 指向 QXmlStreamWriter 对象的指针，用于写入 XML 数据。
   */
  void toXml(QXmlStreamWriter* writer) const;

  /**
   * @brief 从 XML 流中反序列化布局信息。
   * @param reader 指向 QXmlStreamReader 对象的指针，用于读取 XML 数据。
   * @param node_ptrs 一个哈希表，用于将 XML 中存储的节点ID（quintptr）映射回实际的 Node* 指针。
   * 这是因为在反序列化时，需要恢复对项目中实际节点对象的引用。
   * @return 返回从 XML 创建的 MainWindowLayoutInfo 对象。
   */
  static MainWindowLayoutInfo fromXml(QXmlStreamReader* reader, const QHash<quintptr, Node*>& node_ptrs);

  /**
   * @brief 添加一个打开的文件夹到布局信息中。
   * @param f 指向要添加的 Folder 对象的指针。
   */
  void add_folder(Folder* f);

  /**
   * @brief 添加一个打开的序列到布局信息中。
   * @param seq 指向要添加的 Sequence 对象的指针。
   */
  void add_sequence(Sequence* seq);

  /**
   * @brief 添加一个打开的查看器（通常是与特定节点关联的）到布局信息中。
   * @param viewer 指向要添加的 ViewerOutput 对象的指针。
   */
  void add_viewer(ViewerOutput* viewer);

  /**
   * @brief 设置特定面板的配置数据。
   * @param id 面板的唯一标识符字符串。
   * @param data 包含该面板配置信息的 PanelWidget::Info 对象。
   */
  void set_panel_data(const QString& id, const PanelWidget::Info& data);

  /**
   * @brief 移动（重命名）一个面板的配置数据。
   *
   * 当面板的ID发生变化时（例如，由于面板类型或上下文的改变），
   * 此函数用于更新存储的面板数据以匹配新的ID。
   * @param old 旧的面板ID。
   * @param now 新的面板ID。
   */
  void move_panel_data(const QString& old_id, const QString& new_id);

  /**
   * @brief 设置主窗口停靠布局的原始状态。
   *
   * 这通常是 KDDockWidgets::MainWindow::saveState() 返回的 QByteArray。
   * @param layout 包含布局状态的 QByteArray。
   */
  void set_state(const QByteArray& layout);

  /**
   * @brief 获取当前布局中打开的文件夹列表。
   * @return 返回一个包含 Folder* 指针的 std::vector 的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const std::vector<Folder*>& open_folders() const { return open_folders_; }

  /**
   * @brief 获取当前布局中打开的序列列表。
   * @return 返回一个包含 Sequence* 指针的 std::vector 的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const std::vector<Sequence*>& open_sequences() const { return open_sequences_; }

  /**
   * @brief 获取当前布局中打开的查看器列表。
   * @return 返回一个包含 ViewerOutput* 指针的 std::vector 的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const std::vector<ViewerOutput*>& open_viewers() const { return open_viewers_; }

  /**
   * @brief 获取所有面板的配置数据。
   * @return 返回一个 std::map 的常量引用，其中键是面板ID (QString)，值是 PanelWidget::Info。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const std::map<QString, PanelWidget::Info>& panel_data() const { return panel_data_; }

  /**
   * @brief 获取主窗口停靠布局的原始状态。
   * @return 返回包含布局状态的 QByteArray 的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const QByteArray& state() const { return state_; }

 private:
  QByteArray state_;  ///< 存储 KDDockWidgets 主窗口的原始布局状态。

  std::vector<Folder*> open_folders_;        ///< 存储当前打开的文件夹列表。
  std::vector<Sequence*> open_sequences_;    ///< 存储当前打开的序列列表。
  std::vector<ViewerOutput*> open_viewers_;  ///< 存储当前打开的查看器（通常是与节点关联的）列表。

  std::map<QString, PanelWidget::Info> panel_data_;  ///< 存储各个面板的特定配置数据，键为面板ID。

  /**
   * @brief 布局信息格式的版本号。
   *
   * 用于在将来进行数据格式迁移或兼容性检查。
   */
  static const unsigned int kVersion = 1;
};

}  // namespace olive

// 声明 MainWindowLayoutInfo 类型为 Qt元对象系统可识别的类型。
// 这允许该类型的对象可以存储在 QVariant 中，或用于信号/槽机制（如果需要）。
Q_DECLARE_METATYPE(olive::MainWindowLayoutInfo)

#endif  // MAINWINDOWLAYOUTINFO_H
