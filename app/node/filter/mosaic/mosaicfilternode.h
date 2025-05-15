#ifndef MOSAICFILTERNODE_H // 防止头文件被多次包含的宏定义开始
#define MOSAICFILTERNODE_H

#include "node/node.h" // 引入基类 Node 的定义

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 代表“马赛克”滤镜效果的节点。
 * 该节点可以将输入图像像素化，形成马赛克效果，可以通过控制水平和垂直方向的块大小来调整效果。
 */
class MosaicFilterNode : public Node {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief MosaicFilterNode 构造函数。
   */
  MosaicFilterNode();

  NODE_DEFAULT_FUNCTIONS(MosaicFilterNode) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "马赛克")，支持国际化翻译。
   */
  [[nodiscard]] QString Name() const override { return tr("Mosaic"); }

  /**
   * @brief 获取此节点的唯一标识符。
   * @return QString 类型的节点 ID (例如 "org.olivevideoeditor.Olive.mosaicfilter")。
   */
  [[nodiscard]] QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.mosaicfilter"); }

  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "滤镜" (kCategoryFilter) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override { return {kCategoryFilter}; }

  /**
   * @brief 获取此节点的描述信息。
   * @return QString 类型的节点描述 (例如 "对视频应用像素化的马赛克滤镜。")，支持国际化翻译。
   */
  [[nodiscard]] QString Description() const override { return tr("Apply a pixelated mosaic filter to video."); }

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述、参数名)。
   */
  void Retranslate() override;

  /**
   * @brief 计算并输出节点在特定时间点的值（应用马赛克处理后的图像数据）。
   * @param value 当前输入行数据 (包含输入图像及马赛克参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的马赛克化图像)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;
  /**
   * @brief 获取用于应用此马赛克效果的着色器代码。
   * @param request 包含着色器请求参数的对象 (例如马赛克块的大小)。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kTextureInput; ///< "Texture" - 输入纹理（图像）的参数键名。
  static const QString kHorizInput;   ///< "HorizontalBlocks" - 水平方向马赛克块数量（或块大小）的参数键名。
  static const QString kVertInput;    ///< "VerticalBlocks" - 垂直方向马赛克块数量（或块大小）的参数键名。
};

}  // namespace olive

#endif  // MOSAICFILTERNODE_H // 头文件宏定义结束