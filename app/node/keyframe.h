#ifndef NODEKEYFRAME_H  // 防止头文件被多次包含的宏定义开始
#define NODEKEYFRAME_H

#include <QPointF>   // Qt 浮点数点类，用于贝塞尔控制点
#include <QVariant>  // Qt 通用数据类型，用于存储关键帧的值
#include <memory>    // C++ 智能指针库 (在此文件中似乎未直接使用，但包含无害)

#include "node/param.h"  // 引入 NodeInput 和 NodeKeyframeTrackReference 定义

// 可能需要的前向声明
// class Node; // Node 在此被前向声明
// class rational; // 假设
// class QXmlStreamReader; // 假设
// class QXmlStreamWriter; // 假设
// namespace NodeValue { enum class Type; } // 假设

// Qt 元类型声明，需要在命名空间之外
// Q_DECLARE_METATYPE(olive::NodeKeyframe::Type)

namespace olive {  // Olive 编辑器的命名空间

class Node;  // 前向声明 Node 类，关键帧属于某个节点的某个参数

/**
 * @brief 代表一个关键帧，即在特定时间点的一个数据点，用于与其他数据点进行插值形成动画。
 * 关键帧包含时间、值、插值类型（线性、保持、贝塞尔）以及可能的贝塞尔控制点信息。
 * 它还知道自己属于哪个节点的哪个输入参数的哪个通道/元素。
 */
class NodeKeyframe : public QObject {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief 定义关键帧之间的插值方法。
      */
     enum Type {
       kInvalid = -1,  ///< 无效类型
       kLinear,        ///< 线性插值 (两关键帧之间值呈直线变化)
       kHold,          ///< 保持插值 (值保持不变直到下一个关键帧)
       kBezier         ///< 贝塞尔曲线插值 (通过控制点定义平滑曲线变化)
     };

  /**
   * @brief 定义贝塞尔关键帧的两种控制柄类型。
   */
  enum BezierType {
    kInHandle,  ///< 入控制柄 (控制曲线进入此关键帧的形状)
    kOutHandle  ///< 出控制柄 (控制曲线离开此关键帧的形状)
  };

  static const Type kDefaultType;  ///< 默认的关键帧插值类型 (例如 kLinear)。

  /**
   * @brief NodeKeyframe 构造函数。
   * @param time 此关键帧的时间点。
   * @param value 此关键帧的值。
   * @param type 此关键帧的插值类型。
   * @param track 此关键帧所属的参数通道/轨道索引 (例如，对于颜色参数，0=R, 1=G, 2=B, 3=A)。
   * @param element 此关键帧所属的节点输入参数的元素索引 (如果参数是数组)。
   * @param input 此关键帧所属的节点输入参数的名称。
   * @param parent (可选) 父 QObject 对象。
   */
  NodeKeyframe(const rational& time, QVariant value, Type type, int track, int element, QString input,
               QObject* parent = nullptr);
  /**
   * @brief NodeKeyframe 默认构造函数。
   *  创建一个无效或未初始化的关键帧。
   */
  NodeKeyframe();

  /**
   * @brief NodeKeyframe 析构函数。
   */
  ~NodeKeyframe() override;

  /**
   * @brief 创建此关键帧的一个副本。
   * @param element 新副本的元素索引 (如果需要更改)。
   * @param parent (可选) 新副本的父 QObject 对象。
   * @return NodeKeyframe* 指向新创建的副本的指针。调用者负责管理其生命周期。
   */
  NodeKeyframe* copy(int element, QObject* parent = nullptr) const;
  /**
   * @brief 创建此关键帧的一个副本 (使用原始的元素索引)。
   * @param parent (可选) 新副本的父 QObject 对象。
   * @return NodeKeyframe* 指向新创建的副本的指针。
   */
  NodeKeyframe* copy(QObject* parent = nullptr) const;

  /**
   * @brief 获取此关键帧所属的父节点。
   *  通过 QObject::parent() 并进行类型转换为 Node* 实现。
   * @return Node* 指向父节点的指针，如果父对象不是 Node 类型或不存在则为 nullptr。
   */
  [[nodiscard]] Node* parent() const;

  /** @brief 获取此关键帧所属的输入参数的名称。 */
  [[nodiscard]] const QString& input() const { return input_; }
  /** @brief 设置此关键帧所属的输入参数的名称。 */
  void set_input(const QString& input) { input_ = input; }

