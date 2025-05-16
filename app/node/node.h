#ifndef NODE_H  // 防止头文件被重复包含的宏
#define NODE_H  // 定义 NODE_H 宏

#include <QMutex>            // Qt 互斥锁类
#include <QObject>           // Qt 对象模型基类
#include <QPainter>          // Qt 绘图类
#include <QPointF>           // Qt 二维浮点坐标点类
#include <QXmlStreamWriter>  // Qt XML流写入类
#include <map>               // 标准库 map 容器
#include <utility>           // 标准库 utility 头文件，提供 pair 等

#include "codec/frame.h"                // 视频帧数据结构
#include "common/xmlutils.h"            // XML 工具类
#include "node/gizmo/draggable.h"       // 可拖拽 Gizmo 相关
#include "node/globals.h"               // 节点相关的全局定义
#include "node/inputimmediate.h"        // 节点立即输入相关
#include "node/keyframe.h"              // 关键帧数据结构
#include "node/param.h"                 // 参数相关
#include "render/audioplaybackcache.h"  // 音频播放缓存
#include "render/audiowaveformcache.h"  // 音频波形缓存
#include "render/framehashcache.h"      // 帧哈希缓存
#include "render/job/generatejob.h"     // 生成任务
#include "render/job/samplejob.h"       // 采样任务
#include "render/job/shaderjob.h"       //着色器任务
#include "render/shadercode.h"          //着色器代码封装
#include "splitvalue.h"                 // 分离值类型，用于处理多通道数据

namespace olive {  // olive 项目的命名空间

// 宏：为节点类 x 定义默认的析构函数和拷贝函数
#define NODE_DEFAULT_FUNCTIONS(x) \
  NODE_DEFAULT_DESTRUCTOR(x)      \
  NODE_COPY_FUNCTION(x)

// 宏：为节点类 x 定义默认的虚析构函数，在析构时断开所有连接
#define NODE_DEFAULT_DESTRUCTOR(x) \
  virtual ~x() override { DisconnectAll(); }

// 宏：为节点类 x 定义默认的拷贝函数，返回一个新的 x 类型实例
#define NODE_COPY_FUNCTION(x) \
  virtual Node* copy() const override { return new x(); }

class Folder;           // 向前声明 Folder 类
class Project;          // 向前声明 Project 类
struct SerializedData;  // 向前声明 SerializedData 结构体

/**
 * @brief Node 类是处理单元的基础，可以与其他节点连接以创建复杂的处理系统。
 *
 * Node 是“可视化编程”的基石，它代表一个单一的“函数”，接收输入并返回可以连接到其他节点的输出。
 * 输入可以是用户设置的，也可以从另一个节点的输出中检索。通过将多个节点连接在一起，
 * 可以创建复杂、高度可定制且可无限扩展的数据处理系统。这一切都可以暴露给用户，
 * 而无需强迫他们编写代码或编译任何东西。
 *
 * Olive 中的一个主要示例是整个渲染工作流程。要渲染一帧，Olive 将通过一个节点图进行工作，
 * 用户可以无限自定义该节点图以创建图像。
 *
 * 这是一个简单的基类，旨在包含此类处理连接单元的所有功能。
 * 它是一个抽象类，旨在被子类化以创建具有实际功能的节点。
 */
class Node : public QObject {  // Node 类继承自 QObject 以使用 Qt 的元对象系统（例如信号和槽）
 Q_OBJECT                      // 声明此类使用 Qt 的元对象系统
     public :
     // 节点分类ID枚举
     enum CategoryID {
       kCategoryUnknown = -1,  // 未知分类

       kCategoryOutput,      // 输出类节点
       kCategoryGenerator,   // 生成器类节点 (例如：纯色、噪点)
       kCategoryMath,        // 数学运算类节点
       kCategoryKeying,      // 抠像类节点 (例如：绿幕抠像)
       kCategoryFilter,      // 滤镜类节点 (例如：模糊、锐化)
       kCategoryColor,       // 颜色处理类节点 (例如：色彩校正、色相饱和度)
       kCategoryTime,        // 时间处理类节点 (例如：时间重映射)
       kCategoryTimeline,    // 时间轴相关节点 (例如：序列、片段)
       kCategoryTransition,  // 转场效果类节点
       kCategoryDistort,     // 扭曲变形类节点
       kCategoryProject,     // 项目相关节点

       kCategoryCount  // 分类总数，用于迭代或数组大小
     };

  // 节点标志位枚举，用于定义节点的特定行为或属性
  enum Flag {
    kNone = 0,                    // 无标志
    kDontShowInParamView = 0x1,   // 不在参数视图中显示
    kVideoEffect = 0x2,           // 标记为视频效果
    kAudioEffect = 0x4,           // 标记为音频效果
    kDontShowInCreateMenu = 0x8,  // 不在创建菜单中显示
    kIsItem = 0x10                // 标记为时间轴上的一个项目/片段 (Item)
  };

  // 上下文节点对结构体，通常用于表示一个节点和它所在的上下文环境中的另一个节点
  struct ContextPair {
    Node* node;     // 节点指针
    Node* context;  // 上下文节点指针
  };

  // 构造函数
  Node();

  // 虚析构函数，确保派生类的析构函数被正确调用
  ~Node() override;

  /**
   * @brief 创建当前节点的一个克隆副本。
   *
   * 默认情况下，克隆副本不会拥有原始节点的值和连接。
   * 调用者负责使用像 CopyInputs() 这样的函数来复制这些数据，因为拷贝操作可能出于不同的原因。
   * @return 返回新创建的节点副本的指针。这是一个纯虚函数，必须由子类实现。
   */
  [[nodiscard]] virtual Node* copy() const = 0;

  /**
   * @brief 便捷函数 - 假设父对象是一个 NodeGraph (通常是 Project)。
   * @return 返回父项目 Project 的指针。
   */
  [[nodiscard]] Project* parent() const;

  /**
   * @brief 获取此节点所属的项目。
   * @return 返回项目 Project 的指针。
   */
  [[nodiscard]] Project* project() const;

  /**
   * @brief 获取节点的标志位。
   * @return 返回节点的标志位 (uint64_t 类型)。
   */
  [[nodiscard]] const uint64_t& GetFlags() const { return flags_; }

  /**
   * @brief 返回节点的名称。
   *
   * 这是显示给用户的节点名称。子类必须重写此方法，并且最好通过翻译器进行处理。
   * @return 返回节点的名称 (QString 类型)。纯虚函数。
   */
  [[nodiscard]] virtual QString Name() const = 0;

  /**
   * @brief 如果适用，返回此节点的缩写名称。
   *
   * 默认返回 Name() 的结果，但可以被子类重写。
   * @return 返回节点的缩写名称 (QString 类型)。
   */
  [[nodiscard]] virtual QString ShortName() const;

  /**
   * @brief 返回节点的唯一标识符。
   *
   * 此标识符用于保存文件以及在运行时需要区分特定节点的任何其他情况。
   * 这必须是此节点完全唯一的 ID，最好采用捆绑标识符格式（例如 "org.company.Name"）。
   * 此字符串不应被翻译。纯虚函数。
   * @return 返回节点的唯一ID (QString 类型)。
   */
  [[nodiscard]] virtual QString id() const = 0;

  /**
   * @brief 返回此节点所属的分类（子类可选实现，但推荐）。
   *
   * 在任何有组织的节点菜单中，将节点显示在此分类下。
   * 如果此节点应位于子文件夹的子文件夹中，请使用 "/" 分隔分类（例如 "Distort/Noise"）。
   * 字符串不应以 "/" 开头，因为这将被解释为空字符串分类。
   * 此值应通过翻译器运行，因为它主要是面向用户的。纯虚函数。
   * @return 返回一个包含分类ID的 QVector。
   */
  [[nodiscard]] virtual QVector<CategoryID> Category() const = 0;

  /**
   * @brief 返回此节点用途的描述（子类可选实现，但推荐）。
   *
   * 一个简短（1-2句话）的描述，说明此节点的功能，以帮助用户理解其用途。
   * 此描述应通过翻译器运行。
   * @return 返回节点的描述 (QString 类型)。
   */
  [[nodiscard]] virtual QString Description() const;

  /**
   * @brief 获取节点所在的文件夹。
   * @return 返回 Folder 指针，如果节点不在任何文件夹中，则可能为 nullptr。
   */
  [[nodiscard]] Folder* folder() const { return folder_; }

  /**
   * @brief 检查此节点是否是一个项目 (Item，例如时间轴上的片段)。
   * @return 如果节点是项目，则返回 true，否则返回 false。
   */
  [[nodiscard]] bool IsItem() const { return flags_ & kIsItem; }

  /**
   * @brief 用于重新翻译参数名称的函数（应在派生类中重写）。
   */
  virtual void Retranslate();

