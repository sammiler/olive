#ifndef MANAGEDDISPLAYOBJECT_H
#define MANAGEDDISPLAYOBJECT_H

// #define USE_QOPENGLWINDOW // 条件编译宏，用于选择使用 QOpenGLWindow 还是 QOpenGLWidget

#include <QMouseEvent>     // Qt 鼠标事件类
#include <QOpenGLContext>  // Qt OpenGL 上下文类
#ifdef USE_QOPENGLWINDOW
#include <QOpenGLWindow>   // Qt OpenGL 窗口类 (如果 USE_QOPENGLWINDOW 被定义)
#else
#include <QOpenGLWidget>   // Qt OpenGL 控件类 (如果 USE_QOPENGLWINDOW 未定义或被注释掉)
#endif

#include "node/color/colormanager/colormanager.h" // 色彩管理器类
#include "render/renderer.h"                       // 渲染器基类
#include "widget/menu/menu.h"                      // 自定义菜单基类

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWidget;
// class QMenu;
// class QAction;
// class QPaintDevice;
// class QRect;
// class QObject;
// class QEvent;
// class QWindow; // 如果 USE_QOPENGLWINDOW 被定义

namespace olive {

/**
 * @brief ManagedDisplayWidgetOpenGL 类是一个封装了 OpenGL 初始化、绘制和销毁信号的辅助控件/窗口。
 *
 * 根据 USE_QOPENGLWINDOW 宏的定义，此类可以是一个 QOpenGLWindow 或 QOpenGLWidget。
 * 它主要通过信号将 OpenGL 相关的生命周期事件通知给外部。
 */
class ManagedDisplayWidgetOpenGL
#ifdef USE_QOPENGLWINDOW
    : public QOpenGLWindow // 如果定义了 USE_QOPENGLWINDOW，则继承自 QOpenGLWindow
#else
    : public QOpenGLWidget // 否则，继承自 QOpenGLWidget
#endif
{
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 默认构造函数。
   */
  ManagedDisplayWidgetOpenGL() = default;

  /**
   * @brief 析构函数。
   *
   * 在析构时，如果 OpenGL 上下文存在，则断开连接并发出 OnDestroy 信号。
   */
  ~ManagedDisplayWidgetOpenGL() override {
    if (context()) { // 检查 OpenGL 上下文是否存在
      DestroyListener(); // 调用销毁监听器
      // 断开 aboutToBeDestroyed 信号与 DestroyListener 槽的连接
      disconnect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &ManagedDisplayWidgetOpenGL::DestroyListener);
    }
  }

 signals:
  // 渲染信号
  /**
   * @brief 当 OpenGL 上下文初始化完成时发出此信号。
   */
  void OnInit();
  /**
   * @brief 当需要进行 OpenGL 绘制时发出此信号。
   */
  void OnPaint();
  /**
   * @brief 当 OpenGL 上下文即将被销毁时发出此信号。
   */
  void OnDestroy();

 protected:
  /**
   * @brief OpenGL 初始化函数，在第一次显示控件或窗口时调用。
   */
  void initializeGL() override {
    // 连接 OpenGL 上下文的 aboutToBeDestroyed 信号到 DestroyListener 槽
    // Qt::DirectConnection 确保槽函数在信号发出者所在线程立即执行
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &ManagedDisplayWidgetOpenGL::DestroyListener,
            Qt::DirectConnection);

    emit OnInit(); // 发出初始化完成信号
  }

  /**
   * @brief OpenGL 绘制函数，在需要重绘时调用。
   */
  void paintGL() override { emit OnPaint(); } // 发出绘制信号

 private slots:
  /**
   * @brief OpenGL 上下文销毁前的监听槽函数。
   *
   * 确保在正确的上下文中发出 OnDestroy 信号。
   */
  void DestroyListener() {
    makeCurrent(); // 将当前 OpenGL 上下文设置为此控件/窗口的上下文

    emit OnDestroy(); // 发出销毁信号

    doneCurrent(); // 释放当前 OpenGL 上下文
  }
};

// 宏定义，用于简化派生类中 OpenGL 资源清理的析构函数实现
#define MANAGEDDISPLAYWIDGET_DEFAULT_DESTRUCTOR_INNER \
  makeCurrent(); /* 设置当前 OpenGL 上下文 */        \
  OnDestroy();   /* 调用虚函数执行清理 */            \
  doneCurrent()  /* 释放上下文 */
#define MANAGEDDISPLAYWIDGET_DEFAULT_DESTRUCTOR(x) \
  virtual ~x() override { MANAGEDDISPLAYWIDGET_DEFAULT_DESTRUCTOR_INNER; } // 定义一个虚析构函数并使用上述宏

/**
 * @brief ManagedDisplayWidget 类是一个用于显示渲染结果并集成色彩管理的 QWidget。
 *
 * 它内部包含一个 ManagedDisplayWidgetOpenGL 实例（或 QWindow）作为实际的绘图表面。
 * 此类负责连接 ColorManager，处理色彩变换，并提供用于选择色彩空间的菜单。
 * 派生类需要实现 OnPaint() 方法来定义具体的渲染逻辑。
 */