  /**
   * @brief 获取一个引用，指向此关键帧所属的特定参数轨道。
   * @return NodeKeyframeTrackReference 轨道引用对象。
   */
  [[nodiscard]] NodeKeyframeTrackReference key_track_ref() const {
    return NodeKeyframeTrackReference(NodeInput(parent(), input(), element()), track());
  }

  /** @brief 获取此关键帧设定的时间点。 */
  [[nodiscard]] const rational& time() const;
  /** @brief 设置此关键帧的时间点。会发射 TimeChanged 信号。 */
  void set_time(const rational& time);

  /** @brief 获取此关键帧的值。 */
  [[nodiscard]] const QVariant& value() const;
  /** @brief 设置此关键帧的值。会发射 ValueChanged 信号。 */
  void set_value(const QVariant& value);

  /** @brief 获取此关键帧的插值类型。 */
  [[nodiscard]] const Type& type() const;
  /** @brief 设置此关键帧的插值类型。会发射 TypeChanged 信号，并可能调整贝塞尔控制点。 */
  void set_type(const Type& type);
  /**
   * @brief 设置此关键帧的插值类型，但不自动调整贝塞尔控制点。
   *  用于内部或特定情况，当贝塞尔控制点需要手动管理时。
   * @param type 新的插值类型。
   */
  void set_type_no_bezier_adj(const Type& type);

  /** @brief 获取用于贝塞尔插值的入控制点坐标。 */
  [[nodiscard]] const QPointF& bezier_control_in() const;
  /** @brief 设置用于贝塞尔插值的入控制点坐标。会发射 BezierControlInChanged 信号。 */
  void set_bezier_control_in(const QPointF& control);

  /** @brief 获取用于贝塞尔插值的出控制点坐标。 */
  [[nodiscard]] const QPointF& bezier_control_out() const;
  /** @brief 设置用于贝塞尔插值的出控制点坐标。会发射 BezierControlOutChanged 信号。 */
  void set_bezier_control_out(const QPointF& control);

  /**
   * @brief 返回一个确保有效的、应用于实际动画的贝塞尔入控制点。
   * 用户可以随意移动贝塞尔控制点，但内部使用时需要限制它们，以防止动画曲线重叠
   * （即，对于贝塞尔曲线上的任意X值，只能有一个Y值）。此方法返回一个已知“良好”的值。
   * @return QPointF 有效的贝塞尔入控制点坐标。
   */
  [[nodiscard]] QPointF valid_bezier_control_in() const;
  /**
   * @brief 返回一个确保有效的、应用于实际动画的贝塞尔出控制点。
   * @return QPointF 有效的贝塞尔出控制点坐标。
   */
  [[nodiscard]] QPointF valid_bezier_control_out() const;

  /**
   * @brief (便捷函数) 根据 BezierType 获取对应的贝塞尔控制点坐标。
   * @param type 控制柄类型 (kInHandle 或 kOutHandle)。
   * @return const QPointF& 控制点坐标的常量引用。
   */
  [[nodiscard]] const QPointF& bezier_control(BezierType type) const;
  /**
   * @brief (便捷函数) 根据 BezierType 设置对应的贝塞尔控制点坐标。
   * @param type 控制柄类型。
   * @param control 新的控制点坐标。
   */
  void set_bezier_control(BezierType type, const QPointF& control);

  /**
   * @brief 获取此关键帧所属的参数通道/轨道的索引。
   *  对于大多数单值参数，此值为0。对于多通道参数（如二维向量），X通道为0，Y通道为1，依此类推。
   * @return int 通道/轨道索引。
   */
  [[nodiscard]] int track() const { return track_; }
  /** @brief 设置此关键帧所属的参数通道/轨道索引。 */
  void set_track(int t) { track_ = t; }

  /** @brief 获取此关键帧所属的节点输入参数的元素索引 (如果参数是数组)。 */
  [[nodiscard]] int element() const { return element_; }
  /** @brief 设置此关键帧所属的节点输入参数的元素索引。 */
  void set_element(int e) { element_ = e; }

