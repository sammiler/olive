#ifndef TEXTGIZMO_H  // 防止头文件被多次包含的宏定义开始
#define TEXTGIZMO_H

#include "gizmo.h"       // 引入基类 NodeGizmo (或类似 Gizmo 基类) 的定义
#include "node/param.h"  // 引入节点参数相关的定义，如 NodeKeyframeTrackReference

// 可能需要的前向声明
// namespace olive {
// class rational; // 假设
// }
// Qt::Alignment 可能需要 <Qt> 或 <QFlags> 等头文件

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 代表一个文本 Gizmo (图形交互控件)。
 * 该 Gizmo 用于在视图中直接编辑文本内容、调整文本框大小以及可能的对齐方式。
 * 它通常与文本生成器节点 (如 TextGeneratorV3) 关联。
 */
class TextGizmo : public NodeGizmo {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief TextGizmo 构造函数。
      * @param parent 父 QObject 对象，默认为 nullptr。
      */
     explicit TextGizmo(QObject *parent = nullptr);

  /**
   * @brief 获取文本框的矩形区域。
   * @return const QRectF& 对 QRectF 对象的常量引用，表示文本框的位置和大小。
   */
  [[nodiscard]] const QRectF &GetRect() const { return rect_; }
  /**
   * @brief 设置文本框的矩形区域。
   *  当设置新的矩形时，可能会发射 RectChanged 信号。
   * @param r QRectF 对象。
   */
  void SetRect(const QRectF &r);

  /**
   * @brief 获取当前 Gizmo 中显示的文本内容（可能为 HTML 格式）。
   * @return const QString& 对文本字符串的常量引用。
   */
  [[nodiscard]] const QString &GetHtml() const { return text_; }
  /**
   * @brief 设置 Gizmo 中显示的文本内容（可能为 HTML 格式）。
   * @param t 新的文本字符串。
   */
  void SetHtml(const QString &t) { text_ = t; }

  /**
   * @brief 设置此 Gizmo 关联的节点输入参数（通常是文本内容参数）。
   * @param input 对节点关键帧轨道（输入参数）的引用。
   */
  void SetInput(const NodeKeyframeTrackReference &input) { input_ = input; }

  /**
   * @brief 更新关联的节点输入参数中的 HTML 文本内容，并在指定时间点创建关键帧。
   * @param s 新的 HTML 文本内容。
   * @param time 当前时间点，用于关键帧记录。
   */
  void UpdateInputHtml(const QString &s, const rational &time);

  /**
   * @brief 获取文本的垂直对齐方式。
   * @return Qt::Alignment Qt 的对齐标志 (例如 Qt::AlignTop, Qt::AlignVCenter, Qt::AlignBottom)。
   */
  [[nodiscard]] Qt::Alignment GetVerticalAlignment() const { return valign_; }
  /**
   * @brief 设置文本的垂直对齐方式。
   *  当设置新的对齐方式时，可能会发射 VerticalAlignmentChanged 信号。
   * @param va 新的 Qt::Alignment 对齐标志。
   */
  void SetVerticalAlignment(Qt::Alignment va);

 signals:  // Qt 信号声明区域
  /**
   * @brief 当 Gizmo 被激活（例如，用户开始编辑文本）时发射此信号。
   */
  void Activated();
  /**
   * @brief 当 Gizmo 被取消激活（例如，用户完成编辑文本）时发射此信号。
   */
  void Deactivated();
  /**
   * @brief 当文本的垂直对齐方式通过 Gizmo 更改时发射此信号。
   * @param va 新的 Qt::Alignment 对齐标志。
   */
  void VerticalAlignmentChanged(Qt::Alignment va);
  /**
   * @brief 当文本框的矩形区域通过 Gizmo 更改时发射此信号。
   * @param r 新的 QRectF 对象。
   */
  void RectChanged(const QRectF &r);

 private:
  QRectF rect_;  ///< 存储文本框的位置和大小。

  QString text_;  ///< 存储当前 Gizmo 中显示的文本内容（可能为 HTML）。

  NodeKeyframeTrackReference input_;  ///< 引用此 Gizmo 所关联的节点文本内容输入参数。

  Qt::Alignment valign_;  ///< 存储文本的垂直对齐方式。
};

}  // namespace olive

#endif  // TEXTGIZMO_H // 头文件宏定义结束