  // 节点特定数据的类型枚举 (例如用于 data() 方法)
  enum DataType { ICON, DURATION, CREATED_TIME, MODIFIED_TIME, FREQUENCY_RATE, TOOLTIP };

  /**
   * @brief 获取节点的特定数据。
   * @param d 要获取的数据类型。
   * @return 返回一个 QVariant，包含所请求的数据。
   */
  [[nodiscard]] virtual QVariant data(const DataType& d) const;

  /**
   * @brief 获取节点所有输入端口的ID列表。
   * @return 返回一个包含输入ID字符串的 QVector。
   */
  [[nodiscard]] const QVector<QString>& inputs() const { return input_ids_; }

  /**
   * @brief 返回在渲染时应忽略的输入端口ID列表。
   *
   * 某些输入可能仅用于UI或特定逻辑，而不直接参与渲染计算。
   * @return 返回一个包含应忽略的输入ID字符串的 QVector。
   */
  [[nodiscard]] virtual QVector<QString> IgnoreInputsForRendering() const { return {}; }

  // 用于描述在特定时间范围内哪些元素是活动的类
  class ActiveElements {
   public:
    // 活动元素的模式枚举
    enum Mode { kAllElements, kSpecified, kNoElements };  // 所有元素、指定元素、无元素

    // 构造函数
    explicit ActiveElements(Mode m = kAllElements) { mode_ = m; }

    // 获取活动模式
    [[nodiscard]] Mode mode() const { return mode_; }
    // 获取活动的元素列表 (如果模式是 kSpecified)
    [[nodiscard]] std::list<int> elements() const { return elements_; }

    // 添加一个指定的活动元素
    void add(int e) {
      elements_.push_back(e);  // 将元素添加到列表
      mode_ = kSpecified;      // 将模式设置为指定元素
    }

   private:
    Mode mode_;                // 当前的活动模式
    std::list<int> elements_;  // 活动的元素索引列表
  };

  /**
   * @brief 获取在指定输入和时间范围内活动的元素。
   * @param input 输入端口的ID。
   * @param r 时间范围。
   * @return 返回 ActiveElements 对象。
   */
  [[nodiscard]] virtual ActiveElements GetActiveElementsAtTime(const QString& input, const TimeRange& r) const {
    return ActiveElements(ActiveElements::kAllElements);  // 默认返回所有元素都活动
  }

  /**
   * @brief 检查节点是否拥有指定ID的输入端口。
   * @param id 输入端口的ID。
   * @return 如果存在则返回 true，否则返回 false。
   */
  [[nodiscard]] bool HasInputWithID(const QString& id) const { return input_ids_.contains(id); }

  /**
   * @brief 检查节点是否拥有指定ID的参数 (通常输入端口也作为参数)。
   * @param id 参数的ID (通常是输入端口的ID)。
   * @return 如果存在则返回 true，否则返回 false。
   */
  [[nodiscard]] bool HasParamWithID(const QString& id) const { return HasInputWithID(id); }

  // 视频帧缓存访问器
  [[nodiscard]] FrameHashCache* video_frame_cache() const { return video_cache_; }
  // 缩略图缓存访问器
  [[nodiscard]] ThumbnailCache* thumbnail_cache() const { return thumbnail_cache_; }
  // 音频播放缓存访问器
  [[nodiscard]] AudioPlaybackCache* audio_playback_cache() const { return audio_cache_; }
  // 音频波形缓存访问器
  [[nodiscard]] AudioWaveformCache* waveform_cache() const { return waveform_cache_; }

  // 获取视频缓存的时间范围
  [[nodiscard]] virtual TimeRange GetVideoCacheRange() const { return {}; }
  // 获取音频缓存的时间范围
  [[nodiscard]] virtual TimeRange GetAudioCacheRange() const { return {}; }

  // 节点在上下文中的位置和展开状态结构体
  struct Position {
    // 构造函数
    explicit Position(const QPointF& p = QPointF(0, 0), bool e = false) {
      position = p;  // 位置坐标
      expanded = e;  // 是否展开
    }

    // 从 XML 流中加载位置数据
    bool load(QXmlStreamReader* reader);
    // 将位置数据保存到 XML 流
    void save(QXmlStreamWriter* writer) const;

    QPointF position;  // 节点在编辑器中的位置
    bool expanded;     // 节点在编辑器中是否展开显示更多信息

    // 重载 += 运算符，用于位置相加
    inline Position& operator+=(const Position& p) {
      position += p.position;
      return *this;
    }

    // 重载 -= 运算符，用于位置相减
    inline Position& operator-=(const Position& p) {
      position -= p.position;
      return *this;
    }

    // 友元函数，重载 + 运算符
    friend inline Position operator+(Position a, const Position& b) {
      a += b;
      return a;
    }

    // 友元函数，重载 - 运算符
    friend inline Position operator-(Position a, const Position& b) {
      a -= b;
      return a;
    }
  };

  using PositionMap = QHash<Node*, Position>;  // 节点指针到其位置信息的哈希表类型定义
  // 获取所有上下文节点的位置信息
  [[nodiscard]] const PositionMap& GetContextPositions() const { return context_positions_; }

  // 检查指定节点在当前上下文中是否展开
  bool IsNodeExpandedInContext(Node* node) const { return context_positions_.value(node).expanded; }

  // 检查当前上下文是否包含指定节点
  bool ContextContainsNode(Node* node) const { return context_positions_.contains(node); }

  // 获取指定节点在当前上下文中的位置数据 (Position 结构体)
  Position GetNodePositionDataInContext(Node* node) { return context_positions_.value(node); }

  // 获取指定节点在当前上下文中的位置坐标 (QPointF)
  QPointF GetNodePositionInContext(Node* node) { return GetNodePositionDataInContext(node).position; }

  // 设置指定节点在当前上下文中的位置
  bool SetNodePositionInContext(Node* node, const QPointF& pos);

  // 设置指定节点在当前上下文中的位置数据
  bool SetNodePositionInContext(Node* node, const Position& pos);

  // 设置指定节点在当前上下文中是否展开
  void SetNodeExpandedInContext(Node* node, bool e) { context_positions_[node].expanded = e; }

  // 从当前上下文中移除指定节点的位置信息
  bool RemoveNodeFromContext(Node* node);

  /**
   * @brief 获取此节点的颜色。
   * @return 返回节点的颜色 (Color 类型，通常是 QColor 的封装)。
   */
  [[nodiscard]] Color color() const;

  /**
   * @brief 与 color() 类似，但返回一个漂亮的渐变色版本。
   * @param top 渐变的顶部位置。
   * @param bottom 渐变的底部位置。
   * @return 返回 QLinearGradient 对象。
   */
  [[nodiscard]] QLinearGradient gradient_color(qreal top, qreal bottom) const;

  /**
   * @brief 根据配置使用平面颜色 (color()) 或渐变色 (gradient_color())。
   * @param top 渐变的顶部位置 (如果使用渐变)。
   * @param bottom 渐变的底部位置 (如果使用渐变)。
   * @return 返回 QBrush 对象，用于绘制。
   */
  [[nodiscard]] QBrush brush(qreal top, qreal bottom) const;

  /**
   * @brief 获取节点的覆盖颜色索引。
   * @return 返回颜色索引；如果为 -1，则表示没有覆盖颜色。
   */
  [[nodiscard]] int GetOverrideColor() const { return override_color_; }

  /**
   * @brief 设置覆盖颜色。设置为 -1 表示不使用覆盖颜色。
   * @param index 颜色索引。
   */
  void SetOverrideColor(int index) {
    if (override_color_ != index) {
      override_color_ = index;
      emit ColorChanged();  // 发出颜色已改变信号
    }
  }

  // 静态方法：连接一个输出节点的输出到另一个节点的输入
  static void ConnectEdge(Node* output, const NodeInput& input);

  // 静态方法：断开一个输出节点与一个输入节点之间的连接
  static void DisconnectEdge(Node* output, const NodeInput& input);

  // 从另一个节点 n 拷贝缓存的 UUID
  void CopyCacheUuidsFrom(Node* n);

  // 检查缓存是否启用
  [[nodiscard]] bool AreCachesEnabled() const { return caches_enabled_; }
  // 设置缓存是否启用
  void SetCachesEnabled(bool e) { caches_enabled_ = e; }

  /**
   * @brief 获取指定输入端口的用户友好名称。
   * @param id 输入端口的ID。
   * @return 返回输入端口的名称。
   */
  [[nodiscard]] virtual QString GetInputName(const QString& id) const;

  // 设置指定输入端口的用户友好名称
  void SetInputName(const QString& id, const QString& name);

  // 检查指定输入是否隐藏
  [[nodiscard]] bool IsInputHidden(const QString& input) const;
  // 检查指定输入是否可连接
  [[nodiscard]] bool IsInputConnectable(const QString& input) const;
  // 检查指定输入是否可设置关键帧
  [[nodiscard]] bool IsInputKeyframable(const QString& input) const;

