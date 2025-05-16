#ifndef PANELFOCUSMANAGER_H  // 防止头文件被重复包含的宏
#define PANELFOCUSMANAGER_H  // 定义 PANELFOCUSMANAGER_H 宏

#include <QList>    // Qt 列表容器
#include <QObject>  // Qt 对象模型基类

#include "panel/panel.h"  // 包含 PanelWidget 基类的定义

namespace olive {  // olive 项目的命名空间

/**
 * @brief PanelManager 类 (文件名 PanelFocusManager.h 可能暗示其早期或特定用途)
 *
 * 这个对象负责在任何给定时间跟踪哪个面板拥有“焦点”。
 *
 * 有时一个功能 (特别是通过键盘触发的功能，例如 Delete 键) 根据当前哪个面板拥有“焦点”
 * 而可能有不同的用途。例如，在时间轴上按 Delete 键与在项目面板中按 Delete 键的意义是不同的。
 * 这种“焦点”与标准的 QWidget 焦点略有不同，因为它旨在针对面板控件 (PanelWidget) 整体，
 * 而不是像单个 QPushButton 或 QLineEdit 那样具体。
 *
 * PanelManager 的槽函数 FocusChanged() 连接到 QApplication 实例的信号 focusChanged()，
 * 因此它始终知道应用程序内部的焦点何时发生了变化。
 *
 * (注意：类名是 PanelManager，而文件名是 PanelFocusManager.h，这可能是一个历史遗留问题或特定命名约定)
 */
class PanelManager : public QObject {  // PanelManager 继承自 QObject
 Q_OBJECT                              // 声明此类使用 Qt 的元对象系统

     public :
     /**
      * @brief 构造函数。
      * @param parent 父对象指针，默认为 nullptr。
      */
     explicit PanelManager(QObject* parent = nullptr);

  /**
   * @brief销毁所有面板。
   *
   * 应该仅在应用程序退出时使用，以干净地释放所有面板资源。
   */
  void DeleteAllPanels();

  /**
   * @brief 获取所有现有面板的列表。
   *
   * 面板按最近获得焦点到最远获得焦点的顺序排列。
   * @return 返回一个包含 PanelWidget 指针的 QList 的常量引用。
   */
  const QList<PanelWidget*>& panels();

  /**
   * @brief 返回当前拥有“焦点”的面板，如果没有任何面板拥有焦点则返回 nullptr。
   *
   * 如果 enable_hover 为 true 并且鼠标悬停在某个面板上，其结果可能与 CurrentlyHovered() 相同
   * (具体行为取决于实现，通常焦点优先于悬停)。
   * @param enable_hover 是否考虑鼠标悬停作为一种“焦点”形式 (通常用于某些即时交互)。
   * @return 返回当前聚焦的 PanelWidget 指针。
   */
  [[nodiscard]] PanelWidget* CurrentlyFocused(bool enable_hover = true) const;

  /**
   * @brief 返回鼠标当前悬停在其上的面板，如果没有则返回 nullptr。
   * @return 返回当前悬停的 PanelWidget 指针。
   */
  [[nodiscard]] PanelWidget* CurrentlyHovered() const;

  /**
   * @brief 根据名称查找并返回一个面板。
   * @param name 要查找的面板的名称 (通常是其 Qt 对象名称)。
   * @return 返回找到的 PanelWidget 指针，如果未找到则返回 nullptr。
   */
  [[nodiscard]] PanelWidget* GetPanelWithName(const QString& name) const;

  /**
   * @brief (模板方法) 获取指定类型的最近获得“焦点”的面板。
   * @tparam T 要查找的面板类型 (必须是 PanelWidget 的派生类)。
   * @return 返回找到的 T 类型面板指针，如果不存在则返回 nullptr。
   */
  template <class T>
  T* MostRecentlyFocused();

  /**
   * @brief 创建 PanelManager 的单例实例。
   */
  static void CreateInstance();

