#ifndef TIMELINEVIEWMOUSEEVENT_H // 防止头文件被多次包含的宏定义
#define TIMELINEVIEWMOUSEEVENT_H

#include <QEvent>     // 引入 QEvent 类，是所有 Qt 事件类的基类
#include <QMimeData>  // 引入 QMimeData 类，用于处理拖放操作中的数据
#include <QPoint>     // 引入 QPoint 类，表示二维平面上的点 (整数坐标)
#include <QPointF>    // 引入 QPointF 类，表示二维平面上的点 (浮点数坐标)

#include "timeline/timelinecoordinate.h" // 引入 TimelineCoordinate 类的定义，表示时间轴上的坐标 (时间和轨道)
#include "widget/timebased/timescaledobject.h" // 引入 TimeScaledObject 类的定义，提供时间与场景坐标转换的功能

// 根据代码上下文，以下类型应由已包含的头文件或其传递包含的头文件提供定义：
// - olive::Track::Reference: 用于构造函数参数和成员变量 (Track 命名空间下的类型)
// - olive::rational: 用于构造函数参数和成员变量
// - Qt::MouseButton: Qt 枚举，用于构造函数参数和成员变量
// - Qt::KeyboardModifiers: Qt 枚举，用于构造函数参数和成员变量
// 此处严格按照用户提供的代码，不添加额外的 #include 或前向声明。

namespace olive { // olive 命名空间开始

/**
 * @brief TimelineViewMouseEvent 类封装了在时间轴视图 (TimelineView) 中发生的鼠标事件的详细信息。
 *
 * 它不仅包含标准的鼠标事件属性（如屏幕位置、场景位置、按下的按钮、修饰键），
 * 还包含了与时间轴相关的特定信息，如时间基准 (timebase)、当前轨道 (track)、
 * X轴缩放比例 (scale_x)，并提供了将场景坐标转换为时间帧 (rational) 的功能。
 * 此类还用于包装拖放事件中的 MIME 数据。
 */
class TimelineViewMouseEvent {
 public:
  /**
   * @brief 构造一个 TimelineViewMouseEvent 对象。
   * @param scene_pos 鼠标事件在场景坐标系中的位置 (QPointF)。
   * @param screen_pos 鼠标事件在屏幕（或视图控件）坐标系中的位置 (QPoint)。
   * @param scale_x 当前时间轴视图的 X 轴（时间轴）缩放比例 (double)。
   * @param timebase 当前时间轴的时间基准 (rational)，例如帧率。
   * @param track 鼠标事件发生时所在的轨道引用 (Track::Reference)。
   * @param button 触发事件的鼠标按钮 (Qt::MouseButton)。
   * @param modifiers 事件发生时的键盘修饰键状态 (Qt::KeyboardModifiers)，默认为 Qt::NoModifier。
   */
  TimelineViewMouseEvent(const QPointF& scene_pos, const QPoint& screen_pos, const double& scale_x,
                         const rational& timebase, const Track::Reference& track, const Qt::MouseButton& button,
                         const Qt::KeyboardModifiers& modifiers = Qt::NoModifier)
      : scene_pos_(scene_pos),         // 初始化场景位置
        screen_pos_(screen_pos),       // 初始化屏幕位置
        scale_x_(scale_x),             // 初始化X轴缩放比例
        timebase_(timebase),           // 初始化时间基准
        track_(track),                 // 初始化轨道引用
        button_(button),               // 初始化鼠标按钮
        modifiers_(modifiers),         // 初始化键盘修饰键
        source_event_(nullptr),        // 初始化源QEvent指针为空
        mime_data_(nullptr),           // 初始化MIME数据指针为空
        bypass_import_buffer_(false) {} // 初始化是否绕过导入缓冲为false

  /**
   * @brief 获取鼠标事件发生位置的时间轴坐标 (时间和轨道)。
   * @param round_time 是否对时间进行四舍五入，默认为 false (向下取整)。
   * @return 对应的时间轴坐标 (TimelineCoordinate)。
   */
  [[nodiscard]] TimelineCoordinate GetCoordinates(bool round_time = false) const {
    return TimelineCoordinate(GetFrame(round_time), track_); // 使用获取到的帧和当前轨道创建坐标
  }

  /**
   * @brief 获取事件发生时的键盘修饰键状态。
   * @return 对键盘修饰键状态 (Qt::KeyboardModifiers) 的常量引用。
   */
  [[nodiscard]] const Qt::KeyboardModifiers& GetModifiers() const { return modifiers_; }

