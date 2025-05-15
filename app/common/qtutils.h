

#ifndef QTVERSIONABSTRACTION_H
#define QTVERSIONABSTRACTION_H

#include <olive/core/core.h> // 包含 olive::core 命名空间的基础定义，如 rational, Color, TimeRange, Bezier, AudioParams, SampleBuffer
#include <QComboBox>
#include <QDateTime>
#include <QFileInfo>
#include <QFontMetrics>
#include <QFrame>
#include <QMessageBox>
#include <QStringList>      // 为了 WordWrapString 和其他可能的 QStringList 使用
#include <QWidget>          // 为了 MsgBox 的 parent 参数和 GetParentOfType
#include <QVariant>         // 为了 PtrToValue 和 ValueToPtr
#include <QColor>           // 为了 toQColor
#include <QHash>            // 为了 qHash 函数的 seed 参数
#include <QMetaType>        // 为了 Q_DECLARE_METATYPE

// Qt::KeyboardModifiers 在 <Qt> 模块中，通常通过 <QInputEvent> 或 <QEvent> 包含，
// 如果没有，可能需要 #include <QtGui/qevent.h> 或类似头文件，
// 但 Qt::KeyboardModifier 通常作为 Qt 命名空间下的类型是可用的。

namespace olive {

/**
 * @brief 提供一系列 Qt 相关辅助功能和版本抽象的静态工具类。
 *
 *此类封装了常用的 Qt 操作，例如处理不同 Qt 版本间的 API 差异
 * (如 QFontMetrics 的宽度计算)、创建标准UI元素 (如水平/垂直分割线)、
 * 显示消息框、获取文件创建日期、格式化日期时间、文字换行、
 * 修饰键翻转、QComboBox 数据设置、查找特定类型的父对象、
 * 颜色类型转换以及指针与QVariant之间的转换等。
 */
class QtUtils {
 public:
  /**
   * @brief 根据指定的 QFontMetrics 获取字符串的宽度。
   *
   * QFontMetrics::width() 在 Qt 5.11+ 中已被废弃，推荐使用 QFontMetrics::horizontalAdvance()。
   * 此函数封装了这种版本差异：对于 Qt 5.11+ 版本使用 horizontalAdvance()，对于更早版本使用 width()。
   * @param fm 用于计算宽度的 QFontMetrics 对象。
   * @param s 要计算宽度的字符串。
   * @return int 字符串的像素宽度。
   */
  static int QFontMetricsWidth(const QFontMetrics &fm, const QString &s);

  /**
   * @brief 创建一个标准样式的水平分割线 QFrame。
   * @return QFrame* 指向新创建的水平分割线 QFrame 对象的指针。调用者不负责释放此对象（除非没有父对象）。
   */
  static QFrame *CreateHorizontalLine();

  /**
   * @brief 创建一个标准样式的垂直分割线 QFrame。
   * @return QFrame* 指向新创建的垂直分割线 QFrame 对象的指针。调用者不负责释放此对象（除非没有父对象）。
   */
  static QFrame *CreateVerticalLine();

  /**
   * @brief 显示一个标准的消息框。
   * @param parent 父 QWidget 指针，消息框将显示在其上。
   * @param icon 消息框的图标类型 (例如 QMessageBox::Information, QMessageBox::Warning)。
   * @param title 消息框的标题。
   * @param message 要在消息框中显示的消息文本。
   * @param buttons 消息框中显示的按钮组合 (例如 QMessageBox::Ok, QMessageBox::Save | QMessageBox::Cancel)。默认为 QMessageBox::Ok。
   * @return int 用户点击的按钮的 QMessageBox::StandardButton 枚举值。
   */
  static int MsgBox(QWidget *parent, QMessageBox::Icon icon, const QString &title, const QString &message,
                    QMessageBox::StandardButtons buttons = QMessageBox::Ok);

  /**
   * @brief 获取文件的创建日期和时间。
   * 注意：文件创建日期的可用性和精度取决于操作系统和文件系统。
   * @param info 描述文件的 QFileInfo 对象。
   * @return QDateTime 文件的创建日期和时间。如果无法获取，可能返回无效的 QDateTime。
   */
  static QDateTime GetCreationDate(const QFileInfo &info);

  /**
   * @brief 将 QDateTime 对象格式化为易读的字符串表示。
   * 格式通常为本地化的日期和时间格式。
   * @param dt 要格式化的 QDateTime 对象。
   * @return QString 格式化后的日期时间字符串。
   */
  static QString GetFormattedDateTime(const QDateTime &dt);

  /**
   * @brief 根据给定的字体和边界宽度对字符串进行自动换行。
   * @param s 要进行换行的原始字符串。
   * @param fm 用于计算字符和字符串宽度的 QFontMetrics 对象。
   * @param bounding_width 允许的最大行宽度（像素）。
   * @return QStringList 包含换行后各行文本的字符串列表。
   */
  static QStringList WordWrapString(const QString &s, const QFontMetrics &fm, int bounding_width);

  /**
   * @brief 翻转键盘修饰键中的 Control 和 Shift 键的状态。
   * 例如，如果输入包含 ControlModifier，则输出将包含 MetaModifier (在 macOS 上 Command 键常被视为 Meta)，反之亦然。
   * （此函数的确切行为，特别是Control和Shift之间的翻转，需要参照其实现细节来精确描述，此处基于函数名推测）。
   * 假设是 Control 和 Meta (Command on macOS) 之间的翻转，或者 Control 和某个通用“主要”修饰键的翻转。
   * 如果是 Control 和 Shift，则：如果输入 Control，输出 Shift；如果输入 Shift，输出 Control。
   * @param e 原始的 Qt::KeyboardModifiers 枚举值。
   * @return Qt::KeyboardModifiers 翻转了 Control 和 Shift (或 Meta) 状态后的修饰键。
   */
  static Qt::KeyboardModifiers FlipControlAndShiftModifiers(Qt::KeyboardModifiers e);

