#ifndef KEYFRAMEVIEWINPUTCONNECTION_H
#define KEYFRAMEVIEWINPUTCONNECTION_H

#include <QObject> // Qt 对象模型基类，用于支持信号和槽

#include "node/node.h"   // 节点基类定义 (间接通过 NodeKeyframeTrackReference 使用)
#include "node/param.h"  // 节点参数相关定义 (包含 NodeKeyframeTrackReference)

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QBrush;
// class QVector;
// class NodeKeyframe; // 已在 node/keyframe.h 中定义，而 node/param.h 可能已包含 node/keyframe.h

namespace olive {

class KeyframeView; // 前向声明 KeyframeView 类，因为它是此类的父对象类型

/**
 * @brief KeyframeViewInputConnection 类代表关键帧视图 (KeyframeView) 中一个输入轨道（参数的关键帧序列）的连接。
 *
 * 这个类封装了单个关键帧轨道（例如，一个节点参数的X分量、Y分量或单个浮点数参数）
 * 在 KeyframeView 中的显示属性和行为。它持有对实际关键帧数据的引用 (NodeKeyframeTrackReference)，
 * 并与 KeyframeView 交互以更新显示并响应数据变化。
 */
class KeyframeViewInputConnection : public QObject {
  Q_OBJECT // Qt 元对象系统宏，用于支持信号和槽机制

 public:
  /**
   * @brief 构造函数。
   * @param input 对节点关键帧轨道的引用 (NodeKeyframeTrackReference)，指定了要连接的具体参数轨道。
   * @param parent 指向父对象 KeyframeView 的指针。
   */
  explicit KeyframeViewInputConnection(const NodeKeyframeTrackReference &input, KeyframeView *parent);

  /**
   * @brief 获取此轨道在视图中绘制关键帧的 Y 坐标。
   * @return 返回 Y 坐标值 (int 类型)。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const int &GetKeyframeY() const { return y_; }

  /**
   * @brief 设置此轨道在视图中绘制关键帧的 Y 坐标。
   * @param y 要设置的 Y 坐标值。
   */
  void SetKeyframeY(int y);

  /**
   * @brief 定义关键帧在 Y 轴上的行为/布局方式。
   */
  enum YBehavior {
    kSingleRow,     ///< 所有关键帧绘制在由 GetKeyframeY() 指定的单行上。
    kValueIsHeight  ///< 关键帧的 Y 值直接决定其在视图中的垂直位置（用于曲线显示）。
  };

  /**
   * @brief 设置此轨道关键帧在 Y 轴上的行为。
   * @param e YBehavior 枚举值。
   */
  void SetYBehavior(YBehavior e);

  /**
   * @brief 获取此连接所引用的实际关键帧数据。
   * @return 返回一个 const 引用，指向包含 NodeKeyframe 指针的 QVector。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const QVector<NodeKeyframe *> &GetKeyframes() const {
    // 通过 NodeKeyframeTrackReference 访问节点，然后获取特定输入参数的特定轨道上的关键帧
    return input_.input().node()->GetKeyframeTracks(input_.input()).at(input_.track());
  }

  /**
   * @brief 获取用于绘制此轨道关键帧或曲线的画刷。
   * @return 返回一个 const 引用，指向 QBrush 对象。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const QBrush &GetBrush() const { return brush_; }

  /**
   * @brief 获取此连接所代表的节点关键帧轨道的引用。
   * @return 返回一个 const 引用，指向 NodeKeyframeTrackReference 对象。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const NodeKeyframeTrackReference &GetReference() const { return input_; }

  /**
   * @brief 设置用于绘制此轨道关键帧或曲线的画刷。
   * @param brush 要设置的 QBrush 对象。
   */
  void SetBrush(const QBrush &brush);

 signals:
  /**
   * @brief 当此轨道需要更新其在 KeyframeView 中的显示时发出此信号。
   */
  void RequireUpdate();

  /**
   * @brief 当此轨道中关键帧的类型（例如线性、贝塞尔、保持）发生改变时发出此信号。
   */
  void TypeChanged();

 private:
  KeyframeView *keyframe_view_; ///< 指向父 KeyframeView 对象的指针。

  NodeKeyframeTrackReference input_; ///< 对实际节点关键帧轨道的引用，包含了节点、输入参数和轨道索引信息。

  int y_; ///< 此轨道在 KeyframeView 中绘制时的基准 Y 坐标（对于 kSingleRow 行为）。

  YBehavior y_behavior_; ///< 定义关键帧在 Y 轴上的显示行为。

  QBrush brush_; ///< 用于绘制此轨道相关的图形元素（如关键帧、曲线）的画刷。

 private slots:
  /**
   * @brief 当底层数据模型中向此轨道添加新关键帧时调用的槽函数。
   * @param key 指向新添加的 NodeKeyframe 对象的指针。
   */
  void AddKeyframe(NodeKeyframe *key);

  /**
   * @brief 当底层数据模型中从此轨道移除关键帧时调用的槽函数。
   * @param key 指向被移除的 NodeKeyframe 对象的指针。
   */
  void RemoveKeyframe(NodeKeyframe *key);

  /**
   * @brief 当此轨道中的某个关键帧的属性（如时间、值）发生改变时调用的槽函数。
   * @param key 指向已改变的 NodeKeyframe 对象的指针。
   */
  void KeyframeChanged(NodeKeyframe *key);

  /**
   * @brief 当此轨道中的某个关键帧的插值类型发生改变时调用的槽函数。
   * @param key 指向其类型已改变的 NodeKeyframe 对象的指针。
   */
  void KeyframeTypeChanged(NodeKeyframe *key);
};

}  // namespace olive

#endif  // KEYFRAMEVIEWINPUTCONNECTION_H
