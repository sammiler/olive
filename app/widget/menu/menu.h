#ifndef WIDGETMENU_H
#define WIDGETMENU_H

#include <QMenu>       // Qt 菜单基类
#include <QMenuBar>    // Qt 菜单栏类
#include <QMetaMethod> // Qt 元方法类，用于模板元编程和信号槽连接

#include "common/define.h" // 项目通用定义

// QtPrivate::FunctionPointer 是 Qt 内部用于模板元编程处理函数指针的辅助类，
// 通常在 Qt 的 connect 调用中用于类型安全的信号槽连接。
// QKeySequence, QAction, QObject, QString, QVariant, QWidget 等 Qt 类
// 通常会被 QMenu 或 QMenuBar 间接包含，或者作为函数参数/返回值类型时，
// 编译器能找到其声明。根据用户要求，不添加显式的前向声明。

namespace olive {

/**
 * @brief Menu 类是一个用于上下文菜单和菜单栏的菜单控件。
 *
 * 它是 QMenu 的子类，提供了一些函数来创建符合 Olive 菜单和键盘快捷键系统的菜单和菜单项。
 *
 * 在 Olive 中，菜单栏中的菜单项也负责整个应用程序的键盘快捷键。
 * 为了使这些快捷键可配置且配置可保存，每个菜单项都需要一个唯一的 ID。
 * 这个 ID 会在配置文件中与键盘快捷键关联起来。ID 不会被翻译，因此可以在语言更改后保持不变。
 *
 * ID 存储在 QAction 的 "id" 属性中。如果提供了键盘快捷键，则会存储在 QAction 的 "keydefault" 属性中。
 *
 * 强烈建议在任何情况下都使用此类代替 QMenu。
 */
class Menu : public QMenu {
  // Q_OBJECT // 通常 QMenu 已经包含了 Q_OBJECT，子类无需重复，除非定义了新的信号或槽。遵循不修改原则。
 public:
  /**
   * @brief 构造函数，创建一个菜单并将其添加到菜单栏。
   * @param bar 要添加此菜单的 QMenuBar 指针。
   */
  explicit Menu(QMenuBar* bar);

  /**
   * @brief 模板构造函数，创建一个菜单，将其添加到菜单栏，并连接 aboutToShow 信号。
   *
   * 此菜单可以连接到一个槽函数，该槽函数在菜单即将显示时被触发。
   * 使用 `receiver` 和 `member` 来连接此信号 (语法与 QObject::connect 相同)，
   * 如果不需要连接，则将它们保留为 nullptr。
   * @tparam Func 槽函数的类型。
   * @param bar 要添加此菜单的 QMenuBar 指针。
   * @param receiver 接收 aboutToShow 信号的 QObject 对象指针。
   * @param member 要连接到 aboutToShow 信号的槽函数指针。
   */
  template <typename Func>
  Menu(QMenuBar* bar, const typename QtPrivate::FunctionPointer<Func>::Object* receiver, Func member) {
    bar->addMenu(this); // 将当前菜单添加到菜单栏

    Init(); // 执行通用初始化
    ConnectAboutToShow(receiver, member); // 连接 aboutToShow 信号
  }

  /**
   * @brief 构造函数，创建一个子菜单并将其添加到另一个 Menu 对象。
   * @param menu 父 Menu 对象的指针。
   */
  explicit Menu(Menu* menu);

  template <typename Func>
  /**
   * @brief 模板构造函数，创建一个子菜单，将其添加到另一个 Menu 对象，并连接 aboutToShow 信号。
   *
   * 此菜单可以连接到一个槽函数，该槽函数在菜单即将显示时被触发。
   * 使用 `receiver` 和 `member` 来连接此信号 (语法与 QObject::connect 相同)，
   * 如果不需要连接，则将它们保留为 nullptr。
   * @tparam Func 槽函数的类型。
   * @param menu 父 Menu 对象的指针。
   * @param receiver 接收 aboutToShow 信号的 QObject 对象指针。
   * @param member 要连接到 aboutToShow 信号的槽函数指针。
   */
  Menu(Menu* menu, const typename QtPrivate::FunctionPointer<Func>::Object* receiver, Func member) {
    menu->addMenu(this); // 将当前菜单作为子菜单添加到父菜单

    Init(); // 执行通用初始化
    ConnectAboutToShow(receiver, member); // 连接 aboutToShow 信号
  }