  /**
   * @brief Gets the time at this cursor point
   * (原始英文注释：获取此光标点的时间)
   *
   * @param round
   * (原始英文注释：参数 round)
   *
   * If set to true, the time will be rounded to the nearest time. If set to false, the time is floored so the time is
   * always to the left of the cursor. The former behavior is better for clicking between frames (e.g. razor tool) and
   * the latter is better for clicking directly on frames (e.g. pointer tool).
   * (原始英文注释：如果设置为 true，时间将被四舍五入到最近的时间点。如果设置为 false，时间将向下取整，因此时间总是位于光标的左侧。
   * 前者更适用于在帧之间点击（例如剃刀工具），后者更适用于直接点击帧（例如指针工具）。)
   * @return 对应的时间帧 (rational)。
   */
  [[nodiscard]] rational GetFrame(bool round = false) const {
    // 使用 TimeScaledObject 的静态方法将场景X坐标转换为时间
    return TimeScaledObject::SceneToTime(GetSceneX(), scale_x_, timebase_, round);
  }

  /**
   * @brief 获取鼠标事件发生时所在的轨道引用。
   * @return 对轨道引用 (Track::Reference) 的常量引用。
   */
  [[nodiscard]] const Track::Reference& GetTrack() const { return track_; }

  /**
   * @brief 获取与此事件关联的 MIME 数据（主要用于拖放操作）。
   * @return 指向 QMimeData 对象的常量指针，如果不存在则为 nullptr。
   */
  const QMimeData* GetMimeData() { return mime_data_; }

  /**
   * @brief 设置与此事件关联的 MIME 数据。
   * @param data 指向 QMimeData 对象的常量指针。
   */
  void SetMimeData(const QMimeData* data) { mime_data_ = data; }

  /**
   * @brief 设置此鼠标事件的源 QEvent 对象。
   * @param event 指向源 QEvent 对象的指针。
   */
  void SetEvent(QEvent* event) { source_event_ = event; }

  /**
   * @brief 获取鼠标事件在场景坐标系中的 X 坐标。
   * @return 场景 X 坐标 (qreal)。
   */
  [[nodiscard]] qreal GetSceneX() const { return scene_pos_.x(); }

  /**
   * @brief 获取鼠标事件在场景坐标系中的完整位置。
   * @return 对场景位置 (QPointF) 的常量引用。
   */
  [[nodiscard]] const QPointF& GetScenePos() const { return scene_pos_; }
  /**
   * @brief 获取鼠标事件在屏幕（或视图控件）坐标系中的位置。
   * @return 对屏幕位置 (QPoint) 的常量引用。
   */
  [[nodiscard]] const QPoint& GetScreenPos() const { return screen_pos_; }

  /**
   * @brief 获取触发事件的鼠标按钮。
   * @return 对鼠标按钮 (Qt::MouseButton) 的常量引用。
   */
  [[nodiscard]] const Qt::MouseButton& GetButton() const { return button_; }

  /**
   * @brief 接受此事件（如果有关联的源 QEvent）。
   *
   * 调用源 QEvent 的 accept() 方法。
   */
  void accept() {
    if (source_event_ != nullptr) source_event_->accept();
  }

  /**
   * @brief 忽略此事件（如果有关联的源 QEvent）。
   *
   * 调用源 QEvent 的 ignore() 方法。
   */
  void ignore() {
    if (source_event_ != nullptr) source_event_->ignore();
  }

  /**
   * @brief 获取是否应绕过导入缓冲区的标志。
   * @return 如果应绕过导入缓冲区，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool GetBypassImportBuffer() const { return bypass_import_buffer_; }
  /**
   * @brief 设置是否应绕过导入缓冲区的标志。
   * @param e 如果为 true，则绕过导入缓冲区。
   */
  void SetBypassImportBuffer(bool e) { bypass_import_buffer_ = e; }

 private: // 私有成员变量
  QPointF scene_pos_; ///< 鼠标事件在场景坐标系中的位置。
  QPoint screen_pos_; ///< 鼠标事件在屏幕（或视图控件）坐标系中的位置。
  double scale_x_;    ///< 当前时间轴视图的 X 轴（时间轴）缩放比例。
  rational timebase_; ///< 当前时间轴的时间基准。

  Track::Reference track_; ///< 鼠标事件发生时所在的轨道引用。

  Qt::MouseButton button_; ///< 触发事件的鼠标按钮。

  Qt::KeyboardModifiers modifiers_; ///< 事件发生时的键盘修饰键状态。

  QEvent* source_event_; ///< 指向原始 Qt 事件 (QMouseEvent, QDragEvent 等) 的指针。

  const QMimeData* mime_data_; ///< 指向与拖放操作相关的 MIME 数据的常量指针。

  bool bypass_import_buffer_; ///< 标记在导入操作时是否应绕过（预）缓冲步骤。
};

}  // namespace olive

#endif  // TIMELINEVIEWMOUSEEVENT_H
