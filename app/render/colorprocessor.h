#ifndef COLORPROCESSOR_H // 防止头文件被重复包含的宏
#define COLORPROCESSOR_H // 定义 COLORPROCESSOR_H 宏

#include "codec/frame.h"         // 包含 Frame (或 FramePtr) 相关的定义
#include "common/ocioutils.h"    // 包含 OpenColorIO (OCIO) 工具类或相关定义的头文件
                                 // (可能包含 OCIO::ConstProcessorRcPtr, OCIO::ConstCPUProcessorRcPtr 等)
#include "render/colortransform.h" // 包含 ColorTransform 结构体或类的定义

// 假设 Color (olive::core::Color) 和 std::shared_ptr 已通过其他方式被间接包含。

namespace olive { // olive 项目的命名空间

class ColorManager; // 向前声明 ColorManager 类 (色彩配置管理器)

class ColorProcessor; // 向前声明 ColorProcessor 类自身
// 类型别名：ColorProcessorPtr 是一个指向 ColorProcessor 对象的共享指针 (std::shared_ptr)
using ColorProcessorPtr = std::shared_ptr<ColorProcessor>;

/**
 * @brief ColorProcessor 类封装了一个 OpenColorIO (OCIO) 处理器，用于执行颜色转换。
 *
 * 它通常由 ColorManager 根据输入的色彩空间、输出的色彩空间以及可能应用的视图变换或外观 (look) 来创建。
 * 一旦创建，ColorProcessor 可以用于转换单个颜色值 (ConvertColor) 或整个图像帧 (ConvertFrame)。
 *
 * 这个类是 Olive 色彩管理系统的核心组件之一，确保在不同阶段的图像处理中颜色能够被正确和一致地转换。
 * 它底层依赖于 OCIO 库来实现复杂的颜色科学运算。
 *
 * 内部持有 OCIO::ConstProcessorRcPtr (用于GPU处理) 和 OCIO::ConstCPUProcessorRcPtr (用于CPU处理)。
 */
class ColorProcessor {
 public:
  // 定义颜色转换的方向枚举
  enum Direction {
    kNormal,  // 正向转换 (例如，从输入空间到处理空间，或从处理空间到显示空间)
    kInverse  // 反向转换
  };

  /**
   * @brief 构造函数，通过 ColorManager 和 ColorTransform 配置来创建一个颜色处理器。
   * @param config 指向 ColorManager 的指针，用于获取OCIO配置。
   * @param input 输入色彩空间的名称。
   * @param transform 描述具体颜色转换操作 (如视图变换、外观等) 的 ColorTransform 对象。
   * @param direction (可选) 转换方向，默认为正向 (kNormal)。
   */
  ColorProcessor(ColorManager* config, const QString& input, const ColorTransform& transform,
                 Direction direction = kNormal);
  /**
   * @brief 构造函数，直接使用一个已存在的 OCIO 处理器来创建一个 ColorProcessor。
   * @param processor 指向 OCIO::ConstProcessorRcPtr 的常量引用计数智能指针。
   */
  explicit ColorProcessor(OCIO::ConstProcessorRcPtr processor);

  // 禁用拷贝构造和移动构造/赋值，因为 ColorProcessor 可能管理底层 OCIO 资源，不宜直接拷贝或移动。
  DISABLE_COPY_MOVE(ColorProcessor) // 这是一个宏，通常用于删除拷贝和移动构造/赋值函数

  /**
   * @brief (静态工厂方法) 创建一个新的 ColorProcessor 实例 (通过共享指针)。
   * 参数与第一个构造函数相同。
   * @return 返回一个 ColorProcessorPtr (std::shared_ptr<ColorProcessor>)。
   */
  static ColorProcessorPtr Create(ColorManager* config, const QString& input, const ColorTransform& transform,
                                  Direction direction = kNormal);
  /**
   * @brief (静态工厂方法) 创建一个新的 ColorProcessor 实例 (通过共享指针)，直接使用 OCIO 处理器。
   * 参数与第二个构造函数相同。
   * @return 返回一个 ColorProcessorPtr。
   */
  static ColorProcessorPtr Create(const OCIO::ConstProcessorRcPtr& processor);

  /**
   * @brief 获取底层的 OCIO 处理器 (通常用于GPU处理)。
   * @return 返回 OCIO::ConstProcessorRcPtr。
   */
  OCIO::ConstProcessorRcPtr GetProcessor();

  /**
   * @brief 对给定的图像帧 (通过 FramePtr) 应用颜色转换 (通常在CPU上)。
   * @param f 指向要转换的帧的共享指针 (FramePtr)。帧数据会被原地修改。
   */
  void ConvertFrame(const FramePtr& f);
  /**
   * @brief 对给定的图像帧 (通过原始指针) 应用颜色转换 (通常在CPU上)。
   * @param f 指向要转换的帧的指针 (Frame*)。帧数据会被原地修改。
   */
  void ConvertFrame(Frame* f);

  /**
   * @brief 对单个颜色值应用颜色转换。
   * @param in 输入的 Color 对象。
   * @return 返回转换后的 Color 对象。
   */
  Color ConvertColor(const Color& in);

  /**
   * @brief 获取此颜色处理器的缓存ID。
   * 这个ID由底层的 OCIO 处理器提供，可用于缓存或识别特定的颜色转换流程。
   * @return 返回一个 C 风格字符串 (const char*) 作为ID。
   * (注意：如果 processor_ 可能为空，直接解引用 getCacheID() 是不安全的。
   *  但遵循不修改代码的原则，此处保留原始实现。)
   */
  [[nodiscard]] const char* id() const { return processor_->getCacheID(); }

 private:
  OCIO::ConstProcessorRcPtr processor_; // 指向 OCIO (GPU) 处理器的常量引用计数智能指针

  OCIO::ConstCPUProcessorRcPtr cpu_processor_; // 指向 OCIO CPU 处理器的常量引用计数智能指针
                                             // (用于在CPU上处理帧数据或单个颜色)
};

// 类型别名：ColorProcessorChain 代表一个 ColorProcessorPtr 的 QVector。
// 这可以用于表示一系列需要依次应用的颜色转换操作。
using ColorProcessorChain = QVector<ColorProcessorPtr>;

}  // namespace olive

// 声明 ColorProcessorPtr 类型为元类型，以便在 QVariant 中使用或在信号槽中传递
Q_DECLARE_METATYPE(olive::ColorProcessorPtr)

#endif  // COLORPROCESSOR_H