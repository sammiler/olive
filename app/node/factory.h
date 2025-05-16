#ifndef NODEFACTORY_H  // 防止头文件被多次包含的宏定义开始
#define NODEFACTORY_H

#include <QList>  // Qt 列表容器

#include "node.h"              // 引入 Node 基类的定义，工厂用于创建 Node 对象
#include "widget/menu/menu.h"  // 引入 Menu 类的定义，工厂可能用于创建包含节点类型的菜单

// 可能需要的前向声明
// class QWidget; // 假设
// class QAction; // 假设

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 节点工厂类，负责创建项目中所有不同类型的节点实例。
 * 它使用一个内部ID (InternalID) 枚举来唯一标识每种可创建的节点类型。
 * 提供了根据ID或菜单操作创建节点的方法，并能构建包含可用节点类型的菜单。
 */
class NodeFactory {
 public:
  /**
   * @brief 定义了所有可由工厂创建的内部节点类型的唯一标识符。
   *  这些ID用于内部索引和创建特定类型的节点。
   */
  enum InternalID {
    kViewerOutput,                ///< 查看器输出节点 (ViewerOutput)
    kClipBlock,                   ///< 媒体片段块 (ClipBlock)
    kGapBlock,                    ///< 间隙块 (GapBlock)
    kPolygonGenerator,            ///< 多边形生成器 (PolygonGenerator)
    kMatrixGenerator,             ///< 矩阵生成器 (MatrixGenerator，通常是变换节点的基类)
    kTransformDistort,            ///< 变换扭曲节点 (TransformDistortNode)
    kTrackOutput,                 ///< 轨道输出节点 (Track，作为输出使用时)
    kAudioVolume,                 ///< 音频音量调整节点
    kAudioPanning,                ///< 音频声像调整节点
    kMath,                        ///< 通用数学运算节点 (MathNode)
    kTime,                        ///< 时间输入节点 (TimeInput)
    kTrigonometry,                ///< 三角函数运算节点 (TrigonometryNode)
    kBlurFilter,                  ///< 模糊滤镜节点 (BlurFilterNode)
    kSolidGenerator,              ///< 纯色生成器节点 (SolidGenerator)
    kMerge,                       ///< 合并/混合节点 (MergeNode)
    kStrokeFilter,                ///< 描边滤镜节点 (StrokeFilterNode)
    kTextGeneratorV1,             ///< 文本生成器 V1 (TextGeneratorV1)
    kTextGeneratorV2,             ///< 文本生成器 V2 (TextGeneratorV2)
    kTextGeneratorV3,             ///< 文本生成器 V3 (TextGeneratorV3)
    kCrossDissolveTransition,     ///< 交叉溶解转场 (CrossDissolveTransition)
    kDipToColorTransition,        ///< 浸入颜色转场 (DipToColorTransition)
    kMosaicFilter,                ///< 马赛克滤镜节点 (MosaicFilterNode)
    kCropDistort,                 ///< 裁剪扭曲节点 (CropDistortNode)
    kProjectFootage,              ///< 项目素材项 (Footage)
    kProjectFolder,               ///< 项目文件夹项 (Folder)
    kProjectSequence,             ///< 项目序列项 (Sequence)
    kValueNode,                   ///< 值/常量节点 (ValueNode)
    kTimeRemapNode,               ///< 时间重映射节点 (TimeRemapNode)
    kSubtitleBlock,               ///< 字幕块 (SubtitleBlock)
    kShapeGenerator,              ///< 基本形状生成器 (ShapeNode)
    kColorDifferenceKeyKeying,    ///< 色彩差异键控节点 (ColorDifferenceKeyNode)
    kDespillKeying,               ///< 溢色去除键控节点 (DespillNode)
    kGroupNode,                   ///< 节点组 (NodeGroup)
    kOpacityEffect,               ///< 不透明度效果节点 (OpacityEffect)
    kFlipDistort,                 ///< 翻转扭曲节点 (FlipDistortNode)
    kNoiseGenerator,              ///< 噪点生成器节点 (NoiseGeneratorNode)
    kTimeOffsetNode,              ///< 时间偏移节点 (TimeOffsetNode)
    kCornerPinDistort,            ///< 边角定位扭曲节点 (CornerPinDistortNode)
    kDisplayTransform,            ///< OCIO 显示变换节点 (DisplayTransformNode)
    kOCIOGradingTransformLinear,  ///< OCIO 线性调色变换节点 (OCIOGradingTransformLinearNode)
    kChromaKey,                   ///< 色度键控节点 (ChromaKeyNode)
    kMaskDistort,                 ///< 蒙版扭曲节点 (MaskDistortNode)
    kDropShadowFilter,            ///< 投影滤镜节点 (DropShadowFilter)
    kTimeFormat,                  ///< 时间格式化节点 (TimeFormatNode)
    kWaveDistort,                 ///< 波浪扭曲节点 (WaveDistortNode)
    kRippleDistort,               ///< 波纹扭曲节点 (RippleDistortNode)
    kTileDistort,                 ///< 平铺扭曲节点 (TileDistortNode)
    kSwirlDistort,                ///< 漩涡扭曲节点 (SwirlDistortNode)
    kMulticamNode,                ///< 多机位编辑节点 (MultiCamNode)