  // 检查指定输入 (的特定元素) 是否正在使用关键帧
  [[nodiscard]] bool IsInputKeyframing(const QString& input, int element = -1) const;
  // 检查指定 NodeInput 是否正在使用关键帧
  [[nodiscard]] bool IsInputKeyframing(const NodeInput& input) const {
    return IsInputKeyframing(input.input(), input.element());
  }

  // 设置指定输入 (的特定元素) 是否使用关键帧
  void SetInputIsKeyframing(const QString& input, bool e, int element = -1);
  // 设置指定 NodeInput 是否使用关键帧
  void SetInputIsKeyframing(const NodeInput& input, bool e) { SetInputIsKeyframing(input.input(), e, input.element()); }

  // 检查指定输入 (的特定元素) 是否已连接
  [[nodiscard]] bool IsInputConnected(const QString& input, int element = -1) const;
  // 检查指定 NodeInput 是否已连接
  [[nodiscard]] bool IsInputConnected(const NodeInput& input) const {
    return IsInputConnected(input.input(), input.element());
  }

  // 检查指定输入 (的特定元素) 是否为渲染目的而连接 (通常与 IsInputConnected 相同，但可重写)
  [[nodiscard]] virtual bool IsInputConnectedForRender(const QString& input, int element = -1) const {
    return IsInputConnected(input, element);
  }
  // 检查指定 NodeInput 是否为渲染目的而连接
  [[nodiscard]] bool IsInputConnectedForRender(const NodeInput& input) const {
    return IsInputConnectedForRender(input.input(), input.element());
  }

  // 检查指定输入 (的特定元素) 是否为静态值 (既未连接也未使用关键帧)
  [[nodiscard]] bool IsInputStatic(const QString& input, int element = -1) const {
    return !IsInputConnected(input, element) && !IsInputKeyframing(input, element);
  }
  // 检查指定 NodeInput 是否为静态值
  [[nodiscard]] bool IsInputStatic(const NodeInput& input) const {
    return IsInputStatic(input.input(), input.element());
  }

  // 获取连接到指定输入 (的特定元素) 的输出节点
  [[nodiscard]] Node* GetConnectedOutput(const QString& input, int element = -1) const;
  // 获取连接到指定 NodeInput 的输出节点
  [[nodiscard]] Node* GetConnectedOutput(const NodeInput& input) const {
    return GetConnectedOutput(input.input(), input.element());
  }

  // 获取为渲染目的连接到指定输入 (的特定元素) 的输出节点
  [[nodiscard]] virtual Node* GetConnectedRenderOutput(const QString& input, int element = -1) const {
    return GetConnectedOutput(input, element);
  }
  // 获取为渲染目的连接到指定 NodeInput 的输出节点
  [[nodiscard]] Node* GetConnectedRenderOutput(const NodeInput& input) const {
    return GetConnectedRenderOutput(input.input(), input.element());
  }

  // 检查指定输入在特定轨道和元素上是否使用标准值 (而不是关键帧或连接)
  [[nodiscard]] bool IsUsingStandardValue(const QString& input, int track, int element = -1) const;

  // 获取指定输入ID的数据类型
  [[nodiscard]] NodeValue::Type GetInputDataType(const QString& id) const;
  // 设置指定输入ID的数据类型
  void SetInputDataType(const QString& id, const NodeValue::Type& type);

  // 检查指定输入ID是否具有名为 'name' 的属性
  [[nodiscard]] bool HasInputProperty(const QString& id, const QString& name) const;
  // 获取指定输入ID的所有属性
  [[nodiscard]] QHash<QString, QVariant> GetInputProperties(const QString& id) const;
  // 获取指定输入ID的名为 'name' 的属性值
  [[nodiscard]] QVariant GetInputProperty(const QString& id, const QString& name) const;
  // 设置指定输入ID的名为 'name' 的属性值
  void SetInputProperty(const QString& id, const QString& name, const QVariant& value);

  /**
   * @brief 获取指定输入在特定时间的组合值 (如果输入是多轨的，则合并轨道值)。
   * @param input 输入端口的ID。
   * @param time 时间点。
   * @param element 元素的索引 (-1 表示所有元素或非数组情况)。
   * @return 返回 QVariant 形式的值。
   */
  [[nodiscard]] QVariant GetValueAtTime(const QString& input, const rational& time, int element = -1) const {
    NodeValue::Type type = GetInputDataType(input);
    // 将分离的轨道值合并为普通值
    return NodeValue::combine_track_values_into_normal_value(type, GetSplitValueAtTime(input, time, element));
  }
  // NodeInput 重载版本
  [[nodiscard]] QVariant GetValueAtTime(const NodeInput& input, const rational& time) const {
    return GetValueAtTime(input.input(), time, input.element());
  }

  /**
   * @brief 获取指定输入在特定时间的分离值 (SplitValue)。
   * @param input 输入端口的ID。
   * @param time 时间点。
   * @param element 元素的索引。
   * @return 返回 SplitValue。
   */
  [[nodiscard]] SplitValue GetSplitValueAtTime(const QString& input, const rational& time, int element = -1) const;
  // NodeInput 重载版本
  [[nodiscard]] SplitValue GetSplitValueAtTime(const NodeInput& input, const rational& time) const {
    return GetSplitValueAtTime(input.input(), time, input.element());
  }

  // 获取指定输入在特定时间、特定轨道上的分离值
  [[nodiscard]] QVariant GetSplitValueAtTimeOnTrack(const QString& input, const rational& time, int track,
                                                    int element = -1) const;
  // NodeInput 重载版本
  [[nodiscard]] QVariant GetSplitValueAtTimeOnTrack(const NodeInput& input, const rational& time, int track) const {
    return GetSplitValueAtTimeOnTrack(input.input(), time, track, input.element());
  }
  // NodeKeyframeTrackReference 重载版本
  [[nodiscard]] QVariant GetSplitValueAtTimeOnTrack(const NodeKeyframeTrackReference& input,
                                                    const rational& time) const {
    return GetSplitValueAtTimeOnTrack(input.input(), time, input.track());
  }

  // 获取指定输入的默认值
  [[nodiscard]] QVariant GetDefaultValue(const QString& input) const;
  // 获取指定输入的分离形式的默认值
  [[nodiscard]] SplitValue GetSplitDefaultValue(const QString& input) const;
  // 获取指定输入在特定轨道上的分离形式的默认值
  [[nodiscard]] QVariant GetSplitDefaultValueOnTrack(const QString& input, int track) const;

  // 设置指定输入的默认值
  void SetDefaultValue(const QString& input, const QVariant& val);
  // 设置指定输入的分离形式的默认值
  void SetSplitDefaultValue(const QString& input, const SplitValue& val);
  // 设置指定输入在特定轨道上的分离形式的默认值
  void SetSplitDefaultValueOnTrack(const QString& input, const QVariant& val, int track);

  // 获取指定输入 (特定元素) 的所有关键帧轨道
  [[nodiscard]] const QVector<NodeKeyframeTrack>& GetKeyframeTracks(const QString& input, int element) const;
  // NodeInput 重载版本
  [[nodiscard]] const QVector<NodeKeyframeTrack>& GetKeyframeTracks(const NodeInput& input) const {
    return GetKeyframeTracks(input.input(), input.element());
  }

  // 获取指定输入在特定时间 (特定元素) 的所有关键帧
  [[nodiscard]] QVector<NodeKeyframe*> GetKeyframesAtTime(const QString& input, const rational& time,
                                                          int element = -1) const;
  // NodeInput 重载版本
  [[nodiscard]] QVector<NodeKeyframe*> GetKeyframesAtTime(const NodeInput& input, const rational& time) const {
    return GetKeyframesAtTime(input.input(), time, input.element());
  }

  // 获取指定输入在特定时间、特定轨道 (特定元素) 上的关键帧
  [[nodiscard]] NodeKeyframe* GetKeyframeAtTimeOnTrack(const QString& input, const rational& time, int track,
                                                       int element = -1) const;
  // NodeInput 重载版本
  [[nodiscard]] NodeKeyframe* GetKeyframeAtTimeOnTrack(const NodeInput& input, const rational& time, int track) const {
    return GetKeyframeAtTimeOnTrack(input.input(), time, track, input.element());
  }
  // NodeKeyframeTrackReference 重载版本
  [[nodiscard]] NodeKeyframe* GetKeyframeAtTimeOnTrack(const NodeKeyframeTrackReference& input,
                                                       const rational& time) const {
    return GetKeyframeAtTimeOnTrack(input.input(), time, input.track());
  }