  /**
   * @brief 构造函数，创建一个弹出式菜单。
   * @param parent 父 QWidget 指针，默认为 nullptr。
   */
  explicit Menu(QWidget* parent = nullptr);

  /**
   * @brief 构造函数，创建一个带标题的弹出式菜单。
   * @param s 菜单的标题文本。
   * @param parent 父 QWidget 指针，默认为 nullptr。
   */
  explicit Menu(const QString& s, QWidget* parent = nullptr);

  template <typename Func>
  /**
   * @brief 创建一个菜单项的静态方法。
   *
   * @param parent QAction 的父 QObject 指针。
   * @param id 动作的唯一 ID。
   * @param member 当动作被触发时要调用的槽函数 (成员函数指针)。
   * @param key 默认的键盘快捷键序列。
   * @return 返回创建并配置好的 QAction 指针。
   */
  static QAction* CreateItem(QObject* parent, const QString& id, Func member,
                             const QKeySequence& key = QKeySequence()) {
    auto* a = new QAction(parent); // 创建 QAction

    ConformItem(a, id, member, key); // 使 QAction 符合 Olive 的菜单系统

    return a;
  }

  template <typename Func>
  /**
   * @brief 使一个 QAction 符合 Olive 的 ID/keydefault 系统。
   *
   * 如果一个 QAction 是在其他地方创建的 (例如通过 QUndoStack::createUndoAction())，
   * 此函数会给它设置属性，使其符合 Olive 的菜单项系统。
   *
   * @param a 要配置的 QAction 指针。
   * @param id 动作的唯一 ID。
   * @param member 当动作被触发时要调用的槽函数 (成员函数指针)。
   * @param key 默认的键盘快捷键序列。
   */
  static void ConformItem(QAction* a, const QString& id, Func member, const QKeySequence& key = QKeySequence()) {
    ConformItem(a, id, key); // 调用重载版本设置 ID 和快捷键

    connect(a, &QAction::triggered, member); // 连接 triggered 信号到槽函数
  }

  template <typename Func>
  /**
   * @brief 创建一个菜单项并将其添加到此菜单中。
   *
   * @param id 动作的唯一 ID。
   * @param member 当动作被触发时要调用的槽函数 (成员函数指针)。
   * @param key 默认的键盘快捷键序列。
   * @return 返回创建并添加到此菜单的 QAction 指针。
   */
  QAction* AddItem(const QString& id, Func member, const QKeySequence& key = QKeySequence()) {
    QAction* a = CreateItem(this, id, member, key); // 创建菜单项，父对象为当前菜单

    addAction(a); // 将动作添加到菜单

    return a;
  }

  template <typename Func>
  /**
   * @brief 创建一个菜单项并将其添加到此菜单中（带接收者对象的版本）。
   *
   * @param id 动作的唯一 ID。
   * @param receiver 接收 triggered 信号的 QObject 对象指针。
   * @param member 当动作被触发时要调用的槽函数 (成员函数指针)。
   * @param key 默认的键盘快捷键序列。
   * @return 返回创建并添加到此菜单的 QAction 指针。
   */
  QAction* AddItem(const QString& id, const typename QtPrivate::FunctionPointer<Func>::Object* receiver, Func member,
                   const QKeySequence& key = QKeySequence()) {
    QAction* a = CreateItem(this, id, receiver, member, key); // 创建菜单项，父对象为当前菜单

    addAction(a); // 将动作添加到菜单

    return a;
  }

