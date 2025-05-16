#ifndef TEXTGENERATORV1_H  // 防止头文件被多次包含的宏定义开始
#define TEXTGENERATORV1_H

#include "node/node.h"  // 引入基类 Node 的定义

// 可能需要的前向声明 (如果 FramePtr 和 GenerateJob 在此未完全定义)
// namespace olive {
// class Frame; // 假设
// using FramePtr = std::shared_ptr<Frame>; // 假设
// struct GenerateJob; // 假设
// }

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 代表“文本生成器”（版本1）的节点。
 * 该节点用于在视频中创建和渲染文本内容。用户可以指定文本内容、字体、大小、颜色、对齐方式等。
 * "V1" 后缀可能表示这是该功能的第一个或某个特定版本的实现。
 */
class TextGeneratorV1 : public Node {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief TextGeneratorV1 构造函数。
      *  通常会在这里初始化文本相关的输入参数。
      */
     TextGeneratorV1();

  NODE_DEFAULT_FUNCTIONS(TextGeneratorV1)  // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "文本" 或 "文本生成器 V1")。
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
   *  此方法可能会调用 GenerateFrame 来实际渲染文本。
   * @param value 当前输入行数据 (包含文本内容、字体、颜色等参数)。
   * @param globals 全局节点处理参数 (例如输出分辨率)。
   * @param table 用于存储输出值的表 (包含输出的文本图像)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  /**
   * @brief 生成包含渲染后文本的一帧图像数据。
   *  这是实际将文本字符转换为像素数据的地方。
   * @param frame 指向目标帧数据 (FramePtr) 的智能指针，文本将被渲染到此帧上。
   * @param job 包含生成作业所需参数的 GenerateJob 对象 (例如文本内容、字体、颜色、渲染区域等)。
   */
  void GenerateFrame(FramePtr frame, const GenerateJob &job) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kTextInput;   ///< "Text" - 要显示的纯文本内容的参数键名。
  static const QString kHtmlInput;   ///< "RichText" - 是否将输入文本作为富文本 (HTML) 解析的布尔参数键名。
  static const QString kColorInput;  ///< "Color" - 文本颜色的参数键名。
  static const QString
      kVAlignInput;  ///< "VerticalAlignment" - 文本垂直对齐方式（如顶对齐、居中对齐、底对齐）的参数键名。
  static const QString kFontInput;      ///< "Font" - 选择字体的参数键名 (例如字体家族名称)。
  static const QString kFontSizeInput;  ///< "FontSize" - 字体大小的参数键名。
};

}  // namespace olive

#endif  // TEXTGENERATORV1_H // 头文件宏定义结束