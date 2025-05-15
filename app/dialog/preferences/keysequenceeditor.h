#ifndef KEYSEQUENCEEDITOR_H
#define KEYSEQUENCEEDITOR_H

#include <QKeySequenceEdit>

#include "common/debug.h"

namespace olive {

/**
 * @class KeySequenceEditor
 * @brief 键盘快捷键序列编辑器类
 *
 * 一个简单的 QKeySequenceEdit 派生类，用于关联一个 QAction，并提供在该 QAction 与编辑器之间传递键盘快捷键的功能。
 */
class KeySequenceEditor : public QKeySequenceEdit {
  Q_OBJECT
 public:
  /**
   * @brief KeySequenceEditor 构造函数
   *
   * @param parent 父 QWidget 对象指针。
   * @param a 要关联的 QAction 对象指针。此 QAction 在 KeySequenceEditor 的生命周期内不可更改。
   */
  KeySequenceEditor(QWidget *parent, QAction *a);

  /**
   * @brief 将此编辑字段中输入的快捷键设置给关联的 QAction。
   *
   * 此操作不会自动执行，以防用户在“首选项”对话框中取消操作，
   * 此时期望所做的更改不会被保存。因此，当“首选项对话框”保存时，需要手动触发此函数。
   */
  void set_action_shortcut();

  /**
   * @brief 将此编辑字段中的快捷键重置为关联 QAction 的默认快捷键。
   *
   * 每个 QAction 在其 `property("default")` 中都包含默认快捷键，
   * 可以使用此函数来恢复“硬编码”的默认快捷键。
   *
   * 此函数不会将默认快捷键保存回 QAction，它仅仅是将 QAction 中的默认快捷键加载到此编辑字段中。
   * 要将其保存到 QAction 中，需要在调用此函数后调用 set_action_shortcut()。
   */
  void reset_to_default();

  /**
   * @brief 返回关联 QAction 的唯一 ID。
   *
   * Olive 的每个菜单动作都有一个唯一的字符串 ID（与文本不同，它不会被翻译），
   * 用于与外部快捷键配置文件进行匹配。该 ID 存储在 QAction 的 `property("id")` 中。
   * 此函数返回该 ID。
   *
   * @return QAction 的唯一 ID。
   */
  QString action_name();

  /**
   * @brief 将此快捷键条目序列化为可以保存到文件的字符串。
   *
   * @return 此快捷键的字符串序列化表示。
   * 格式为 "[ID]\t[SEQUENCE]"，其中 [ID] 是关联 QAction 的唯一标识符，
   * [SEQUENCE] 是字段中当前的键盘快捷键（不一定是 QAction 中的快捷键）。
   * 如果输入的快捷键与 QAction 的默认快捷键相同，则返回空字符串，
   * 因为默认快捷键不需要保存到文件中。
   */
  QString export_shortcut();

protected:
  /**
   * @brief 重写 QKeySequenceEdit 的按键按下事件处理函数。
   * @param e QKeyEvent 事件对象指针。
   */
  void keyPressEvent(QKeyEvent *e) override;

  /**
   * @brief 重写 QKeySequenceEdit 的按键释放事件处理函数。
   * @param e QKeyEvent 事件对象指针。
   */
  void keyReleaseEvent(QKeyEvent *e) override;

private:
  /**
   * @brief 指向关联的 QAction 的内部引用。
   */
  QAction *action;
};

}  // namespace olive

#endif  // KEYSEQUENCEEDITOR_H