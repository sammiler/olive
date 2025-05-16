#ifndef NODEGIZMO_H  // 防止头文件被多次包含的宏定义开始
#define NODEGIZMO_H

#include <QObject>   // Qt 对象模型基类
#include <QPainter>  // Qt 绘图类，用于绘制 Gizmo

#include "node/globals.h"  // 引入 NodeGlobals 定义，Gizmo 绘制和行为可能需要全局信息

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 代表节点 Gizmo (图形交互控件) 的抽象基类。
 * Gizmo 是用户在视图（如监视器窗口）中与节点参数进行交互的视觉元素，
 * 例如拖动点、线、框来改变位置、大小、形状等。
 * 此基类提供了 Gizmo 的基本属性（如可见性）和绘制接口。
 */
class NodeGizmo : public QObject {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief NodeGizmo 构造函数。
      * @param parent 父 QObject 对象，默认为 nullptr。
      */
     explicit NodeGizmo(QObject *parent = nullptr);
  /**
   * @brief NodeGizmo 析构函数。
   */
  ~NodeGizmo() override;

  /**
   * @brief 纯虚函数 (在此基类中为空实现)，用于绘制 Gizmo。
   *  派生类必须重写此方法以实现其特定的绘制逻辑。
   * @param p 指向 QPainter 对象的指针，用于进行绘图操作。
   */
  virtual void Draw(QPainter *p) const {}

  /**
   * @brief 获取与此 Gizmo 关联的全局节点参数。
   *  这些全局参数可能包含画布缩放、偏移、当前时间等信息，影响 Gizmo 的显示和行为。
   * @return const NodeGlobals& 对 NodeGlobals 对象的常量引用。
   */
  [[nodiscard]] const NodeGlobals &GetGlobals() const { return globals_; }
  /**
   * @brief 设置与此 Gizmo 关联的全局节点参数。
   * @param globals NodeGlobals 对象。
   */
  void SetGlobals(const NodeGlobals &globals) { globals_ = globals; }

  /**
   * @brief 检查此 Gizmo 当前是否可见。
   * @return 如果 Gizmo 可见则返回 true，否则返回 false。
   */
  [[nodiscard]] bool IsVisible() const { return visible_; }
  /**
   * @brief 设置此 Gizmo 的可见性。
   * @param e true 表示可见，false 表示隐藏。
   */
  void SetVisible(bool e) { visible_ = e; }

 signals:  // Qt 信号声明区域 (此基类中未定义具体信号)

 private:
  NodeGlobals globals_;  ///< 存储与 Gizmo 相关的全局节点参数。

  bool visible_;  ///< 标记此 Gizmo 是否可见。
};

}  // namespace olive

#endif  // NODEGIZMO_H // 头文件宏定义结束