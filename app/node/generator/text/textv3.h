#ifndef TEXTGENERATORV3_H // 防止头文件被多次包含的宏定义开始
#define TEXTGENERATORV3_H

#include "node/generator/shape/shapenodebase.h" // 引入基类 ShapeNodeBase 的定义
#include "node/gizmo/text.h"                   // 引入文本 Gizmo (TextGizmo) 的定义

// 可能需要的前向声明 (如果 FramePtr 和 GenerateJob 在此未完全定义)
// namespace olive {
// class Frame; // 假设
// using FramePtr = std::shared_ptr<Frame>; // 假设
// struct GenerateJob; // 假设
// }
// Qt::Alignment 可能需要 <Qt> 或 <QFlags> 等头文件，但这里仅注释，不添加

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 代表“文本生成器”（版本3）的节点。
 * 这是文本生成功能的又一个版本，继承自 ShapeNodeBase，并引入了 TextGizmo。
 * "V3" 可能表示在文本编辑、布局、对齐以及与参数化字符串交互方面有更高级的功能。
 */
class TextGeneratorV3 : public ShapeNodeBase {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief TextGeneratorV3 构造函数。
   *  可能会初始化更复杂的文本参数和 TextGizmo。
   */
  TextGeneratorV3();

  NODE_DEFAULT_FUNCTIONS(TextGeneratorV3) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "文本" 或 "文本生成器 V3")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "生成器" (Generator) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此节点的描述信息。
   * @return QString 类型的节点描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述、参数名)。
   */
  void Retranslate() override;

  /**
   * @brief 计算并输出节点在特定时间点的值（生成的包含文本的图像）。
   * @param value 当前输入行数据。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  /**
   * @brief 生成包含渲染后文本的一帧图像数据。
   * @param frame 指向目标帧数据 (FramePtr) 的智能指针。
   * @param job 包含生成作业所需参数的 GenerateJob 对象。
   */
  void GenerateFrame(FramePtr frame, const GenerateJob &job) const override;

  /**
   * @brief 更新 Gizmo (特别是 TextGizmo) 在屏幕上的位置和状态。
   * @param row 当前节点的参数值。
   * @param globals 全局节点处理参数。
   */
  void UpdateGizmoPositions(const NodeValueRow &row, const NodeGlobals &globals) override;

  /**
   * @brief 定义文本的垂直对齐方式。
   */
  enum VerticalAlignment {
    kVAlignTop,    ///< 顶端对齐
    kVAlignMiddle, ///< 垂直居中对齐
    kVAlignBottom  ///< 底端对齐
  };

  /**
   * @brief 获取当前设置的垂直对齐方式。
   * @return VerticalAlignment 枚举值。
   */
  [[nodiscard]] VerticalAlignment GetVerticalAlignment() const {
    return static_cast<VerticalAlignment>(GetStandardValue(kVerticalAlignmentInput).toInt());
  }

  /**
   * @brief (静态工具函数) 将此节点内部的垂直对齐枚举值转换为 Qt 的对齐标志。
   * @param v TextGeneratorV3::VerticalAlignment 枚举值。
   * @return Qt::Alignment Qt 的对齐标志。
   */
  static Qt::Alignment GetQtAlignmentFromOurs(VerticalAlignment v);
  /**
   * @brief (静态工具函数) 将 Qt 的对齐标志转换为此节点内部的垂直对齐枚举值。
   * @param v Qt::Alignment Qt 的对齐标志。
   * @return VerticalAlignment TextGeneratorV3::VerticalAlignment 枚举值。
   */
  static VerticalAlignment GetOurAlignmentFromQts(Qt::Alignment v);

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kTextInput;              ///< "Text" - 文本内容的参数键名 (可能支持格式化占位符)。
  static const QString kVerticalAlignmentInput; ///< "VerticalAlignment" - 垂直对齐方式的参数键名。
  static const QString kUseArgsInput;           ///< "UseArgs" - 是否使用参数化字符串的布尔参数键名。
  static const QString kArgsInput;              ///< "Args" - 用于格式化字符串的参数列表的键名。

  /**
   * @brief (静态工具函数) 使用给定的参数列表格式化输入字符串。
   *  例如，将 "Hello %1, meet %2" 和参数 ["World", "Olive"] 格式化为 "Hello World, meet Olive"。
   * @param input 包含占位符（如 %1, %2）的原始字符串。
   * @param args 用于替换占位符的字符串列表。
   * @return QString 格式化后的字符串。
   */
  static QString FormatString(const QString &input, const QStringList &args);

 protected:
  /**
   * @brief 当节点的某个输入参数值发生变化时调用的事件处理函数。
   * @param input 值发生变化的输入端口/参数名称。
   * @param element 相关元素的索引。
   */
  void InputValueChangedEvent(const QString &input, int element) override;

 private:
  TextGizmo *text_gizmo_; ///< 指向文本交互 Gizmo (TextGizmo) 对象的指针。

  bool dont_emit_valign_; ///< 一个标志，可能用于在某些情况下阻止发射垂直对齐相关的信号，以避免循环更新。

 private slots:
  /**
   * @brief 当文本 Gizmo 被激活（例如，用户开始编辑文本）时调用的槽函数。
   */
  void GizmoActivated();
  /**
   * @brief 当文本 Gizmo 被取消激活（例如，用户完成编辑文本）时调用的槽函数。
   */
  void GizmoDeactivated();
  /**
   * @brief 通过 Gizmo 操作设置垂直对齐方式，并确保此操作可撤销。
   * @param a 新的 Qt::Alignment 对齐标志。
   */
  void SetVerticalAlignmentUndoable(Qt::Alignment a);
};

}  // namespace olive

#endif  // TEXTGENERATORV3_H // 头文件宏定义结束