    // Count value
    kInternalNodeCount  ///< 内部节点类型的总数，用作计数或数组边界
  };

  /**
   * @brief NodeFactory 默认构造函数。
   *  由于所有方法都是静态的，通常不会创建此类的实例。
   */
  NodeFactory() = default;

  /**
   * @brief (静态方法) 初始化节点工厂。
   *  通常在应用程序启动时调用，用于创建所有已知节点类型的原型实例并存储到 `library_` 中。
   */
  static void Initialize();

  /**
   * @brief (静态方法) 销毁节点工厂资源。
   *  通常在应用程序关闭时调用，用于清理 `library_` 中存储的节点原型实例。
   */
  static void Destroy();

  /**
   * @brief (静态方法) 创建一个包含可用节点类型的菜单。
   * @param parent 菜单的父 QWidget。
   * @param create_none_item (可选) 是否在菜单顶部添加一个“无”或“空”选项，默认为 false。
   * @param restrict_to (可选) 限制菜单只显示属于特定分类 (Node::CategoryID) 的节点，默认为显示所有分类。
   * @param restrict_flags (可选) 进一步限制菜单项的标志位 (例如，只显示可用于某种轨道的节点)，默认为0 (无额外限制)。
   * @return Menu* 指向新创建的菜单对象的指针。调用者负责管理其生命周期。
   */
  static Menu* CreateMenu(QWidget* parent, bool create_none_item = false,
                          Node::CategoryID restrict_to = Node::kCategoryUnknown, uint64_t restrict_flags = 0);

  /**
   * @brief (静态方法) 根据菜单中被选中的 QAction 创建对应的节点实例。
   * @param action 用户在节点创建菜单中选择的 QAction。此 QAction 通常存储了关联节点的 ID。
   * @return Node* 指向新创建的节点实例的指针，如果无法创建则为 nullptr。
   */
  static Node* CreateFromMenuAction(QAction* action);

  /**
   * @brief (静态方法) 从菜单 QAction 中获取关联节点的唯一 ID 字符串。
   * @param action 菜单中的 QAction。
   * @return QString 节点的唯一 ID。
   */
  static QString GetIDFromMenuAction(QAction* action);

  /**
   * @brief (静态方法) 根据节点的唯一 ID 字符串获取其可读名称。
   * @param id 节点的唯一 ID。
   * @return QString 节点的名称。
   */
  static QString GetNameFromID(const QString& id);

  /**
   * @brief (静态方法) 根据节点的唯一 ID 字符串创建对应的节点实例。
   * @param id 要创建的节点的唯一 ID。
   * @return Node* 指向新创建的节点实例的指针，如果 ID 无效或无法创建则为 nullptr。
   */
  static Node* CreateFromID(const QString& id);

  /**
   * @brief (静态方法) 根据内部工厂索引 (InternalID) 创建对应的节点实例。
   * @param id 要创建的节点的内部工厂索引。
   * @return Node* 指向新创建的节点实例的指针，如果索引无效则为 nullptr。
   */
  static Node* CreateFromFactoryIndex(const InternalID& id);

 private:
  // 存储所有已知节点类型的原型实例的列表。
  // 当需要创建某个类型的节点时，会从这个库中找到对应的原型，然后克隆它。
  static QList<Node*> library_;  ///< 节点原型库。
};

}  // namespace olive

#endif  // NODEFACTORY_H // 头文件宏定义结束