class ManagedDisplayWidget : public QWidget {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit ManagedDisplayWidget(QWidget* parent = nullptr);

  /**
   * @brief 析构函数。
   */
  ~ManagedDisplayWidget() override;

  /**
   * @brief 断开当前连接的色彩管理器 (等效于 ConnectColorManager(nullptr))。
   */
  void DisconnectColorManager();

  /**
   * @brief 获取当前连接的色彩管理器。
   * @return 如果已连接，则返回 ColorManager 指针；否则返回 nullptr。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] ColorManager* color_manager() const;

  /**
   * @brief 获取当前的色彩变换设置。
   * @return 返回一个 const 引用，指向 ColorTransform 对象。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const ColorTransform& GetColorTransform() const;

  /**
   * @brief 获取用于选择输入色彩空间的菜单。
   * @param parent 菜单的父 QMenu 指针。
   * @param auto_connect 如果为 true (默认)，则菜单的 action 会自动连接到内部槽函数以更新色彩空间。
   * @return 返回一个 Menu 指针 (项目自定义菜单类)。
   */
  Menu* GetColorSpaceMenu(QMenu* parent, bool auto_connect = true);

  /**
   * @brief 获取用于选择显示设备变换的菜单。
   * @param parent 菜单的父 QMenu 指针。
   * @param auto_connect 如果为 true (默认)，则菜单的 action 会自动连接到内部槽函数。
   * @return 返回一个 Menu 指针。
   */
  Menu* GetDisplayMenu(QMenu* parent, bool auto_connect = true);

  /**
   * @brief 获取用于选择视图变换的菜单。
   * @param parent 菜单的父 QMenu 指针。
   * @param auto_connect 如果为 true (默认)，则菜单的 action 会自动连接到内部槽函数。
   * @return 返回一个 Menu 指针。
   */
  Menu* GetViewMenu(QMenu* parent, bool auto_connect = true);

  /**
   * @brief 获取用于选择外观 (look) 变换的菜单。
   * @param parent 菜单的父 QMenu 指针。
   * @param auto_connect 如果为 true (默认)，则菜单的 action 会自动连接到内部槽函数。
   * @return 返回一个 Menu 指针。
   */
  Menu* GetLookMenu(QMenu* parent, bool auto_connect = true);

  /**
   * @brief 将更新请求传递给内部的绘图控件。
   */
  void update();

  /**
   * @brief 事件过滤器，可用于捕获和处理子控件或其他对象的事件。
   * @param o 被观察的对象。
   * @param e 发生的事件。
   * @return 如果事件被过滤（即已处理并不应进一步传递），则返回 true；否则返回 false。
   */
  bool eventFilter(QObject* o, QEvent* e) override;

 public slots:
  /**
   * @brief 设置新的色彩变换。
   * @param transform 要应用的 ColorTransform 对象。
   */
  void SetColorTransform(const ColorTransform& transform);

  /**
   * @brief 连接到一个色彩管理器。色彩管理器通常属于项目 (Project)。
   * @param color_manager 指向要连接的 ColorManager 对象的指针。传入 nullptr 以断开连接。
   */
  void ConnectColorManager(ColorManager* color_manager);

 signals:
  /**
   * @brief 当色彩处理器发生改变时发出此信号。
   * @param processor 指向新的 ColorProcessor 的智能指针。
   */
  void ColorProcessorChanged(ColorProcessorPtr processor);

  /**
   * @brief 当连接到新的色彩管理器时发出此信号。
   * @param color_manager 指向新连接的 ColorManager 对象的指针。
   */
  void ColorManagerChanged(ColorManager* color_manager);

  /**
   * @brief 当内部绘图控件的帧交换完成时发出此信号 (通常与 QOpenGLWindow 相关)。
   */
  void frameSwapped();

 protected:
  /**
   * @brief 提供对当前色彩处理器的访问。
   * @return 如果已设置，则返回 ColorProcessorPtr；否则返回空指针。
   */
  ColorProcessorPtr color_service();

  /**
   * @brief 启用一个默认的上下文菜单，允许用户快速访问 DVL (Display, View, Look) 管线设置。
   */
  void EnableDefaultContextMenu();

  /**
   * @brief 当色彩处理器发生改变时调用的虚函数。
   *
   * 默认实现仅调用 update() 来请求重绘。派生类可以重写此函数以执行特定操作。
   */
  virtual void ColorProcessorChangedEvent();

  /**
   * @brief 获取附加的渲染器实例。
   * @return 指向 Renderer 对象的指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Renderer* renderer() const { return attached_renderer_; }

  /**
   * @brief 设置当前 OpenGL 上下文为内部绘图控件的上下文。
   */
  void makeCurrent();