  // 获取在特定时间、特定轨道 (特定元素) 上最适合的关键帧类型 (例如，如果时间点正好在某个关键帧上，则返回其类型)
  [[nodiscard]] NodeKeyframe::Type GetBestKeyframeTypeForTimeOnTrack(const QString& input, const rational& time,
                                                                     int track, int element = -1) const;
  // NodeInput 重载版本
  [[nodiscard]] NodeKeyframe::Type GetBestKeyframeTypeForTimeOnTrack(const NodeInput& input, const rational& time,
                                                                     int track) const {
    return GetBestKeyframeTypeForTimeOnTrack(input.input(), time, track, input.element());
  }
  // NodeKeyframeTrackReference 重载版本
  [[nodiscard]] NodeKeyframe::Type GetBestKeyframeTypeForTimeOnTrack(const NodeKeyframeTrackReference& input,
                                                                     const rational& time) const {
    return GetBestKeyframeTypeForTimeOnTrack(input.input(), time, input.track());
  }

  // 获取指定输入ID的关键帧轨道数量
  [[nodiscard]] int GetNumberOfKeyframeTracks(const QString& id) const;
  // NodeInput 重载版本
  [[nodiscard]] int GetNumberOfKeyframeTracks(const NodeInput& id) const {
    return GetNumberOfKeyframeTracks(id.input());
  }

  // 获取指定输入 (特定元素) 的最早关键帧
  [[nodiscard]] NodeKeyframe* GetEarliestKeyframe(const QString& id, int element = -1) const;
  // NodeInput 重载版本
  [[nodiscard]] NodeKeyframe* GetEarliestKeyframe(const NodeInput& id) const {
    return GetEarliestKeyframe(id.input(), id.element());
  }

  // 获取指定输入 (特定元素) 的最晚关键帧
  [[nodiscard]] NodeKeyframe* GetLatestKeyframe(const QString& id, int element = -1) const;
  // NodeInput 重载版本
  [[nodiscard]] NodeKeyframe* GetLatestKeyframe(const NodeInput& id) const {
    return GetLatestKeyframe(id.input(), id.element());
  }

  // 获取指定输入在特定时间点之前最近的关键帧 (特定元素)
  [[nodiscard]] NodeKeyframe* GetClosestKeyframeBeforeTime(const QString& id, const rational& time,
                                                           int element = -1) const;
  // NodeInput 重载版本
  [[nodiscard]] NodeKeyframe* GetClosestKeyframeBeforeTime(const NodeInput& id, const rational& time) const {
    return GetClosestKeyframeBeforeTime(id.input(), time, id.element());
  }

  // 获取指定输入在特定时间点之后最近的关键帧 (特定元素)
  [[nodiscard]] NodeKeyframe* GetClosestKeyframeAfterTime(const QString& id, const rational& time,
                                                          int element = -1) const;
  // NodeInput 重载版本
  [[nodiscard]] NodeKeyframe* GetClosestKeyframeAfterTime(const NodeInput& id, const rational& time) const {
    return GetClosestKeyframeAfterTime(id.input(), time, id.element());
  }

  // 检查指定输入在特定时间点是否存在关键帧 (特定元素)
  [[nodiscard]] bool HasKeyframeAtTime(const QString& id, const rational& time, int element = -1) const;
  // NodeInput 重载版本
  [[nodiscard]] bool HasKeyframeAtTime(const NodeInput& id, const rational& time) const {
    return HasKeyframeAtTime(id.input(), time, id.element());
  }

  // 如果输入是下拉框类型，获取其选项字符串列表
  [[nodiscard]] QStringList GetComboBoxStrings(const QString& id) const;

  // 获取指定输入 (特定元素) 的标准值 (即非连接、非关键帧时的值)
  [[nodiscard]] QVariant GetStandardValue(const QString& id, int element = -1) const;
  // NodeInput 重载版本
  [[nodiscard]] QVariant GetStandardValue(const NodeInput& id) const {
    return GetStandardValue(id.input(), id.element());
  }

  // 获取指定输入 (特定元素) 的分离形式的标准值
  [[nodiscard]] SplitValue GetSplitStandardValue(const QString& id, int element = -1) const;
  // NodeInput 重载版本
  [[nodiscard]] SplitValue GetSplitStandardValue(const NodeInput& id) const {
    return GetSplitStandardValue(id.input(), id.element());
  }

  // 获取指定输入在特定轨道 (特定元素) 上的分离形式的标准值
  [[nodiscard]] QVariant GetSplitStandardValueOnTrack(const QString& input, int track, int element = -1) const;
  // NodeKeyframeTrackReference 重载版本
  [[nodiscard]] QVariant GetSplitStandardValueOnTrack(const NodeKeyframeTrackReference& id) const {
    return GetSplitStandardValueOnTrack(id.input().input(), id.track(), id.input().element());
  }

  // 设置指定输入 (特定元素) 的标准值
  void SetStandardValue(const QString& id, const QVariant& value, int element = -1);
  // NodeInput 重载版本
  void SetStandardValue(const NodeInput& id, const QVariant& value) {
    SetStandardValue(id.input(), value, id.element());
  }

  // 设置指定输入 (特定元素) 的分离形式的标准值
  void SetSplitStandardValue(const QString& id, const SplitValue& value, int element = -1);
  // NodeInput 重载版本
  void SetSplitStandardValue(const NodeInput& id, const SplitValue& value) {
    SetSplitStandardValue(id.input(), value, id.element());
  }

  // 设置指定输入在特定轨道 (特定元素) 上的分离形式的标准值
  void SetSplitStandardValueOnTrack(const QString& id, int track, const QVariant& value, int element = -1);
  // NodeKeyframeTrackReference 重载版本
  void SetSplitStandardValueOnTrack(const NodeKeyframeTrackReference& id, const QVariant& value) {
    SetSplitStandardValueOnTrack(id.input().input(), id.track(), value, id.input().element());
  }

  // 检查指定输入是否为数组类型
  [[nodiscard]] bool InputIsArray(const QString& id) const;

  // 在指定输入的数组中指定索引处插入一个元素
  void InputArrayInsert(const QString& id, int index);
  // 调整指定输入数组的大小
  void InputArrayResize(const QString& id, int size);
  // 从指定输入的数组中移除指定索引处的元素
  void InputArrayRemove(const QString& id, int index);

  // 在指定输入数组的末尾追加一个元素
  void InputArrayAppend(const QString& id) { InputArrayResize(id, InputArraySize(id) + 1); }

  // 在指定输入数组的开头插入一个元素
  void InputArrayPrepend(const QString& id) { InputArrayInsert(id, 0); }

  // 移除指定输入数组的最后一个元素
  void InputArrayRemoveLast(const QString& id) { InputArrayResize(id, InputArraySize(id) - 1); }

  // 获取指定输入数组的大小
  [[nodiscard]] int InputArraySize(const QString& id) const;

  // 获取指定输入 (特定元素) 的立即值对象 (NodeInputImmediate)
  [[nodiscard]] NodeInputImmediate* GetImmediate(const QString& input, int element) const;

  // 获取效果输入 (通常用于效果节点，指明哪个输入是主要的被处理对象)
  NodeInput GetEffectInput() { return effect_input_.isEmpty() ? NodeInput() : NodeInput(this, effect_input_); }

  // 获取效果输入的ID
  [[nodiscard]] const QString& GetEffectInputID() const { return effect_input_; }

  // 值提示类，用于向UI或其他系统提供关于输入值的额外信息
  class ValueHint {
   public:
    // 构造函数
    explicit ValueHint(const QVector<NodeValue::Type>& types = QVector<NodeValue::Type>(), int index = -1,
                       QString tag = QString())
        : type_(types), index_(index), tag_(std::move(tag)) {}

    explicit ValueHint(const QVector<NodeValue::Type>& types, QString tag)
        : type_(types), index_(-1), tag_(std::move(tag)) {}

    explicit ValueHint(int index) : index_(index) {}

    explicit ValueHint(QString tag) : index_(-1), tag_(std::move(tag)) {}

    // 获取允许的数据类型列表
    [[nodiscard]] const QVector<NodeValue::Type>& types() const { return type_; }
    // 获取索引提示
    [[nodiscard]] const int& index() const { return index_; }
    // 获取标签提示
    [[nodiscard]] const QString& tag() const { return tag_; }

    // 设置允许的数据类型列表
    void set_type(const QVector<NodeValue::Type>& type) { type_ = type; }
    // 设置索引提示
    void set_index(const int& index) { index_ = index; }
    // 设置标签提示
    void set_tag(const QString& tag) { tag_ = tag; }

    // 从 XML 流加载值提示
    bool load(QXmlStreamReader* reader);
    // 将值提示保存到 XML 流
    void save(QXmlStreamWriter* writer) const;

   private:
    QVector<NodeValue::Type> type_;  // 允许的数据类型
    int index_;                      // 索引提示，例如用于数组或向量分量
    QString tag_;                    // 标签字符串，用于特定用途的标识
  };

  // 获取所有输入元素对 (InputElementPair) 到其值提示 (ValueHint) 的映射
  [[nodiscard]] const QMap<InputElementPair, ValueHint>& GetValueHints() const { return value_hints_; }