  /**
   * @brief 添加一个带有数据的动作，并根据 compare 值决定其初始选中状态。
   * @param text 动作显示的文本。
   * @param d 与动作关联的数据 (QVariant)。
   * @param compare 用于与 d比较以确定初始选中状态的数据 (QVariant)。
   * @return 返回创建并添加的 QAction 指针。
   */
  QAction* AddActionWithData(const QString& text, const QVariant& d, const QVariant& compare);

  /**
   * @brief 按字母顺序插入一个带有指定文本的新动作。
   * @param s 新动作的文本。
   * @return 返回创建并插入的 QAction 指针。
   */
  QAction* InsertAlphabetically(const QString& s);
  /**
   * @brief 按字母顺序插入一个已有的 QAction。
   * @param entry 要插入的 QAction 指针。
   */
  void InsertAlphabetically(QAction* entry);
  /**
   * @brief 按字母顺序插入一个子菜单。
   * @param menu 要插入的 Menu 指针。
   */
  void InsertAlphabetically(Menu* menu);

  template <typename Func>
  /**
   * @brief 创建一个菜单项的静态方法（带接收者对象的版本）。
   *
   * @param parent QAction 的父 QObject 指针。
   * @param id 动作的唯一 ID。
   * @param receiver 接收 triggered 信号的 QObject 对象指针。
   * @param member 当动作被触发时要调用的槽函数 (成员函数指针)。
   * @param key 默认的键盘快捷键序列。
   * @return 返回创建并配置好的 QAction 指针。
   */
  static QAction* CreateItem(QObject* parent, const QString& id,
                             const typename QtPrivate::FunctionPointer<Func>::Object* receiver, Func member,
                             const QKeySequence& key = QKeySequence()) {
    auto* a = new QAction(parent); // 创建 QAction

    ConformItem(a, id, receiver, member, key); // 使 QAction 符合 Olive 的菜单系统

    return a;
  }

  template <typename Func>
  /**
   * @brief 使一个 QAction 符合 Olive 的 ID/keydefault 系统（带接收者对象的版本）。
   *
   * 如果一个 QAction 是在其他地方创建的，此函数会给它设置属性，使其符合 Olive 的菜单项系统。
   *
   * @param a 要配置的 QAction 指针。
   * @param id 动作的唯一 ID。
   * @param receiver 接收 triggered 信号的 QObject 对象指针。
   * @param member 当动作被触发时要调用的槽函数 (成员函数指针)。
   * @param key 默认的键盘快捷键序列。
   */
  static void ConformItem(QAction* a, const QString& id,
                          const typename QtPrivate::FunctionPointer<Func>::Object* receiver, Func member,
                          const QKeySequence& key = QKeySequence()) {
    ConformItem(a, id, key); // 调用重载版本设置 ID 和快捷键

    connect(a, &QAction::triggered, receiver, member); // 连接 triggered 信号到指定接收者的槽函数
  }

  /**
   * @brief 使一个 QAction 符合 Olive 的 ID/keydefault 系统（仅设置 ID 和快捷键的基础版本）。
   * @param a 要配置的 QAction 指针。
   * @param id 动作的唯一 ID。
   * @param key 默认的键盘快捷键序列。
   */
  static void ConformItem(QAction* a, const QString& id, const QKeySequence& key = QKeySequence());

 private:
  /**
   * @brief 执行菜单的通用初始化操作。
   */
  void Init();

  /**
   * @brief 模板辅助函数，用于连接当前菜单的 aboutToShow 信号到指定的接收者和槽函数。
   * @tparam Func 槽函数的类型。
   * @param receiver 接收 aboutToShow 信号的 QObject 对象指针。
   * @param member 要连接到 aboutToShow 信号的槽函数指针。
   */
  template <typename Func>
  void ConnectAboutToShow(const typename QtPrivate::FunctionPointer<Func>::Object* receiver, Func member) {
    // 连接 this (当前 Menu 对象) 的 aboutToShow 信号到 receiver 对象的 member 槽函数
    connect(this, &Menu::aboutToShow, receiver, member);
  }
};

}  // namespace olive

#endif  // WIDGETMENU_H
