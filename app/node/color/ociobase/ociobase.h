#ifndef OCIOBASENODE_H
#define OCIOBASENODE_H

#include <utility>  // 引入 std::move 等工具

#include "node/node.h"                     // 引入基类 Node 的定义
#include "render/job/colortransformjob.h"  // 引入 ColorTransformJob，可能与颜色处理相关

// 前向声明 ColorManager 和 ColorProcessor，以及 ColorProcessorPtr 类型别名
// 这些通常在其他头文件中定义
namespace olive {
class ColorManager;
class ColorProcessor;                                       // 假设 ColorProcessor 类的声明
using ColorProcessorPtr = std::shared_ptr<ColorProcessor>;  // 假设 ColorProcessorPtr 是 ColorProcessor 的智能指针
}  // namespace olive

namespace olive {

/**
 * @brief 所有基于 OpenColorIO (OCIO) 的颜色操作节点的基类。
 * 提供了与 ColorManager 交互以及管理 ColorProcessor 的通用功能。
 */
class OCIOBaseNode : public Node {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief OCIOBaseNode 构造函数。
      */
     OCIOBaseNode();

  /**
   * @brief 当此节点被添加到项目图中时调用的事件处理函数。
   *  用于进行初始化，例如连接到 ColorManager 的信号。
   * @param p 指向当前项目的指针。
   */
  void AddedToGraphEvent(Project *p) override;
  /**
   * @brief 当此节点从项目图中移除时调用的事件处理函数。
   *  用于进行清理，例如断开与 ColorManager 的信号连接。
   * @param p 指向当前项目的指针。
   */
  void RemovedFromGraphEvent(Project *p) override;

  /**
   * @brief 计算并输出节点在特定时间点的值（通常是经过颜色变换后的图像数据）。
   *  这个方法会使用内部的 ColorProcessor 来处理输入的图像。
   * @param value 当前输入行数据 (通常包含输入的图像/纹理)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (通常包含输出的图像/纹理)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  static const QString kTextureInput;  ///< "TextureIn" - 默认的纹理输入端口的键名。

 protected slots:
  /**
   * @brief 纯虚槽函数，当 OCIO 配置发生变化时由 ColorManager 发射信号并调用此函数。
   *  派生类必须实现此方法以响应配置更改，例如重新生成其 ColorProcessor。
   */
  virtual void ConfigChanged() = 0;

 protected:
  /**
   * @brief 获取指向 ColorManager 实例的指针。
   * @return ColorManager* 指向色彩管理器的指针。
   */
  [[nodiscard]] ColorManager *manager() const { return manager_; }

  /**
   * @brief 获取当前节点使用的颜色处理器 (ColorProcessor) 的智能指针。
   * @return ColorProcessorPtr 智能指针，指向实际执行颜色变换的对象。
   */
  [[nodiscard]] ColorProcessorPtr processor() const { return processor_; }
  /**
   * @brief 设置当前节点使用的颜色处理器。
   * @param p 一个 ColorProcessorPtr 智能指针，指向新的颜色处理器。
   */
  void set_processor(ColorProcessorPtr p) { processor_ = std::move(p); }

 private:
  ColorManager *manager_;  ///< 指向项目中 ColorManager 实例的指针。通过它获取 OCIO 配置等信息。

  ColorProcessorPtr processor_;  ///< 智能指针，持有实际执行颜色变换的 ColorProcessor 对象。
};

}  // namespace olive

#endif  // OCIOBASENODE_H