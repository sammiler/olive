#ifndef MENUSHARED_H
#define MENUSHARED_H

#include <olive/core/core.h> // 引入 Olive 核心库，可能包含 rational 等类型
#include "widget/colorlabelmenu/colorlabelmenu.h" // 自定义颜色标签菜单类
#include "widget/menu/menu.h"                    // 自定义的 Menu 基类

// Qt 类的前向声明 (根据用户要求，不添加)
// class QObject;
// class QAction;
// class QActionGroup;
// class QMenu; // Menu 类已继承自 QMenu
// class QString;
// class QVariant;
// class QWidget; // Menu 的构造函数参数

namespace olive {

using namespace core; // 使用 olive::core 命名空间

/**
 * @brief MenuShared 类是一个静态对象，提供在整个应用程序中使用的各种“标准”菜单和菜单项。
 *
 * 它作为单例存在，集中管理常用的菜单动作，如文件操作、编辑操作、颜色编码等，
 * 方便在不同地方复用这些菜单项，并统一处理其行为和快捷键。
 */
class MenuShared : public QObject {
  Q_OBJECT // Qt 元对象系统宏，用于支持信号和槽机制

 public:
  /**
   * @brief 构造函数。
   *
   * 通常在 CreateInstance() 中被调用。
   */
  MenuShared();
  /**
   * @brief 析构函数。
   */
  ~MenuShared() override;

  /**
   * @brief 创建 MenuShared 的单例实例。
   *
   * 如果实例尚不存在，则创建它。
   */
  static void CreateInstance();
  /**
   * @brief 销毁 MenuShared 的单例实例。
   *
   * 释放单例对象占用的资源。
   */
  static void DestroyInstance();

  /**
   * @brief 重新翻译所有共享菜单项的文本。
   *
   * 当应用程序语言更改时调用此方法，以更新菜单项的显示文本。
   */
  void Retranslate();

  /**
   * @brief 为“新建”类型的菜单添加标准的菜单项。
   * @param m 要添加项目的 Menu 指针。
   */
  void AddItemsForNewMenu(Menu* m);
  /**
   * @brief 为“编辑”类型的菜单添加标准的菜单项。
   * @param m 要添加项目的 Menu 指针。
   * @param for_clips 布尔值，指示这些编辑项是否特别针对剪辑操作（可能会影响某些项的可用性或行为）。
   */
  void AddItemsForEditMenu(Menu* m, bool for_clips);
  /**
   * @brief 为“可添加对象”类型的菜单（例如，创建新节点或素材的菜单）添加标准的菜单项。
   * @param m 要添加项目的 Menu 指针。
   */
  void AddItemsForAddableObjectsMenu(Menu* m);
  /**
   * @brief 为“入点/出点”操作的菜单添加标准的菜单项。
   * @param m 要添加项目的 Menu 指针。
   */
  void AddItemsForInOutMenu(Menu* m);
  /**
   * @brief 添加颜色编码子菜单到指定的菜单。
   * @param m 要添加颜色编码子菜单的 Menu 指针。
   */
  void AddColorCodingMenu(Menu* m);
  /**
   * @brief 为“剪辑编辑”相关的上下文菜单添加标准的菜单项。
   * @param m 要添加项目的 Menu 指针。
   */
  void AddItemsForClipEditMenu(Menu* m);
  /**
   * @brief 为时间标尺的上下文菜单添加标准的菜单项。
   * @param m 要添加项目的 Menu 指针。
   */
  void AddItemsForTimeRulerMenu(Menu* m);

  /**
   * @brief 在时间标尺菜单即将显示之前，根据当前时间基准更新相关菜单项的状态。
   * @param timebase 当前的时间基准 (rational 类型)。
   */
  void AboutToShowTimeRulerActions(const rational& timebase);

  /**
   * @brief 获取 MenuShared 的单例实例。
   * @return 返回 MenuShared 的静态实例指针。
   */
  static MenuShared* instance();

  /**
   * @brief 获取“编辑”菜单中的“删除”动作项。
   * @return 返回指向“删除”QAction 的指针。
   */
  QAction* edit_delete_item() { return edit_delete_item_; }

 public slots:
  /**
   * @brief “删除选中项”动作被触发时的静态槽函数。
   */
  static void DeleteSelectedTriggered();

 private:
  // "新建" 菜单的共享项
  QAction* new_project_item_;  ///< “新建项目”菜单项。
  QAction* new_sequence_item_; ///< “新建序列”菜单项。
  QAction* new_folder_item_;   ///< “新建文件夹”菜单项。

  // "编辑" 菜单的共享项
  QAction* edit_cut_item_;            ///< “剪切”菜单项。
  QAction* edit_copy_item_;           ///< “复制”菜单项。
  QAction* edit_paste_item_;          ///< “粘贴”菜单项。
  QAction* edit_paste_insert_item_;   ///< “粘贴并插入”菜单项。
  QAction* edit_duplicate_item_;      ///< “复制副本”或“克隆”菜单项。
  QAction* edit_rename_item_;         ///< “重命名”菜单项。
  QAction* edit_delete_item_;         ///< “删除”菜单项。
  QAction* edit_ripple_delete_item_;  ///< “波纹删除”菜单项。
  QAction* edit_split_item_;          ///< “分割剪辑”或“在播放头分割”菜单项。
  QAction* edit_speedduration_item_;  ///< “速度/持续时间”菜单项。