  /**
   * @brief销毁 PanelManager 的单例实例。
   *
   * 如果 PanelManager 未被创建，则此操作无效。
   */
  static void DestroyInstance();

  /**
   * @brief 访问 PanelManager 的单例实例。
   * @return 返回 PanelManager 的静态实例指针。
   */
  static PanelManager* instance();

  /**
   * @brief (模板方法) 获取指定类型的所有面板的列表。
   * @tparam T 要查找的面板类型。
   * @return 返回一个包含 T 类型面板指针的 QList。
   */
  template <class T>
  QList<T*> GetPanelsOfType();

  /**
   * @brief 面板应在构造时调用此方法，以便 PanelManager 可以跟踪它。
   * @param panel 要注册的 PanelWidget 指针。
   */
  void RegisterPanel(PanelWidget* panel);

  /**
   * @brief 面板应在析构时调用此方法，以便 PanelManager 不会保留无效的指针。
   * @param panel 要注销的 PanelWidget 指针。
   */
  void UnregisterPanel(PanelWidget* panel);

  /**
   * @brief 设置是否抑制 FocusedPanelChanged 信号的发出。
   * 在某些批量操作或内部状态调整时可能需要。
   * @param e 如果为 true，则抑制信号；否则正常发出。
   */
  void SetSuppressChangedSignal(bool e) { suppress_changed_signal_ = e; }

 public slots:  // Qt 公有槽函数
  /**
   * @brief 将此槽连接到 QApplication 的 SIGNAL(focusChanged(QWidget*, QWidget*))。
   *
   * 解析焦点信息以确定当前拥有“焦点”的面板。
   * @param old 失去焦点的 QWidget 指针。
   * @param now 获得焦点的 QWidget 指针。
   */
  void FocusChanged(QWidget* old, QWidget* now);

 signals:  // Qt 信号声明
  /**
   * @brief 当当前拥有“焦点”的面板发生改变时发出的信号。
   * @param panel 新获得“焦点”的 PanelWidget 指针，如果无面板聚焦则为 nullptr。
   */
  void FocusedPanelChanged(PanelWidget* panel);

 private:
  /**
   * @brief 用于遍历的历史记录数组 (参见 MostRecentlyFocused())。
   * 存储了面板获得焦点的顺序。
   */
  QList<PanelWidget*> focus_history_;

  /**
   * @brief PanelManager 的单例实例指针。
   */
  static PanelManager* instance_;

  bool suppress_changed_signal_;  // 标记是否抑制 FocusedPanelChanged 信号的发出
};

// --- 模板成员函数的实现 ---

template <class T>
T* PanelManager::MostRecentlyFocused() {
  T* cast_test;  // 用于动态类型转换的临时指针

  // 从最近获得焦点的开始遍历 focus_history_
  for (auto i : focus_history_) {     // Qt 的 foreach 循环
    cast_test = dynamic_cast<T*>(i);  // 尝试将 PanelWidget 指针转换为 T 类型指针

    if (cast_test != nullptr) {  // 如果转换成功 (即 i 是 T 类型或其派生类型)
      return cast_test;          // 返回第一个找到的匹配类型的面板
    }
  }

  return nullptr;  // 如果没有找到指定类型的面板，则返回 nullptr
}

template <class T>
QList<T*> PanelManager::GetPanelsOfType() {
  QList<T*> panels;  // 用于存储结果的列表
  T* cast_test;      // 用于动态类型转换的临时指针

  // 遍历所有已注册的面板 (focus_history_ 包含了所有面板，并按焦点顺序排列)
  foreach (PanelWidget* panel, focus_history_) {  // Qt 的 foreach 宏
    cast_test = dynamic_cast<T*>(panel);          // 尝试转换

    if (cast_test) {             // 如果转换成功
      panels.append(cast_test);  // 将其添加到结果列表中
    }
  }

  return panels;  // 返回包含所有 T 类型面板的列表
}

}  // namespace olive

#endif  // PANELFOCUSMANAGER_H