  /**
   * @brief 设置 QComboBox 当前选中项的用户数据 (int 类型)。
   * 此函数会查找 QComboBox 中与给定整数数据关联的项，并将其设为当前项。
   * @param cb 指向要操作的 QComboBox 对象的指针。
   * @param data 要查找并选中的用户数据 (int)。
   */
  static void SetComboBoxData(QComboBox *cb, int data);
  /**
   * @brief 设置 QComboBox 当前选中项的用户数据 (QString 类型)。
   * 此函数会查找 QComboBox 中与给定字符串数据关联的项，并将其设为当前项。
   * @param cb 指向要操作的 QComboBox 对象的指针。
   * @param data 要查找并选中的用户数据 (QString)。
   */
  static void SetComboBoxData(QComboBox *cb, const QString &data);

  /**
   * @brief 从指定的 QObject 子对象开始，向上遍历其父对象链，查找第一个类型为 T 的父对象。
   * @tparam T 要查找的父对象的类型。
   * @param child 开始查找的子 QObject 对象。
   * @return T* 如果找到，则返回指向类型为 T 的父对象的指针；否则返回 nullptr。
   */
  template <typename T>
  static T *GetParentOfType(const QObject *child) {
    QObject *t = child->parent(); // 获取父对象

    while (t) { // 当父对象存在时循环
      if (T *p = dynamic_cast<T *>(t)) { // 尝试将父对象动态转换为类型 T
        return p; // 如果转换成功，则返回该父对象指针
      }
      t = t->parent(); // 继续向上查找下一级父对象
    }

    return nullptr; // 如果未找到指定类型的父对象，则返回 nullptr
  }

  /**
   * @brief 将 olive::core::Color 对象转换为 Qt 的 QColor 对象。
   * @param i 输入的 olive::core::Color 对象。
   * @return QColor 转换后的 QColor 对象。
   */
  static QColor toQColor(const core::Color &i);

  /**
   * @brief 将任意类型的指针转换为一个可以存储在 QVariant 中的值 (通常是 quintptr)。
   * 这对于需要在不直接暴露指针类型的情况下（例如在节点参数 NodeParams 中）传递指针非常有用。
   * @param ptr 要转换的原始指针。
   * @return QVariant 包含指针转换后值的 QVariant 对象。
   */
  static QVariant PtrToValue(void *ptr) { return reinterpret_cast<quintptr>(ptr); }

  /**
   * @brief 将之前通过 PtrToValue 存储在 QVariant 中的值转换回指定类型的指针。
   * @tparam T 期望的目标指针类型。
   * @param ptr 包含指针转换后值的 QVariant 对象。
   * @return T* 转换回的类型为 T 的指针。
   * @warning 使用此函数时必须确保 QVariant 中存储的值确实是可安全转换为类型 T* 的指针，否则可能导致未定义行为。
   */
  template <class T>
  static T *ValueToPtr(const QVariant &ptr) {
    return reinterpret_cast<T *>(ptr.value<quintptr>());
  }
};

namespace core { // 这个命名空间似乎是为了给 olive::core 中的类型提供 Qt 哈希函数

/**
 * @brief 为 olive::core::rational 类型重载 qHash 函数。
 * 使得此类型能被 Qt 的哈希容器（如 QHash, QSet）正确处理。
 * @param r 要计算哈希值的 rational 对象。
 * @param seed 哈希种子，用于链式哈希或改善分布。
 * @return uint 计算得到的哈希值。
 */
uint qHash(const core::rational &r, uint seed = 0);

/**
 * @brief 为 olive::core::TimeRange 类型重载 qHash 函数。
 * 使得此类型能被 Qt 的哈希容器正确处理。
 * @param r 要计算哈希值的 TimeRange 对象。
 * @param seed 哈希种子。
 * @return uint 计算得到的哈希值。
 */
uint qHash(const core::TimeRange &r, uint seed = 0);

}  // namespace core

}  // namespace olive

/**
 * @brief 将 olive::core::rational 类型注册到 Qt 元对象系统。
 * 允许此类型在 QVariant 中使用，以及用于信号槽连接等。
 */
Q_DECLARE_METATYPE(olive::core::rational)
/**
 * @brief 将 olive::core::Color 类型注册到 Qt 元对象系统。
 */
Q_DECLARE_METATYPE(olive::core::Color)
/**
 * @brief 将 olive::core::TimeRange 类型注册到 Qt 元对象系统。
 */
Q_DECLARE_METATYPE(olive::core::TimeRange)
/**
 * @brief 将 olive::core::Bezier 类型注册到 Qt 元对象系统。
 */
Q_DECLARE_METATYPE(olive::core::Bezier)
/**
 * @brief 将 olive::core::AudioParams 类型注册到 Qt 元对象系统。
 */
Q_DECLARE_METATYPE(olive::core::AudioParams)
/**
 * @brief 将 olive::core::SampleBuffer 类型注册到 Qt 元对象系统。
 */
Q_DECLARE_METATYPE(olive::core::SampleBuffer)

#endif  // QTVERSIONABSTRACTION_H