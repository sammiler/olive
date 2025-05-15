

#ifndef CONFIG_H
#define CONFIG_H

#include <QMap>     // 用于存储配置项的映射
#include <QString>  // 用于配置键和值
#include <QVariant> // 用于存储不同类型的配置数据
#include <QDir>     // 为了 GetConfigFilePath 中可能用到的 QDir

#include "node/value.h"    // 包含 NodeValue::Type，用于定义配置项的类型
#include "common/define.h" // 可能包含 QStringLiteral 等宏或通用定义

namespace olive {

/**
 * @brief 快速访问指定键的配置项值的宏。
 * 使用 `QStringLiteral` 优化字符串字面量的创建。
 * @param x 配置项的键名 (应为C字符串字面量，例如 "General/Theme")。
 * @return QVariant 对配置项值的引用 (可修改)。
 * @see Config::Current()
 * @see Config::operator[](const QString&)
 */
#define OLIVE_CONFIG(x) Config::Current()[QStringLiteral(x)]

/**
 * @brief 快速访问指定键的配置项值的宏 (接受 QString 作为键)。
 * @param x 配置项的键名 (QString 类型)。
 * @return QVariant 对配置项值的引用 (可修改)。
 * @see Config::Current()
 * @see Config::operator[](const QString&)
 */
#define OLIVE_CONFIG_STR(x) Config::Current()[x]

/**
 * @brief 管理应用程序配置项的单例类。
 *
 *此类负责加载、保存和提供对 Olive 编辑器各种配置设置的访问。
 * 配置项以键值对的形式存储，其中值可以是多种类型 (通过 QVariant)。
 * 配置通常从文件中加载，并在应用程序关闭时保存。
 */
class Config {
 public:
  /**
   * @brief 获取 Config 类的全局单例实例。
   * @return Config& 对当前配置对象的引用。
   */
  static Config& Current();

  /**
   * @brief 设置所有配置项的默认值。
   * 通常在首次运行或用户请求重置配置时调用。
   */
  void SetDefaults();

  /**
   * @brief 从配置文件加载所有配置项。
   * 配置文件的路径通过 GetConfigFilePath() 获取。
   */
  static void Load();

  /**
   * @brief 将当前所有配置项保存到配置文件。
   * 配置文件的路径通过 GetConfigFilePath() 获取。
   */
  static void Save();

  /**
   * @brief 重载下标操作符，用于获取指定键的配置项的值 (const 版本)。
   * @param key 要获取的配置项的键名。
   * @return QVariant 配置项的值。如果键不存在，可能返回无效的 QVariant。
   */
  QVariant operator[](const QString& key) const;

  /**
   * @brief 重载下标操作符，用于获取或设置指定键的配置项的值 (非 const 版本)。
   * 如果键不存在，此操作可能会创建一个新的配置项条目 (具体行为取决于实现)。
   * @param key 要获取或设置的配置项的键名。
   * @return QVariant& 对配置项值的引用，允许修改。
   */
  QVariant& operator[](const QString& key);

  /**
   * @brief 获取指定配置项键的数据类型。
   * @param key 要查询的配置项的键名。
   * @return NodeValue::Type 配置项的数据类型。如果键不存在，则返回 NodeValue::Type::kInvalid。
   */
  [[nodiscard]] NodeValue::Type GetConfigEntryType(const QString& key) const;

 private:
  /**
   * @brief 私有构造函数，用于实现单例模式。
   */
  Config();
  // 拷贝构造和赋值操作符应被禁用以维护单例特性
  Config(const Config&) = delete;
  Config& operator=(const Config&) = delete;

  /**
   * @brief 存储单个配置项及其类型和数据的内部结构体。
   */
  struct ConfigEntry {
    NodeValue::Type type; ///< @brief 配置项的数据类型 (例如整数、字符串、布尔等)。
    QVariant data;        ///< @brief 配置项的实际数据，使用 QVariant 存储以支持多种类型。
  };

  /**
   * @brief 内部方法，用于设置配置项的类型和数据。
   * 此方法通常在 SetDefaults 或加载配置时使用。
   * @param key 配置项的键名。
   * @param type 配置项的数据类型。
   * @param data 配置项的数据值。
   */
  void SetEntryInternal(const QString& key, NodeValue::Type type, const QVariant& data);

  /**
   * @brief 存储所有配置项的 QMap。
   * 键是配置项的名称 (QString)，值是包含类型和数据的 ConfigEntry 结构体。
   */
  QMap<QString, ConfigEntry> config_map_;

  /**
   * @brief Config 类的静态单例实例。
   */
  static Config current_config_;

  /**
   * @brief 获取配置文件的完整存储路径。
   * 此路径会考虑应用程序是否处于便携模式。
   * @return QString 配置文件的路径。
   */
  static QString GetConfigFilePath();
};

}  // namespace olive

#endif  // CONFIG_H