  // 获取指定输入 (特定元素) 的值提示
  [[nodiscard]] ValueHint GetValueHintForInput(const QString& input, int element = -1) const {
    return value_hints_.value({input, element});
  }

  // 为指定输入 (特定元素) 设置值提示
  void SetValueHintForInput(const QString& input, const ValueHint& hint, int element = -1);

  // 根据关键帧对象获取其所属的关键帧轨道
  const NodeKeyframeTrack& GetTrackFromKeyframe(NodeKeyframe* key) const;

  // 输入连接的类型定义：NodeInput (本节点的输入) -> Node* (连接到此输入的上游输出节点)
  using InputConnections = std::map<NodeInput, Node*>;

  /**
   * @brief 返回输入连接的映射。
   *
   * 输入只能有一个连接，因此键是连接的输入，值是它连接到的输出节点。
   * @return 返回输入连接的常量引用。
   */
  [[nodiscard]] const InputConnections& input_connections() const { return input_connections_; }

  // 输出连接对的类型定义：Node* (下游输入节点) -> NodeInput (下游节点的具体输入端口)
  using OutputConnection = std::pair<Node*, NodeInput>;
  // 输出连接列表的类型定义
  using OutputConnections = std::vector<OutputConnection>;

  /**
   * @brief 返回输出连接的列表。
   *
   * 一个输出可以连接到无限数量的输入，因此这是一个包含所有输出连接的列表。
   * @return 返回输出连接的常量引用。
   */
  [[nodiscard]] const OutputConnections& output_connections() const { return output_connections_; }

  /**
   * @brief 返回此节点输入所连接到的所有上游节点列表 (不包括此节点本身)。
   * @return 返回一个包含上游 Node 指针的 QVector。
   */
  [[nodiscard]] QVector<Node*> GetDependencies() const;

  /**
   * @brief 返回此节点所依赖的节点列表，前提是这些依赖节点在此层级结构之外没有被其他节点依赖。
   *
   * 与 GetDependencies() 类似，但排除了在此节点依赖图之外被使用的任何节点。
   * @return 返回一个包含独占依赖 Node 指针的 QVector。
   */
  [[nodiscard]] QVector<Node*> GetExclusiveDependencies() const;

  /**
   * @brief 获取直接依赖项 (仅直接连接到此节点输入的节点)。
   * @return 返回一个包含直接上游 Node 指针的 QVector。
   */
  [[nodiscard]] QVector<Node*> GetImmediateDependencies() const;

  // 着色器请求结构体
  struct ShaderRequest {
    // 构造函数，仅ID
    explicit ShaderRequest(const QString& shader_id) { id = shader_id; }

    // 构造函数，ID和代码片段
    ShaderRequest(const QString& shader_id, const QString& shader_stub) {
      id = shader_id;
      stub = shader_stub;
    }

    QString id;    // 着色器的唯一ID
    QString stub;  // 着色器代码片段 (可选)
  };

  /**
   * @brief为此节点生成硬件加速代码 (着色器代码)。
   * @param request 着色器请求参数。
   * @return 返回 ShaderCode 对象。
   */
  [[nodiscard]] virtual ShaderCode GetShaderCode(const ShaderRequest& request) const;

  /**
   * @brief 如果 Value() 推送了一个 ShaderJob，此函数将处理它们。
   * (通常用于CPU端对采样数据的后处理或特定计算)
   * @param values 当前行节点值。
   * @param input 输入采样缓冲区。
   * @param output 输出采样缓冲区。
   * @param index 索引 (例如，通道索引)。
   */
  virtual void ProcessSamples(const NodeValueRow& values, const SampleBuffer& input, SampleBuffer& output,
                              int index) const;

  /**
   * @brief 如果 Value() 推送了一个 GenerateJob (通常用于生成器节点)，则重写此函数以创建图像。
   * @param frame 目标帧缓冲区。它将已经被分配并准备好写入。
   * @param job 生成任务的详细信息。
   */
  virtual void GenerateFrame(FramePtr frame, const GenerateJob& job) const;

  /**
   * @brief 返回此节点是否从特定节点实例接收输入 (可递归检查)。
   * @param n 要检查的源节点。
   * @param recursively 是否递归向上游检查。
   * @return 如果接收输入则返回 true。
   */
  bool InputsFrom(Node* n, bool recursively) const;

  /**
   * @brief 返回此节点是否从具有特定ID的节点接收输入 (可递归检查)。
   * @param id 要检查的源节点的ID。
   * @param recursively 是否递归向上游检查。
   * @return 如果接收输入则返回 true。
   */
  [[nodiscard]] bool InputsFrom(const QString& id, bool recursively) const;

  /**
   * @brief 查找 `output` 节点输出到此节点的路径中的输入端口。
   *
   * 递归遍历此节点的输入，查找 `output` 节点，并返回 `output` 节点用于到达此节点的边 (NodeInput) 列表。
   * @param output 要查找的上游输出节点。
   * @return 返回一个 NodeInput 列表。
   */
  QVector<NodeInput> FindWaysNodeArrivesHere(const Node* output) const;

  /**
   * @brief 断开所有输入和输出连接。
   */
  void DisconnectAll();

  /**
   * @brief 获取任何分类ID的人类可读名称。
   * @param c 分类ID。
   * @return 返回分类名称字符串。
   */
  static QString GetCategoryName(const CategoryID& c);

  // 时间变换方向枚举
  enum TransformTimeDirection { kTransformTowardsInput, kTransformTowardsOutput };  // 朝输入方向变换，朝输出方向变换

  /**
   * @brief 将时间从此节点通过其连接转换到指定目标节点。
   * @param time 要转换的时间范围。
   * @param target 目标节点。
   * @param dir 转换方向 (朝输入或朝输出)。
   * @param path_index 路径索引 (如果存在多条路径)。
   * @return 返回转换后的时间范围。
   */
  TimeRange TransformTimeTo(TimeRange time, Node* target, TransformTimeDirection dir, int path_index);

  /**
   * @brief 查找此节点从其获取输入的特定类型的节点。
   * @tparam T 要查找的节点类型。
   * @param maximum 要查找的最大节点数 (0表示无限制)。
   * @return 返回一个包含 T 类型节点指针的 QVector。
   */
  template <class T>
  QVector<T*> FindInputNodes(int maximum = 0) const;

  /**
   * @brief 查找连接到特定输入端口的特定类型的上游节点。
   * @tparam T 要查找的节点类型。
   * @param input 目标输入端口。
   * @param maximum 要查找的最大节点数 (0表示无限制)。
   * @return 返回一个包含 T 类型节点指针的 QVector。
   */
  template <class T>
  static QVector<T*> FindInputNodesConnectedToInput(const NodeInput& input, int maximum = 0);

  // 使缓存无效的选项类型定义
  using InvalidateCacheOptions = QHash<QString, QVariant>;

  /**
   * @brief 通知所有依赖此节点的下游节点，在指定时间范围内的任何缓存内容都已失效，需要重新渲染。
   *
   * 如果您的 Node 子类维护缓存，请重写此函数，但在子类函数的末尾调用此基类函数。
   * 默认行为是将此信号中继到所有连接的输出，这样做是为了不破坏有向无环图（DAG）的正确性。
   * 即使时间需要以某种方式转换（例如，将媒体时间转换为序列时间），您也可以使用转换后的时间调用此函数并以此方式中继信号。
   * @param range 时间范围。
   * @param from 导致缓存失效的输入端口ID。
   * @param element 输入端口的元素索引。
   * @param options 额外的选项。
   */
  virtual void InvalidateCache(const TimeRange& range, const QString& from, int element = -1,
                               InvalidateCacheOptions options = InvalidateCacheOptions());

  // NodeInput 重载版本的 InvalidateCache
  void InvalidateCache(const TimeRange& range, const NodeInput& from,
                       const InvalidateCacheOptions& options = InvalidateCacheOptions()) {
    InvalidateCache(range, from.input(), from.element(), options);
  }

  /**
   * @brief 调整应发送到连接到某些输入的节点的时间。
   *
   * 如果此节点修改 `time`（例如，一个片段将序列时间转换为媒体时间），则应重写此函数来实现。
   * 确保同时重写 OutputTimeAdjustment() 以提供反向函数。
   * @param input 输入端口ID。
   * @param element 元素索引。
   * @param input_time 输入的时间范围。
   * @param clamp 是否限制时间范围。
   * @return 返回调整后的时间范围。
   */
  [[nodiscard]] virtual TimeRange InputTimeAdjustment(const QString& input, int element, const TimeRange& input_time,
                                                      bool clamp) const;

  /**
   * @brief InputTimeAdjustment() 的反向函数。
   * @param input 输入端口ID。
   * @param element 元素索引。
   * @param input_time 输入的时间范围 (通常是 OutputTimeAdjustment 调整后的时间)。
   * @return 返回原始的或反向调整的时间范围。
   */
  [[nodiscard]] virtual TimeRange OutputTimeAdjustment(const QString& input, int element,
                                                       const TimeRange& input_time) const;