  /**
   * @brief (静态工具函数) 获取与给定贝塞尔控制柄类型相反的类型。
   *  例如，输入 kInHandle 返回 kOutHandle，反之亦然。
   * @param type 原始贝塞尔控制柄类型。
   * @return BezierType 相反的贝塞尔控制柄类型。
   */
  static BezierType get_opposing_bezier_type(BezierType type);

  /** @brief 获取此关键帧在同一参数轨道上的前一个关键帧。 */
  [[nodiscard]] NodeKeyframe* previous() const { return previous_; }
  /** @brief 设置此关键帧在同一参数轨道上的前一个关键帧。 */
  void set_previous(NodeKeyframe* keyframe) { previous_ = keyframe; }

  /** @brief 获取此关键帧在同一参数轨道上的后一个关键帧。 */
  [[nodiscard]] NodeKeyframe* next() const { return next_; }
  /** @brief 设置此关键帧在同一参数轨道上的后一个关键帧。 */
  void set_next(NodeKeyframe* keyframe) { next_ = keyframe; }

  /**
   * @brief 检查在给定的时间点，此关键帧是否有“兄弟”关键帧。
   *  “兄弟”关键帧是指属于同一节点、同一输入参数、同一时间点，但属于不同通道/轨道 (track_) 的关键帧。
   * @param t 要检查的时间点。
   * @return 如果存在兄弟关键帧则返回 true，否则返回 false。
   */
  [[nodiscard]] bool has_sibling_at_time(const rational& t) const;

  /**
   * @brief 从 XML 流加载关键帧数据。
   * @param reader 指向 QXmlStreamReader 的指针。
   * @param data_type 此关键帧期望的数据类型 (NodeValue::Type)，用于正确解析 QVariant 中的值。
   * @return 如果加载成功则返回 true，否则返回 false。
   */
  bool load(QXmlStreamReader* reader, NodeValue::Type data_type);
  /**
   * @brief 将关键帧数据保存到 XML 流。
   * @param writer 指向 QXmlStreamWriter 的指针。
   * @param data_type 此关键帧的数据类型。
   */
  void save(QXmlStreamWriter* writer, NodeValue::Type data_type) const;

 signals:  // Qt 信号声明区域
  /**
   * @brief 当此关键帧的时间点发生改变时发射此信号。
   * @param time 新的时间点。
   */
  void TimeChanged(const rational& time);

  /**
   * @brief 当此关键帧的值发生改变时发射此信号。
   * @param value 新的值。
   */
  void ValueChanged(const QVariant& value);

  /**
   * @brief 当此关键帧的插值类型发生改变时发射此信号。
   * @param type 新的插值类型。
   */
  void TypeChanged(const Type& type);

  /**
   * @brief 当此关键帧的贝塞尔入控制点发生改变时发射此信号。
   * @param d 新的入控制点坐标。
   */
  void BezierControlInChanged(const QPointF& d);

  /**
   * @brief 当此关键帧的贝塞尔出控制点发生改变时发射此信号。
   * @param d 新的出控制点坐标。
   */
  void BezierControlOutChanged(const QPointF& d);

 private:
  rational time_;               ///< 关键帧的时间点。
  QVariant value_;              ///< 关键帧的值。
  Type type_;                   ///< 关键帧的插值类型。
  QPointF bezier_control_in_;   ///< 贝塞尔入控制点。
  QPointF bezier_control_out_;  ///< 贝塞尔出控制点。
  QString input_;               ///< 所属输入参数的名称。
  int track_{};                 ///< 所属参数通道/轨道的索引。
  int element_{};               ///< 所属节点输入参数的元素索引。

  NodeKeyframe* previous_{};  ///< 指向同一轨道上前一个关键帧的指针 (形成链表)。
  NodeKeyframe* next_{};      ///< 指向同一轨道上后一个关键帧的指针。
};

// 类型别名，表示一个关键帧轨道，即存储特定参数通道/分量的所有关键帧指针的向量。
// 关键帧在轨道内通常按时间排序。
using NodeKeyframeTrack = QVector<NodeKeyframe*>;  ///< 关键帧轨道类型别名。

}  // namespace olive

// 声明 NodeKeyframe::Type 枚举为元类型，以便可以在 QVariant 中存储和在信号槽机制中使用。
Q_DECLARE_METATYPE(olive::NodeKeyframe::Type)

#endif  // NODEKEYFRAME_H // 头文件宏定义结束