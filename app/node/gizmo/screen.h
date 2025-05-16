#ifndef SCREENGIZMO_H  // 防止头文件被多次包含的宏定义开始
#define SCREENGIZMO_H

#include "draggable.h"  // 引入基类 DraggableGizmo (或类似可拖动 Gizmo 基类) 的定义
                        // 假设 "draggable.h" 是正确的基类头文件名

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 代表一个“屏幕” Gizmo (图形交互控件)，或者更具体地说，是一个覆盖整个屏幕或某个大区域的可交互 Gizmo。
 * 这种 Gizmo
 * 通常用于捕捉在特定区域内的鼠标事件，例如用于实现自由变换工具中的旋转控制（当鼠标在对象外部一定范围内拖动时）
 * 或者用于画布平移等操作。
 * 它继承自 DraggableGizmo，表明它可以响应拖动事件。
 * 这个类本身非常简单，可能其主要逻辑和具体行为在派生类中或通过信号槽连接到其他控制器来实现。
 */
class ScreenGizmo : public DraggableGizmo {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief ScreenGizmo 构造函数。
      * @param parent 父 QObject 对象，默认为 nullptr。
      */
     explicit ScreenGizmo(QObject *parent = nullptr);
};

}  // namespace olive

#endif  // SCREENGIZMO_H // 头文件宏定义结束