  /**
   * @brief 将输入从源节点复制到目标节点，包括连接。
   *
   * 节点必须是相同类型（即具有相同的ID）。
   * @param source 源节点。
   * @param destination 目标节点。
   * @param include_connections 是否包括连接。
   * @param command 可选的撤销命令对象。
   */
  static void CopyInputs(const Node* source, Node* destination, bool include_connections = true,
                         MultiUndoCommand* command = nullptr);

  // 静态方法：复制单个输入从源节点到目标节点
  static void CopyInput(const Node* src, Node* dst, const QString& input, bool include_connections,
                        bool traverse_arrays, MultiUndoCommand* command);

  // 静态方法：复制指定输入特定元素的值从源节点到目标节点
  static void CopyValuesOfElement(const Node* src, Node* dst, const QString& input, int src_element, int dst_element,
                                  MultiUndoCommand* command = nullptr);
  // 重载版本，源元素和目标元素索引相同
  static void CopyValuesOfElement(const Node* src, Node* dst, const QString& input, int element,
                                  MultiUndoCommand* command = nullptr) {
    return CopyValuesOfElement(src, dst, input, element, element, command);
  }

  /**
   * @brief 克隆一组节点，并按旧节点的方式连接新节点。
   * @param nodes 要复制的节点列表。
   * @param command 可选的撤销命令对象。
   * @return 返回新创建的节点列表。
   */
  static QVector<Node*> CopyDependencyGraph(const QVector<Node*>& nodes, MultiUndoCommand* command);
  // 静态方法：将源节点列表的依赖图结构复制到目标节点列表
  static void CopyDependencyGraph(const QVector<Node*>& src, const QVector<Node*>& dst, MultiUndoCommand* command);

  // 静态方法：复制节点及其依赖图，但不包括标记为 Item 的节点
  static Node* CopyNodeAndDependencyGraphMinusItems(Node* node, MultiUndoCommand* command);

  // 静态方法：在图中复制单个节点 (通常意味着也复制其必要的依赖)
  static Node* CopyNodeInGraph(Node* node, MultiUndoCommand* command);

  /**
   * @brief 主要的处理函数。
   *
   * 节点的主要目的是从输入中获取值以设置输出中的值。对于您创建的任何子类节点，
   * 这就是其代码所在的位置。
   *
   * 注意，作为视频编辑器，节点图必须在时间上工作。根据节点的目的，
   * 它可能会根据时间输出不同的值，即使不是这样，它也可能根据时间接收不同的输入。
   * 这里大部分困难的工作由 NodeInput::get_value() 处理，您应该将 `time` 参数传递给它。
   * 它将返回其值（在那个时间，如果它是关键帧化的），或者如果它连接到一个相应的输出，则将时间传递给该输出。
   * 如果您的节点不直接处理时间，NodeParam 对象的默认行为将自动处理所有与之相关的事情。
   * @param value 当前处理行所需的值 (通常来自 NodeValueRow)。
   * @param globals 全局渲染上下文信息。
   * @param table 用于存储输出值的表。
   */
  virtual void Value(const NodeValueRow& value, const NodeGlobals& globals, NodeValueTable* table) const;

  // 检查节点是否有 Gizmo (屏幕控制器)
  [[nodiscard]] bool HasGizmos() const { return !gizmos_.isEmpty(); }

  // 获取节点的所有 Gizmo
  [[nodiscard]] const QVector<NodeGizmo*>& GetGizmos() const { return gizmos_; }

  /**
   * @brief 获取 Gizmo 的变换矩阵。
   * @param row 当前节点值行。
   * @param globals 全局信息。
   * @return 返回 QTransform 变换矩阵。
   */
  [[nodiscard]] virtual QTransform GizmoTransformation(const NodeValueRow& row, const NodeGlobals& globals) const {
    return {};  // 默认返回单位变换
  }

  // 更新 Gizmo 的位置 (通常在节点参数改变后调用)
  virtual void UpdateGizmoPositions(const NodeValueRow& row, const NodeGlobals& globals) {}

  // 获取节点的自定义标签
  [[nodiscard]] const QString& GetLabel() const;
  // 设置节点的自定义标签
  void SetLabel(const QString& s);

  // 获取节点的标签和名称组合 (例如 "标签 (名称)")
  [[nodiscard]] QString GetLabelAndName() const;
  // 获取节点的标签，如果标签为空则返回名称
  [[nodiscard]] QString GetLabelOrName() const;

  // 使与指定输入 (特定元素) 相关的所有缓存等失效
  void InvalidateAll(const QString& input, int element = -1);

  // 检查节点是否有链接 (与其他节点的特殊关联)
  [[nodiscard]] bool HasLinks() const { return !links_.isEmpty(); }

  // 获取与此节点链接的所有节点
  [[nodiscard]] const QVector<Node*>& links() const { return links_; }

  // 静态方法：链接两个节点
  static bool Link(Node* a, Node* b);
  // 静态方法：取消两个节点之间的链接
  static bool Unlink(Node* a, Node* b);
  // 静态方法：检查两个节点是否已链接
  static bool AreLinked(Node* a, Node* b);

  // 从 XML 流加载节点数据
  bool Load(QXmlStreamReader* reader, SerializedData* data);
  // 将节点数据保存到 XML 流
  void Save(QXmlStreamWriter* writer) const;

  // 加载子类特有的自定义数据
  virtual bool LoadCustom(QXmlStreamReader* reader, SerializedData* data);
  // 保存子类特有的自定义数据
  virtual void SaveCustom(QXmlStreamWriter* writer) const {}
  // 所有数据加载完成后的后处理事件
  virtual void PostLoadEvent(SerializedData* data);

  // 从 XML 流加载单个输入的数据
  bool LoadInput(QXmlStreamReader* reader, SerializedData* data);
  // 将单个输入的数据保存到 XML 流
  void SaveInput(QXmlStreamWriter* writer, const QString& id) const;

  // 从 XML 流加载立即值数据
  bool LoadImmediate(QXmlStreamReader* reader, const QString& input, int element, SerializedData* data);
  // 将立即值数据保存到 XML 流
  void SaveImmediate(QXmlStreamWriter* writer, const QString& input, int element) const;

  // 设置节点所在的文件夹
  void SetFolder(Folder* folder) { folder_ = folder; }

  // 获取指定输入的标志位
  [[nodiscard]] InputFlags GetInputFlags(const QString& input) const;
  // 设置指定输入的某个标志位
  void SetInputFlag(const QString& input, InputFlag f, bool on = true);

  // 加载完成事件 (通常在整个图加载完毕后触发)
  virtual void LoadFinishedEvent() {}
  // 连接到预览窗口事件
  virtual void ConnectedToPreviewEvent() {}

  // 静态方法：在特定时间为指定输入设置值 (可能会创建关键帧)
  static void SetValueAtTime(const NodeInput& input, const rational& time, const QVariant& value, int track,
                             MultiUndoCommand* command, bool insert_on_all_tracks_if_no_key);

  /**
   * @brief 查找从 `from` 节点开始到 `to` 节点的输出路径。
   * @param from 起始节点。
   * @param to 目标节点。
   * @param path_index 路径索引 (如果存在多条路径)。
   * @return 返回一个 NodeInput 列表，表示路径上的连接点。
   */
  static std::list<NodeInput> FindPath(Node* from, Node* to, int path_index);

  // 内部方法：调整数组输入的大小
  void ArrayResizeInternal(const QString& id, int size);

  // 节点被添加到图 (项目) 中的事件
  virtual void AddedToGraphEvent(Project* p) {}
  // 节点从图 (项目) 中移除的事件
  virtual void RemovedFromGraphEvent(Project* p) {}

  // 静态方法：获取连接操作的命令字符串 (用于撤销/重做系统)
  static QString GetConnectCommandString(Node* output, const NodeInput& input);
  // 静态方法：获取断开连接操作的命令字符串
  static QString GetDisconnectCommandString(Node* output, const NodeInput& input);

  static const QString kEnabledInput;  // "enabled" 输入端口的常量ID

 protected:
  // 在指定索引处插入一个新的输入端口
  void InsertInput(const QString& id, NodeValue::Type type, const QVariant& default_value, InputFlags flags, int index);

  // 在输入列表的开头添加一个新的输入端口
  void PrependInput(const QString& id, NodeValue::Type type, const QVariant& default_value,
                    InputFlags flags = InputFlags(kInputFlagNormal)) {
    InsertInput(id, type, default_value, flags, 0);
  }
  // 重载版本，使用默认的 QVariant() 作为默认值
  void PrependInput(const QString& id, NodeValue::Type type, InputFlags flags = InputFlags(kInputFlagNormal)) {
    PrependInput(id, type, QVariant(), flags);
  }

