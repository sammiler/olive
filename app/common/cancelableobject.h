

#ifndef CANCELABLEOBJECT_H
#define CANCELABLEOBJECT_H

#include "common/define.h"     // 可能包含一些通用定义
#include "render/cancelatom.h" // 包含 CancelAtom 类，用于原子性地标记取消状态

namespace olive {

/**
 * @brief 提供可取消操作功能的基类。
 *
 * 任何需要支持取消操作的类都可以继承自此类。
 * 它内部使用一个 CancelAtom 来管理取消状态，并提供了一个虚函数
 * `CancelEvent()` 供派生类在取消发生时执行特定操作。
 */
class CancelableObject {
 public:
  /**
   * @brief 默认构造函数。
   * 初始化一个 CancelableObject 实例，其取消状态默认为未取消。
   */
  CancelableObject() = default;

  /**
   * @brief 默认的虚析构函数。
   * 允许派生类被正确销毁。
   */
  virtual ~CancelableObject() = default;

  /**
   * @brief 请求取消当前对象正在进行的操作。
   * 此方法会设置内部的 CancelAtom 为取消状态，并调用 CancelEvent() 虚函数。
   */
  void Cancel() {
    cancel_.Cancel(); // 设置原子取消标记
    CancelEvent();    // 调用虚方法，允许派生类响应取消事件
  }

  /**
   * @brief 获取指向内部 CancelAtom 对象的指针。
   * 允许其他组件直接查询或（在某些情况下，但不推荐）修改取消状态。
   * @return CancelAtom* 指向内部 CancelAtom 对象的指针。
   */
  CancelAtom *GetCancelAtom() { return &cancel_; }

  /**
   * @brief 检查当前对象的操作是否已被请求取消。
   * @return bool 如果已被取消则返回 true，否则返回 false。
   */
  bool IsCancelled() { return cancel_.IsCancelled(); }

 protected:
  /**
   * @brief 取消事件的虚处理函数。
   * 派生类可以覆盖此方法以在 Cancel() 被调用时执行特定的清理工作或状态变更。
   * 默认实现为空。
   */
  virtual void CancelEvent() {}

 private:
  /**
   * @brief 用于原子性地管理取消状态的对象。
   */
  CancelAtom cancel_;
};

}  // namespace olive

#endif  // CANCELABLEOBJECT_H