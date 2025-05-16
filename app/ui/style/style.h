#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QMap>       // 引入 QMap 类，用于存储可用主题的映射
#include <QPalette>   // 引入 QPalette 类，用于管理颜色调色板
#include <QSettings>  // 引入 QSettings 类，用于读取 .ini 格式的配置文件（例如调色板设置）
#include <QString>    // 引入 QString 类
#include <QWidget>    // 引入 QWidget 类，虽然在此头文件中未直接使用其成员，但样式管理通常应用于 QWidget 及其派生类

#include "common/define.h"  // 引入项目内通用的定义文件

namespace olive {

/**
 * @brief 管理应用程序的视觉样式和主题。
 *
 * StyleManager 类提供了一组静态方法来初始化、获取和设置应用程序的样式表和调色板。
 * 它能够解析存储在 .ini 文件中的调色板信息，并管理可用的主题列表。
 */
class StyleManager : public QObject {  // 继承自 QObject 以支持 Qt 的元对象系统，尽管在此静态类中可能不直接使用信号/槽
 public:
  /**
   * @brief 初始化样式管理器。
   *
   * 此函数通常在应用程序启动时调用，用于加载默认样式或用户配置的样式，
   * 并发现可用的主题。
   */
  static void Init();

  /**
   * @brief 获取当前应用的样式表路径或内容。
   * @return 返回一个 QString 的常量引用，表示当前样式的路径或标识符。
   */
  static const QString& GetStyle();

  /**
   * @brief 设置应用程序的样式。
   *
   * 这可能会涉及到加载新的样式表 (CSS) 和调色板，并将其应用于整个应用程序。
   * @param style_path 新样式的路径或标识符。
   */
  static void SetStyle(const QString& style_path);

  /**
   * @brief 默认样式的标识符或路径。
   *
   * 这是一个静态常量字符指针，指向默认样式的名称。
   */
  static const char* kDefaultStyle;

  /**
   * @brief 获取可用主题的映射。
   *
   * 返回一个 QMap，其中键是主题的内部名称/ID，值是用户可读的主题显示名称。
   * @return 返回可用主题映射的常量引用。
   */
  static const QMap<QString, QString>& available_themes() { return available_themes_; }

 private:
  /**
   * @brief 从指定的 .ini 文件路径解析调色板 (QPalette)。
   * @param ini_path 指向包含调色板设置的 .ini 文件的路径。
   * @return 返回解析得到的 QPalette 对象。
   */
  static QPalette ParsePalette(const QString& ini_path);

  /**
   * @brief 从 QSettings 对象中为指定的颜色组 (ColorGroup) 解析调色板颜色。
   * @param ini 指向已打开的 QSettings 对象的指针，该对象关联到 .ini 文件。
   * @param palette 指向要填充颜色信息的 QPalette 对象的指针。
   * @param group 要解析颜色的 QPalette::ColorGroup (例如，Active, Disabled, Inactive)。
   */
  static void ParsePaletteGroup(QSettings* ini, QPalette* palette, QPalette::ColorGroup group);

  /**
   * @brief 从 QSettings 对象中为指定的颜色组和颜色角色 (ColorRole) 解析单个颜色。
   * @param ini 指向 QSettings 对象的指针。
   * @param palette 指向 QPalette 对象的指针。
   * @param group 目标颜色组。
   * @param role_name 目标颜色角色的名称 (例如，"Window", "ButtonText")。
   */
  static void ParsePaletteColor(QSettings* ini, QPalette* palette, QPalette::ColorGroup group,
                                const QString& role_name);

  /**
   * @brief 存储当前活动样式的路径或标识符的静态 QString。
   */
  static QString current_style_;

  /**
   * @brief 存储可用主题列表的静态 QMap。
   * 键是主题的内部名称/ID，值是用户可读的显示名称。
   */
  static QMap<QString, QString> available_themes_;
};

}  // namespace olive

#endif  // STYLEMANAGER_H