  // 在输入列表的末尾添加一个新的输入端口
  void AddInput(const QString& id, NodeValue::Type type, const QVariant& default_value,
                InputFlags flags = InputFlags(kInputFlagNormal)) {
    InsertInput(id, type, default_value, flags, input_ids_.size());
  }
  // 重载版本，使用默认的 QVariant() 作为默认值
  void AddInput(const QString& id, NodeValue::Type type, InputFlags flags = InputFlags(kInputFlagNormal)) {
    AddInput(id, type, QVariant(), flags);
  }

  // 移除指定ID的输入端口
  void RemoveInput(const QString& id);

  // 为下拉框类型的输入设置选项字符串
  void SetComboBoxStrings(const QString& id, const QStringList& strings) {
    SetInputProperty(id, QStringLiteral("combo_str"), strings);
  }

  // 发送缓存失效信号给下游连接的节点
  void SendInvalidateCache(const TimeRange& range, const InvalidateCacheOptions& options);

  // Gizmo 缩放控制柄的枚举
  enum GizmoScaleHandles {
    kGizmoScaleTopLeft,       // 左上角
    kGizmoScaleTopCenter,     // 顶部中心
    kGizmoScaleTopRight,      // 右上角
    kGizmoScaleBottomLeft,    // 左下角
    kGizmoScaleBottomCenter,  // 底部中心
    kGizmoScaleBottomRight,   // 右下角
    kGizmoScaleCenterLeft,    // 左侧中心
    kGizmoScaleCenterRight,   // 右侧中心
    kGizmoScaleCount,         // 控制柄总数
  };

  // 链接状态改变事件 (虚函数，子类可重写)
  virtual void LinkChangeEvent() {}

  // 输入值改变事件 (虚函数，子类可重写)
  virtual void InputValueChangedEvent(const QString& input, int element);

  // 输入端口连接事件 (虚函数，子类可重写)
  virtual void InputConnectedEvent(const QString& input, int element, Node* output);

  // 输入端口断开连接事件 (虚函数，子类可重写)
  virtual void InputDisconnectedEvent(const QString& input, int element, Node* output);

  // 本节点的输出端口被连接事件 (虚函数，子类可重写)
  virtual void OutputConnectedEvent(const NodeInput& input);

  // 本节点的输出端口被断开连接事件 (虚函数，子类可重写)
  virtual void OutputDisconnectedEvent(const NodeInput& input);

  // QObject 的子对象事件处理 (用于跟踪子 NodeInputImmediate 的删除)
  void childEvent(QChildEvent* event) override;

  // 设置效果输入端口的ID (受保护，供子类使用)
  void SetEffectInput(const QString& input) { effect_input_ = input; }

  // 设置节点的某个标志位 (受保护，供子类使用)
  void SetFlag(Flag f, bool on = true) {
    if (on) {
      flags_ |= f;  // 设置标志位
    } else {
      flags_ &= ~f;  // 清除标志位
    }
  }

  /**
   * @brief 添加一个可拖拽的 Gizmo。
   * @tparam T Gizmo 的类型，必须是 DraggableGizmo 的子类。
   * @param inputs 此 Gizmo 控制的输入参数引用列表。
   * @param behavior Gizmo 拖拽时的值行为。
   * @return 返回创建的 Gizmo 指针。
   */
  template <typename T>
  T* AddDraggableGizmo(const QVector<NodeKeyframeTrackReference>& inputs = QVector<NodeKeyframeTrackReference>(),
                       DraggableGizmo::DragValueBehavior behavior = DraggableGizmo::kDeltaFromStart) {
    T* gizmo = new T(this);                                      // 创建 Gizmo 实例，父对象为当前节点
    gizmo->SetDragValueBehavior(behavior);                       // 设置拖拽行为
    foreach (const NodeKeyframeTrackReference& input, inputs) {  // 遍历所有关联的输入
      gizmo->AddInput(input);                                    // 将输入添加到 Gizmo
    }
    // 连接 Gizmo 的信号到节点的槽函数
    connect(gizmo, &DraggableGizmo::HandleStart, this, &Node::GizmoDragStart);
    connect(gizmo, &DraggableGizmo::HandleMovement, this, &Node::GizmoDragMove);
    return gizmo;
  }

  /**
   * @brief 添加一个可拖拽的 Gizmo (通过输入ID列表)。
   * @tparam T Gizmo 的类型。
   * @param inputs 此 Gizmo 控制的输入参数的ID列表。
   * @param behavior Gizmo 拖拽时的值行为。
   * @return 返回创建的 Gizmo 指针。
   */
  template <typename T>
  T* AddDraggableGizmo(const QStringList& inputs,
                       DraggableGizmo::DragValueBehavior behavior = DraggableGizmo::kDeltaFromStart) {
    QVector<NodeKeyframeTrackReference> refs(inputs.size());  // 创建引用列表
    for (int i = 0; i < refs.size(); i++) {
      refs[i] = NodeKeyframeTrackReference(NodeInput(this, inputs[i]));  // 将输入ID转换为引用
    }
    return AddDraggableGizmo<T>(refs, behavior);  // 调用另一个重载版本
  }

 protected slots:
  // Gizmo 开始拖拽时的槽函数 (虚函数，子类可重写)
  virtual void GizmoDragStart(const olive::NodeValueRow& row, double x, double y, const olive::core::rational& time) {}

  // Gizmo 拖拽过程中的槽函数 (虚函数，子类可重写)
  virtual void GizmoDragMove(double x, double y, const Qt::KeyboardModifiers& modifiers) {}

 signals:  // Qt 信号声明
  /**
   * @brief 当 SetLabel() 被调用时发出的信号。
   * @param s 新的标签字符串。
   */
  void LabelChanged(const QString& s);

  // 节点颜色改变时发出的信号
  void ColorChanged();

  // 节点某个输入的值在特定时间范围改变时发出的信号
  void ValueChanged(const NodeInput& input, const TimeRange& range);

  // 节点的某个输入端口被连接时发出的信号
  void InputConnected(Node* output, const NodeInput& input);

  // 节点的某个输入端口断开连接时发出的信号
  void InputDisconnected(Node* output, const NodeInput& input);

  // 节点的某个输出端口被连接 (到下游节点的输入) 时发出的信号
  void OutputConnected(Node* output, const NodeInput& input);

  // 节点的某个输出端口断开连接时发出的信号
  void OutputDisconnected(Node* output, const NodeInput& input);

  // 某个输入的值提示 (ValueHint) 改变时发出的信号
  void InputValueHintChanged(const NodeInput& input);

  // 某个输入的属性改变时发出的信号
  void InputPropertyChanged(const QString& input, const QString& key, const QVariant& value);

  // 节点的链接状态改变时发出的信号
  void LinksChanged();

  // 数组类型输入的大小改变时发出的信号
  void InputArraySizeChanged(const QString& input, int old_size, int new_size);

  // 添加关键帧时发出的信号
  void KeyframeAdded(NodeKeyframe* key);

  // 移除关键帧时发出的信号
  void KeyframeRemoved(NodeKeyframe* key);

  // 关键帧时间改变时发出的信号
  void KeyframeTimeChanged(NodeKeyframe* key);

  // 关键帧类型改变时发出的信号
  void KeyframeTypeChanged(NodeKeyframe* key);

  // 关键帧值改变时发出的信号
  void KeyframeValueChanged(NodeKeyframe* key);

  // 关键帧启用状态改变时发出的信号 (针对整个输入)
  void KeyframeEnableChanged(const NodeInput& input, bool enabled);

  // 添加新输入端口时发出的信号
  void InputAdded(const QString& id);

  // 移除输入端口时发出的信号
  void InputRemoved(const QString& id);

  // 输入端口名称改变时发出的信号
  void InputNameChanged(const QString& id, const QString& name);

  // 输入端口数据类型改变时发出的信号
  void InputDataTypeChanged(const QString& id, NodeValue::Type type);

  // 节点被添加到图 (项目) 时发出的信号
  void AddedToGraph(Project* graph);

  // 节点从图 (项目) 中移除时发出的信号
  void RemovedFromGraph(Project* graph);

  // 节点被添加到上下文 (例如，父节点的内部图) 时发出的信号
  void NodeAddedToContext(Node* node);

  // 节点在上下文中的位置改变时发出的信号
  void NodePositionInContextChanged(Node* node, const QPointF& pos);

  // 节点从上下文中移除时发出的信号
  void NodeRemovedFromContext(Node* node);

  // 输入端口的标志位改变时发出的信号
  void InputFlagsChanged(const QString& input, const InputFlags& flags);

 private:
  // 内部结构体，用于存储每个输入的详细信息
  struct Input {
    NodeValue::Type type;                 // 输入的数据类型
    InputFlags flags;                     // 输入的标志位
    SplitValue default_value;             // 输入的默认值 (分离形式)
    QHash<QString, QVariant> properties;  // 输入的额外属性 (例如下拉框选项)
    QString human_name;                   // 用户可读的输入名称
    int array_size{};                     // 如果是数组类型，表示数组大小
  };