  /**
   * @brief 释放当前 OpenGL 上下文。
   */
  void doneCurrent();

#ifdef USE_QOPENGLWINDOW
  QWindow* // 如果使用 QOpenGLWindow，则内部控件是 QWindow*
#else
  [[nodiscard]] QWidget* // 否则，内部控件是 QWidget*
#endif
  /**
   * @brief 获取内部的绘图控件/窗口。
   * @return 返回指向内部绘图控件/窗口的指针。
   */
  inner_widget() const {
    return inner_widget_;
  }

  /**
   * @brief 获取内部控件作为 QPainter 的绘图设备。
   *
   * 注意：此函数可能与 QVulkanWindow 不兼容，因此使用它的函数可能需要后续修改。
   * @return 指向 QPaintDevice 对象的指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QPaintDevice* paint_device() const;

  /**
   * @brief 设置内部绘图控件的鼠标跟踪状态。
   * @param e 如果为 true，则启用鼠标跟踪；否则禁用。
   */
  void SetInnerMouseTracking(bool e);

  /**
   * @brief 获取内部绘图控件（或其包装器）的矩形区域。
   * @return 返回一个 QRect 对象。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QRect GetInnerRect() const { return wrapper_ ? wrapper_->rect() : QRect(); }

  /**
   * @brief 获取视口的视频参数（例如尺寸、像素宽高比）。
   * @return 返回一个 VideoParams 对象。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] VideoParams GetViewportParams() const;

 protected slots:
  /**
   * @brief 当内部渲染上下文创建完成时调用的虚函数。
   *
   * 派生类应重写此函数以执行其 OpenGL 相关的初始化。
   */
  virtual void OnInit();

  /**
   * @brief 当需要渲染内部上下文时调用的纯虚函数。
   *
   * 派生类必须实现此函数以执行实际的绘制逻辑。
   */
  virtual void OnPaint() = 0;

  /**
   * @brief 在内部渲染上下文即将被销毁之前调用的虚函数。
   *
   * 派生类应重写此函数以释放其 OpenGL 资源。
   */
  virtual void OnDestroy();

 private:
  /**
   * @brief 当用户选择了不同的显示设备、视图或外观时，调用此函数以重新创建色彩处理器。
   */
  void SetupColorProcessor();

  /**
   * @brief 清理函数，用于释放 OCIO LUT（查找表）相关的纹理资源。
   */
  void ClearOCIOLutTexture();

  /**
   * @brief 主要的绘图表面抽象。
   */
#ifdef USE_QOPENGLWINDOW
  QWindow* inner_widget_; ///< 指向内部 QWindow 的指针 (如果 USE_QOPENGLWINDOW 被定义)。
#else
  QWidget* inner_widget_; ///< 指向内部 QOpenGLWidget (或其基类 QWidget) 的指针。
#endif
  QWidget* wrapper_;      ///< 如果 inner_widget_ 是 QWindow，则此 wrapper_ 是用于将其嵌入 QWidget 布局的容器。

  /**
   * @brief 渲染器抽象。
   */
  Renderer* attached_renderer_; ///< 指向附加的渲染器对象的指针。

  /**
   * @brief 连接的色彩管理器。
   */
  ColorManager* color_manager_; ///< 指向当前连接的色彩管理器对象的指针。

  /**
   * @brief 色彩管理服务。
   */
  ColorProcessorPtr color_service_; ///< 指向当前活动的色彩处理器对象的智能指针。

  /**
   * @brief 内部存储的色彩变换设置。
   */
  ColorTransform color_transform_; ///< 当前应用的色彩变换 (显示、视图、外观等)。

 private slots:
  /**
   * @brief 当色彩配置（例如从 ColorManager 加载的配置）发生改变时调用的槽函数。
   */
  void ColorConfigChanged();

  /**
   * @brief 显示默认的上下文菜单。
   */
  void ShowDefaultContextMenu();

  /**
   * @brief 如果 GetDisplayMenu() 被调用且 auto_connect 为 true，则此槽函数会连接到菜单项的 triggered 信号。
   * @param action 被触发的 QAction 指针。
   */
  void MenuDisplaySelect(QAction* action);

  /**
   * @brief 如果 GetViewMenu() 被调用且 auto_connect 为 true，则此槽函数会连接到菜单项的 triggered 信号。
   * @param action 被触发的 QAction 指针。
   */
  void MenuViewSelect(QAction* action);

  /**
   * @brief 如果 GetLookMenu() 被调用且 auto_connect 为 true，则此槽函数会连接到菜单项的 triggered 信号。
   * @param action 被触发的 QAction 指针。
   */
  void MenuLookSelect(QAction* action);

  /**
   * @brief 如果 GetColorSpaceMenu() 被调用且 auto_connect 为 true，则此槽函数会连接到菜单项的 triggered 信号。
   * @param action 被触发的 QAction 指针。
   */
  void MenuColorspaceSelect(QAction* action);
};

}  // namespace olive

#endif  // MANAGEDDISPLAYOBJECT_H