  // 可添加对象的列表
  QVector<QAction*> addable_items_; ///< 存储代表可添加对象（如各种节点类型）的 QAction 列表。

  // "入点/出点" 菜单的共享项
  QAction* inout_set_in_item_;      ///< “设置入点”菜单项。
  QAction* inout_set_out_item_;     ///< “设置出点”菜单项。
  QAction* inout_reset_in_item_;    ///< “重置入点”或“跳转到入点”菜单项。
  QAction* inout_reset_out_item_;   ///< “重置出点”或“跳转到出点”菜单项。
  QAction* inout_clear_inout_item_; ///< “清除入点和出点”菜单项。

  // "剪辑编辑" 菜单的共享项
  QAction* clip_add_default_transition_item_; ///< “添加默认转场”菜单项。
  QAction* clip_link_unlink_item_;            ///< “链接/取消链接音视频”菜单项。
  QAction* clip_enable_disable_item_;         ///< “启用/禁用剪辑”菜单项。
  QAction* clip_nest_item_;                   ///< “嵌套剪辑”（创建子序列）菜单项。

  // 时间标尺菜单的共享项
  QActionGroup* frame_view_mode_group_;             ///< 用于时间码显示模式的动作组（确保互斥选择）。
  QAction* view_timecode_view_dropframe_item_;      ///< 以“掉帧时间码”模式显示。
  QAction* view_timecode_view_nondropframe_item_;   ///< 以“不掉帧时间码”模式显示。
  QAction* view_timecode_view_seconds_item_;        ///< 以“秒”模式显示。
  QAction* view_timecode_view_frames_item_;         ///< 以“帧”模式显示。
  QAction* view_timecode_view_milliseconds_item_;   ///< 以“毫秒”模式显示。

  // 颜色编码菜单项
  ColorLabelMenu* color_coding_menu_; ///< 指向颜色标签子菜单的指针。

  static MenuShared* instance_; ///< MenuShared 的静态单例实例指针。

 private slots:
  /**
   * @brief “在播放头分割”动作被触发时的静态槽函数。
   */
  static void SplitAtPlayheadTriggered();

  /**
   * @brief “波纹删除”动作被触发时的静态槽函数。
   */
  static void RippleDeleteTriggered();

  /**
   * @brief “设置入点”动作被触发时的静态槽函数。
   */
  static void SetInTriggered();

  /**
   * @brief “设置出点”动作被触发时的静态槽函数。
   */
  static void SetOutTriggered();

  /**
   * @brief “重置入点”动作被触发时的静态槽函数。
   */
  static void ResetInTriggered();

  /**
   * @brief “重置出点”动作被触发时的静态槽函数。
   */
  static void ResetOutTriggered();

  /**
   * @brief “清除入点和出点”动作被触发时的静态槽函数。
   */
  static void ClearInOutTriggered();

  /**
   * @brief “链接/取消链接”动作被触发时的静态槽函数。
   */
  static void ToggleLinksTriggered();

  /**
   * @brief “剪切”动作被触发时的静态槽函数。
   */
  static void CutTriggered();

  /**
   * @brief “复制”动作被触发时的静态槽函数。
   */
  static void CopyTriggered();

  /**
   * @brief “粘贴”动作被触发时的静态槽函数。
   */
  static void PasteTriggered();

  /**
   * @brief “粘贴并插入”动作被触发时的静态槽函数。
   */
  static void PasteInsertTriggered();

  /**
   * @brief “复制副本”动作被触发时的静态槽函数。
   */
  static void DuplicateTriggered();

  /**
   * @brief “重命名选中项”动作被触发时的静态槽函数。
   */
  static void RenameSelectedTriggered();

  /**
   * @brief “启用/禁用”动作被触发时的静态槽函数。
   */
  static void EnableDisableTriggered();

  /**
   * @brief “嵌套”动作被触发时的静态槽函数。
   */
  static void NestTriggered();

  /**
   * @brief “添加默认转场”动作被触发时的静态槽函数。
   */
  static void DefaultTransitionTriggered();

  /**
   * @brief 时间码显示模式菜单项被触发时的槽函数。
   *
   * 假设 sender() 是一个 QAction*，其 data() 是 Timecode::Display 枚举的成员。
   * 使用 data() 来在应用程序的其余部分发出时间码更改信号。
   */
  void TimecodeDisplayTriggered();

  /**
   * @brief 颜色标签菜单项被触发时的静态槽函数。
   * @param color_index 被选中的颜色标签的索引。
   */
  static void ColorLabelTriggered(int color_index);

  /**
   * @brief “速度/持续时间”动作被触发时的静态槽函数。
   */
  static void SpeedDurationTriggered();

  /**
   * @brief 当“可添加对象”列表中的某个项目被触发时调用的槽函数。
   */
  void AddableItemTriggered();
};

}  // namespace olive

#endif  // MENUSHARED_H