  // 为指定输入创建 NodeInputImmediate 对象 (用于管理未连接时的值和关键帧)
  NodeInputImmediate* CreateImmediate(const QString& input);

  // 获取指定输入ID在内部输入列表中的索引
  [[nodiscard]] int GetInternalInputIndex(const QString& input) const { return input_ids_.indexOf(input); }

  // 获取指定输入的内部数据结构 (Input 结构体)
  Input* GetInternalInputData(const QString& input) {
    int i = GetInternalInputIndex(input);

    if (i == -1) {
      return nullptr;  // 未找到
    } else {
      return &input_data_[i];  // 返回数据的可修改引用
    }
  }
  // const 版本
  [[nodiscard]] const Input* GetInternalInputData(const QString& input) const {
    int i = GetInternalInputIndex(input);

    if (i == -1) {
      return nullptr;  // 未找到
    } else {
      return &input_data_.at(i);  // 返回数据的常量引用
    }
  }

  // 报告无效输入的操作 (用于调试或错误处理)
  void ReportInvalidInput(const char* attempted_action, const QString& id, int element) const;

  // 内部静态辅助函数：复制节点及其依赖图，但不包括 Item 节点
  static Node* CopyNodeAndDependencyGraphMinusItemsInternal(QMap<Node*, Node*>& created, Node* node,
                                                            MultiUndoCommand* command);

  /**
   * @brief 获取输入数组的内部实际大小 (立即值对象的数量可能大于 ArraySize())。
   * @param input 输入端口ID。
   * @return 返回内部数组大小。
   */
  int GetInternalInputArraySize(const QString& input);

  /**
   * @brief 内部静态辅助函数：递归查找连接到指定输入端口的特定类型的上游节点。
   */
  template <class T>
  static void FindInputNodesConnectedToInputInternal(const NodeInput& input, QVector<T*>& list, int maximum);

  /**
   * @brief 内部静态辅助函数：递归查找从节点 n 开始的特定类型的上游节点。
   */
  template <class T>
  static void FindInputNodeInternal(const Node* n, QVector<T*>& list, int maximum);

  // 内部函数：获取依赖节点 (可配置是否遍历和是否仅独占依赖)
  [[nodiscard]] QVector<Node*> GetDependenciesInternal(bool traverse, bool exclusive_only) const;

  // 参数值改变时的内部处理函数
  void ParameterValueChanged(const QString& input, int element, const olive::core::TimeRange& range);
  // 重载版本
  void ParameterValueChanged(const NodeInput& input, const olive::core::TimeRange& range) {
    ParameterValueChanged(input.input(), input.element(), range);
  }

  /**
   * @brief 智能地确定一个关键帧影响的时间范围。
   * @param key 关键帧指针。
   * @return 返回受影响的时间范围。
   */
  TimeRange GetRangeAffectedByKeyframe(NodeKeyframe* key) const;

  /**
   * @brief 获取指定索引的关键帧与其前后关键帧之间的时间范围。
   * @param input 输入端口ID。
   * @param index 关键帧在其轨道上的索引。
   * @param track 轨道索引。
   * @param element 元素索引。
   * @return 返回时间范围。
   */
  [[nodiscard]] TimeRange GetRangeAroundIndex(const QString& input, int index, int track, int element) const;

  // 清除指定输入特定元素的所有数据 (例如，从数组中移除元素时)
  void ClearElement(const QString& input, int index);

  /**
   * @brief 节点的用户自定义标签。
   */
  QString label_;

  /**
   * @brief 覆盖颜色索引。-1 表示颜色基于分类，>=0 表示用户设置了自定义颜色。
   */
  int override_color_;

  /**
   * @brief 与此节点链接的其他节点列表。
   */
  QVector<Node*> links_;

  QVector<QString> input_ids_;  // 存储所有输入端口ID的列表 (有序)
  QVector<Input> input_data_;   // 存储每个输入端口详细信息的列表 (与 input_ids_ 对应)

  // 标准 (非数组) 输入的立即值对象映射 (输入ID -> NodeInputImmediate*)
  QMap<QString, NodeInputImmediate*> standard_immediates_;

  // 数组输入的立即值对象映射 (输入ID -> QVector<NodeInputImmediate*>)
  QMap<QString, QVector<NodeInputImmediate*> > array_immediates_;

  InputConnections input_connections_;  // 存储此节点的输入连接

  OutputConnections output_connections_;  // 存储从此节点出去的输出连接

  Folder* folder_;  // 此节点所属的文件夹指针，可能为 nullptr

  // 输入元素对到其值提示的映射
  QMap<InputElementPair, ValueHint> value_hints_;

  PositionMap context_positions_;  // 存储此节点作为上下文时，其内部节点的位置信息

  uint64_t flags_;  // 节点的标志位 (使用 Flag 枚举按位组合)

  QVector<NodeGizmo*> gizmos_;  // 此节点拥有的 Gizmo 列表

  QString effect_input_;  // 特殊输入端口的ID，标记为“效果输入”

  FrameHashCache* video_cache_;      // 视频帧缓存指针
  ThumbnailCache* thumbnail_cache_;  // 缩略图缓存指针

  AudioPlaybackCache* audio_cache_;     // 音频播放缓存指针
  AudioWaveformCache* waveform_cache_;  // 音频波形缓存指针

  bool caches_enabled_;  // 缓存是否启用标志

 private slots:  // Qt 私有槽函数
  /**
   * @brief 当关键帧时间改变时的槽函数，用于保持关键帧按时间正确排序并使缓存失效。
   */
  void InvalidateFromKeyframeTimeChange();

  /**
   * @brief 当关键帧值改变时的槽函数，用于通知缓存需要更新。
   */
  void InvalidateFromKeyframeValueChange();

  /**
   * @brief 当关键帧类型改变时的槽函数，用于通知缓存需要更新。
   */
  void InvalidateFromKeyframeTypeChanged();

  /**
   * @brief 当关键帧的贝塞尔入控制柄值改变时的槽函数，用于通知缓存需要更新。
   */
  void InvalidateFromKeyframeBezierInChange();

  /**
   * @brief 当关键帧的贝塞尔出控制柄值改变时的槽函数，用于通知缓存需要更新。
   */
  void InvalidateFromKeyframeBezierOutChange();
};

// 模板函数实现：递归查找连接到特定输入端口的特定类型的上游节点 (内部辅助)
template <class T>
void Node::FindInputNodesConnectedToInputInternal(const NodeInput& input, QVector<T*>& list, int maximum) {
  Node* edge = input.GetConnectedOutput();  // 获取连接到此输入的上游节点
  if (!edge) {                              // 如果没有连接，则返回
    return;
  }

  T* cast_test = dynamic_cast<T*>(edge);  // 尝试将上游节点动态转换为类型 T

  if (cast_test) {                                 // 如果转换成功
    list.append(cast_test);                        // 将其添加到列表中
    if (maximum != 0 && list.size() == maximum) {  // 如果达到了最大查找数量
      return;                                      // 则返回
    }
  }

  FindInputNodeInternal<T>(edge, list, maximum);  // 递归查找更上游的节点
}

// 模板函数实现：查找连接到特定输入端口的特定类型的上游节点 (公共接口)
template <class T>
QVector<T*> Node::FindInputNodesConnectedToInput(const NodeInput& input, int maximum) {
  QVector<T*> list;  // 创建结果列表

  FindInputNodesConnectedToInputInternal<T>(input, list, maximum);  // 调用内部辅助函数

  return list;  // 返回结果
}

// 模板函数实现：递归查找从节点 n 开始的特定类型的上游节点 (内部辅助)
template <class T>
void Node::FindInputNodeInternal(const Node* n, QVector<T*>& list, int maximum) {
  // 遍历节点 n 的所有输入连接
  for (const auto& input_connection : n->input_connections_) {
    // 对每个输入连接递归调用 FindInputNodesConnectedToInputInternal
    // input_connection.first 是 NodeInput 对象
    FindInputNodesConnectedToInputInternal(input_connection.first, list, maximum);
    if (maximum != 0 && list.size() == maximum) {  // 如果达到了最大查找数量
      return;                                      // 则返回
    }
  }
}

// 模板函数实现：查找此节点从其获取输入的特定类型的节点 (公共接口)
template <class T>
QVector<T*> Node::FindInputNodes(int maximum) const {
  QVector<T*> list;  // 创建结果列表

  FindInputNodeInternal<T>(this, list, maximum);  // 从当前节点开始查找

  return list;  // 返回结果
}

}  // namespace olive

// 声明 Node::ValueHint 类型为元类型，以便在 QVariant 中使用
Q_DECLARE_METATYPE(olive::Node::ValueHint)

#endif  // NODE_H