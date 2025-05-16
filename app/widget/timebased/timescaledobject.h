#ifndef TIMELINESCALEDOBJECT_H  // 防止头文件被多次包含的宏定义
#define TIMELINESCALEDOBJECT_H

#include <olive/core/core.h>  // 引入 Olive 核心库中的 core.h，可能包含 rational 等基本类型定义
#include <QWidget>            // 引入 QWidget 类，是所有用户界面对象的基类

#include "node/block/block.h"  // 引入 Block 类定义，具体用途需查看该文件

namespace olive {  // olive 命名空间开始

/**
 * @brief TimeScaledObject 类为使用时间和缩放的对象提供基础功能。
 *
 * 这个类封装了时间基准 (timebase) 和缩放比例 (scale) 的管理，
 * 以及在场景坐标和时间单位之间进行转换的方法。
 */
class TimeScaledObject {
 public:
  /**
   * @brief 构造一个 TimeScaledObject 对象。
   *
   * 初始化时间基准、缩放比例等成员变量。
   */
  TimeScaledObject();
  /**
   * @brief 析构 TimeScaledObject 对象。
   *
   * 默认为 default，允许派生类正确地进行析构。
   */
  virtual ~TimeScaledObject() = default;

  /**
   * @brief 设置时间基准。
   *
   * 时间基准通常表示帧率或每秒的单位数。
   * @param timebase 新的时间基准 (rational 类型)。
   */
  void SetTimebase(const rational& timebase);

  /**
   * @brief 获取当前的时间基准。
   * @return 对当前时间基准 (rational 类型) 的常量引用。
   */
  [[nodiscard]] const rational& timebase() const;
  /**
   * @brief 获取当前时间基准的 double 类型表示。
   * @return 当前时间基准的 double 类型值。
   */
  [[nodiscard]] const double& timebase_dbl() const;

  /**
   * @brief 将场景中的 X 坐标转换为时间。
   * @param x 场景中的 X 坐标 (double)。
   * @param x_scale X 轴的缩放比例 (double)。
   * @param timebase 时间基准 (rational)。
   * @param round 是否对结果进行四舍五入，默认为 false。
   * @return 对应的时间值 (rational)。
   */
  static rational SceneToTime(const double& x, const double& x_scale, const rational& timebase, bool round = false);
  /**
   * @brief 将场景中的 X 坐标转换为时间，不考虑网格吸附。
   * @param x 场景中的 X 坐标 (double)。
   * @param x_scale X 轴的缩放比例 (double)。
   * @return 对应的时间值 (rational)。
   */
  static rational SceneToTimeNoGrid(const double& x, const double& x_scale);

  /**
   * @brief 获取当前的（水平）缩放比例。
   * @return 对当前缩放比例 (double) 的常量引用。
   */
  [[nodiscard]] const double& GetScale() const;
  /**
   * @brief 获取允许的最大缩放比例。
   * @return 对最大缩放比例 (double) 的常量引用。
   */
  [[nodiscard]] const double& GetMaximumScale() const { return max_scale_; }

  /**
   * @brief 设置（水平）缩放比例。
   *
   * 此方法会确保新的缩放比例在最小和最大允许值之间。
   * @param scale 新的缩放比例 (double)。
   */
  void SetScale(const double& scale);

  /**
   * @brief 根据视口宽度和内容宽度设置缩放比例。
   * @param viewport_width 视口的宽度 (double)。
   * @param content_width 内容的总宽度 (double)。
   */
  void SetScaleFromDimensions(double viewport_width, double content_width);
  /**
   * @brief 根据视口尺寸和内容尺寸计算缩放比例。
   * @param viewport_sz 视口的尺寸 (double)。
   * @param content_sz 内容的总尺寸 (double)。
   * @return 计算得到的缩放比例 (double)。
   */
  static double CalculateScaleFromDimensions(double viewport_sz, double content_sz);
  /**
   * @brief 根据视口尺寸和缩放比例计算内边距。
   * @param viewport_sz 视口的尺寸 (double)。
   * @return 计算得到的内边距 (double)。
   */
  static double CalculatePaddingFromDimensionScale(double viewport_sz);

  /**
   * @brief 将时间值转换为场景中的 X 坐标。
   * @param time 要转换的时间值 (rational)。
   * @return 对应的场景 X 坐标 (double)。
   */
  [[nodiscard]] double TimeToScene(const rational& time) const;
  /**
   * @brief 将场景中的 X 坐标转换为时间。
   * @param x 场景中的 X 坐标 (double)。
   * @param round 是否对结果进行四舍五入，默认为 false。
   * @return 对应的时间值 (rational)。
   */
  [[nodiscard]] rational SceneToTime(const double& x, bool round = false) const;
  /**
   * @brief 将场景中的 X 坐标转换为时间，不考虑网格吸附。
   * @param x 场景中的 X 坐标 (double)。
   * @return 对应的时间值 (rational)。
   */
  [[nodiscard]] rational SceneToTimeNoGrid(const double& x) const;

 protected:  // 受保护成员
  /**
   * @brief 当时间基准发生变化时调用的虚函数。
   *
   * 派生类可以重写此函数以响应时间基准的变化。
   * @param timebase 新的时间基准 (rational)。 // 参数名根据上下文推断，原始代码中省略
   */
  virtual void TimebaseChangedEvent(const rational&) {}

  /**
   * @brief 当（水平）缩放比例发生变化时调用的虚函数。
   *
   * 派生类可以重写此函数以响应缩放比例的变化。
   * @param scale 新的缩放比例 (double)。 // 参数名根据上下文推断，原始代码中省略
   */
  virtual void ScaleChangedEvent(const double&) {}

  /**
   * @brief 设置允许的最大缩放比例。
   * @param max 新的最大缩放比例 (double)。
   */
  void SetMaximumScale(const double& max);

  /**
   * @brief 设置允许的最小缩放比例。
   * @param min 新的最小缩放比例 (double)。
   */
  void SetMinimumScale(const double& min);

 private:              // 私有成员
  rational timebase_;  ///< 当前的时间基准。

  double timebase_dbl_{};  ///< 时间基准的 double 类型缓存值。

  double scale_;  ///< 当前的（水平）缩放比例。

  double min_scale_;  ///< 允许的最小缩放比例。

  double max_scale_;  ///< 允许的最大缩放比例。

  /**
   * @brief 用于 CalculateScaleFromDimensions 计算中的内边距常量。
   */
  static const int kCalculateDimensionsPadding;
};

/**
 * @brief TimelineScaledWidget 类是一个结合了 QWidget 和 TimeScaledObject 功能的控件。
 *
 * 它继承自 QWidget 以获得 Qt 控件的特性，并继承自 TimeScaledObject 以获得时间缩放管理功能。
 * 这个类可以作为所有需要在时间轴上显示并支持缩放的自定义控件的基类。
 */
class TimelineScaledWidget : public QWidget, public TimeScaledObject {
 Q_OBJECT  // Q_OBJECT 宏，用于启用 Qt 的元对象特性
     public :
     /**
      * @brief 构造一个 TimelineScaledWidget 对象。
      * @param parent 父 QWidget 对象，默认为 nullptr。
      */
     explicit TimelineScaledWidget(QWidget* parent = nullptr);
};

}  // namespace olive

#endif  // TIMELINESCALEDOBJECT_H
