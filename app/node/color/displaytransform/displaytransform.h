#ifndef DISPLAYTRANSFORMNODE_H
#define DISPLAYTRANSFORMNODE_H

#include "node/color/ociobase/ociobase.h"  // 引入基类 OCIOBaseNode 的定义
#include "render/colorprocessor.h"         // 引入 ColorProcessor 的定义，用于实际执行颜色变换

namespace olive {

/**
 * @brief 代表显示变换的节点。
 * 该节点使用 OpenColorIO (OCIO) 将图像从工作色彩空间转换为指定的显示设备和视图设置。
 * 例如，从场景线性的 ACEScg 转换到 sRGB 显示器的 Rec.709 视图。
 */
class DisplayTransformNode : public OCIOBaseNode {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief DisplayTransformNode 构造函数。
      */
     DisplayTransformNode();

  NODE_DEFAULT_FUNCTIONS(DisplayTransformNode)  // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此显示变换节点的名称。
   * @return QString 类型的节点名称 (例如 "显示变换")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此显示变换节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，用于组织和归类节点 (例如，属于 "色彩" 或 "OCIO" 分类)。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此显示变换节点的描述信息。
   * @return QString 类型的节点描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本。
   */
  void Retranslate() override;
  /**
   * @brief 当节点的某个输入参数值发生变化时调用的事件处理函数。
   *  例如，当用户更改了目标显示设备或视图时。
   * @param input 值发生变化的输入端口/参数名称。
   * @param element 相关元素的索引。
   */
  void InputValueChangedEvent(const QString &input, int element) override;

  /**
   * @brief 获取当前选择的显示设备名称。
   * @return QString 类型的显示设备名称。
   */
  [[nodiscard]] QString GetDisplay() const;
  /**
   * @brief 获取当前选择的视图名称。
   * @return QString 类型的视图名称。
   */
  [[nodiscard]] QString GetView() const;
  /**
   * @brief 获取颜色变换的方向。
   * @return ColorProcessor::Direction 枚举值，表示是正向变换 (例如场景到显示) 还是反向变换。
   */
  [[nodiscard]] ColorProcessor::Direction GetDirection() const;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kDisplayInput;    ///< "Display" - 选择显示设备的输入参数键名。
  static const QString kViewInput;       ///< "View" - 选择视图的输入参数键名。
  static const QString kDirectionInput;  ///< "Direction" - 选择变换方向的输入参数键名。

 protected slots:
  /**
   * @brief 当 OCIO 配置发生变化时调用的槽函数。
   *  需要重新生成颜色处理器并更新可用的显示和视图列表。
   */
  void ConfigChanged() override;

 private:
  /**
   * @brief 根据当前的显示、视图和方向设置，生成或更新内部的颜色处理器 (ColorProcessor)。
   */
  void GenerateProcessor();

  /**
   * @brief 更新节点参数中可用的显示设备列表。
   *  通常在 OCIO 配置更改后调用。
   */
  void UpdateDisplays();

  /**
   * @brief 根据当前选择的显示设备，更新节点参数中可用的视图列表。
   *  通常在 OCIO 配置更改或所选显示设备更改后调用。
   */
  void UpdateViews();
};

}  // namespace olive

#endif  // DISPLAYTRANSFORMNODE_H