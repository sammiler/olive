#ifndef TIMELINEWORKAREA_H
#define TIMELINEWORKAREA_H

#include <olive/core/core.h>  // 引入 Olive 核心库的头文件，提供 TimeRange, rational 等核心类型
#include <QObject>            // 引入 QObject 基类，用于信号和槽机制
#include <QXmlStreamReader>   // 引入 QXmlStreamReader 类，用于从 XML 读取数据
#include <QXmlStreamWriter>   // 引入 QXmlStreamWriter 类，用于向 XML 写入数据

namespace olive {

using namespace core;  // 使用 olive::core 命名空间，方便直接访问其成员如 TimeRange, rational

/**
 * @brief 管理时间轴上的一个可定义的工作区域。
 *
 * TimelineWorkArea 类代表用户可以在时间轴上设定的一个特定范围，
 * 通常用于标记感兴趣的区域，例如用于循环播放、限定渲染范围或导出范围等。
 * 它包含一个启用状态和一个时间范围（入点和出点）。
 */
class TimelineWorkArea : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief 显式构造函数。
   * @param parent 父 QObject 对象，默认为 nullptr。
   */
  explicit TimelineWorkArea(QObject* parent = nullptr);

  /**
   * @brief 检查工作区是否已启用。
   * @return 如果工作区已启用，则返回 true；否则返回 false。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool enabled() const;

  /**
   * @brief 设置工作区的启用状态。
   *
   * 如果状态发生变化，会发出 EnabledChanged() 信号。
   * @param e true 表示启用工作区，false 表示禁用。
   */
  void set_enabled(bool e);

  /**
   * @brief 获取工作区的入点时间。
   * @return 返回工作区入点时间的常量引用 (rational 类型)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const rational& in() const;

  /**
   * @brief 获取工作区的出点时间。
   * @return 返回工作区出点时间的常量引用 (rational 类型)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const rational& out() const;

  /**
   * @brief 获取工作区的长度（出点与入点之差）。
   * @return 返回工作区长度的常量引用 (rational 类型)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const rational& length() const;

  /**
   * @brief 获取工作区的时间范围对象。
   * @return 返回工作区时间范围 (TimeRange) 的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const TimeRange& range() const;

  /**
   * @brief 设置工作区的时间范围。
   *
   * 如果范围发生变化，会发出 RangeChanged() 信号。
   * @param range 新的时间范围 (TimeRange 类型)。
   */
  void set_range(const TimeRange& range);

  /**
   * @brief 从 XML 流中加载工作区数据。
   * @param reader 指向 QXmlStreamReader 对象的指针，用于读取 XML。
   * @return 如果加载成功返回 true，否则返回 false。
   */
  bool load(QXmlStreamReader* reader);

  /**
   * @brief 将工作区数据保存到 XML 流中。
   * @param writer 指向 QXmlStreamWriter 对象的指针，用于写入 XML。
   */
  void save(QXmlStreamWriter* writer) const;

  /**
   * @brief 工作区重置时的默认入点值。
   *
   * 通常为0或者一个非常小的值。
   */
  static const rational kResetIn;

  /**
   * @brief 工作区重置时的默认出点值。
   *
   * 通常为一个表示较短持续时间的值，例如10秒（假设时间基准）。
   */
  static const rational kResetOut;

 signals:
  /**
   * @brief 当工作区的启用状态发生改变时发出的信号。
   * @param e 新的启用状态 (true 为启用, false 为禁用)。
   */
  void EnabledChanged(bool e);

  /**
   * @brief 当工作区的时间范围发生改变时发出的信号。
   * @param r 新的时间范围。
   */
  void RangeChanged(const TimeRange& r);

 private:
  bool workarea_enabled_;     // 存储工作区的启用状态
  TimeRange workarea_range_;  // 存储工作区的时间范围（入点和出点）
};

}  // namespace olive

#endif  // TIMELINEWORKAREA_H
