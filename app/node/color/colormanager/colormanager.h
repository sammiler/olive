#ifndef COLORSERVICE_H
#define COLORSERVICE_H

#include <QMutex>       // Qt 互斥锁，可能用于保护共享资源（如此头文件中未直接使用，可能在 .cpp 文件中）
#include <memory>       // C++11 智能指针库

#include "codec/frame.h"             // 可能包含帧数据结构，色彩管理会作用于帧数据
#include "node/node.h"               // 基类 Node，ColorManager 可能作为一种服务节点或与之交互
#include "render/colorprocessor.h"   // 可能包含颜色处理相关的接口或类


class Config; // 假设 OCIO::Config 的声明，实际可能在 OCIO 的头文件中
using ConstConfigRcPtr = std::shared_ptr<const Config>; // 模拟 OCIO 的引用计数常量配置指针类型


// 前向声明 Project 类
namespace olive {
class Project;
}


namespace olive {

/**
 * @brief 色彩管理器类，负责处理项目中的所有色彩管理任务。
 * 通常使用 OpenColorIO (OCIO) 配置来实现色彩空间转换、显示映射等。
 */
class ColorManager : public QObject {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief ColorManager 构造函数。
   * @param project 指向当前项目的指针，色彩管理设置通常与项目关联。
   */
  explicit ColorManager(Project* project);

  /**
   * @brief 初始化色彩管理器。
   * 可能包括加载默认配置或项目特定配置。
   */
  void Init();

  /**
   * @brief 获取当前加载的 OCIO 配置。
   * @return OCIO::ConstConfigRcPtr 类型的常量 OCIO 配置对象的智能指针。
   */
  [[nodiscard]] OCIO::ConstConfigRcPtr GetConfig() const;

  /**
   * @brief 从指定的 OCIO 配置文件创建并加载一个 OCIO 配置。
   * @param filename OCIO 配置文件的路径。
   * @return OCIO::ConstConfigRcPtr 类型的常量 OCIO 配置对象的智能指针。如果加载失败则可能为空。
   */
  static OCIO::ConstConfigRcPtr CreateConfigFromFile(const QString& filename);

  /**
   * @brief 获取当前使用的 OCIO 配置文件的路径。
   * @return QString 类型的文件路径。
   */
  [[nodiscard]] QString GetConfigFilename() const;

  /**
   * @brief 获取默认的 OCIO 配置。
   * 这可能是一个内置的、标准的 OCIO 配置。
   * @return OCIO::ConstConfigRcPtr 类型的常量 OCIO 配置对象的智能指针。
   */
  static OCIO::ConstConfigRcPtr GetDefaultConfig();

  /**
   * @brief 设置并初始化默认的 OCIO 配置。
   * 通常在应用程序启动时调用。
   */
  static void SetUpDefaultConfig();

  /**
   * @brief 设置要使用的 OCIO 配置文件名，并尝试加载该配置。
   * @param filename OCIO 配置文件的路径。此操作会触发 ConfigChanged 信号。
   */
  void SetConfigFilename(const QString& filename) const; // const 标记可能意味着它修改的是可变成员，或者通过信号间接触发非 const 操作

  /**
   * @brief 列出当前 OCIO 配置中可用的显示设备名称。
   * @return QStringList 包含显示设备名称的列表。
   */
  QStringList ListAvailableDisplays();

  /**
   * @brief 获取当前 OCIO 配置中定义的默认显示设备名称。
   * @return QString 类型的默认显示设备名称。
   */
  QString GetDefaultDisplay();

  /**
   * @brief 根据给定的显示设备，列出当前 OCIO 配置中可用的视图名称。
   * @param display 显示设备的名称。
   * @return QStringList 包含该显示设备可用视图名称的列表。
   */
  QStringList ListAvailableViews(const QString& display);

  /**
   * @brief 根据给定的显示设备，获取当前 OCIO 配置中定义的默认视图名称。
   * @param display 显示设备的名称。
   * @return QString 类型的默认视图名称。
   */
  QString GetDefaultView(const QString& display);

  /**
   * @brief 列出当前 OCIO 配置中可用的 "Looks" (风格化调色预设)。
   * @return QStringList 包含可用 Looks 名称的列表。
   */
  QStringList ListAvailableLooks();

  /**
   * @brief 列出当前 OCIO 配置中所有可用的色彩空间名称。
   * @return QStringList 包含色彩空间名称的列表。
   */
  [[nodiscard]] QStringList ListAvailableColorspaces() const;

  /**
   * @brief 获取项目设置的默认输入色彩空间。
   * 当导入新的媒体文件时，如果文件本身没有色彩空间信息，会使用此默认值。
   * @return QString 类型的默认输入色彩空间名称。
   */
  [[nodiscard]] QString GetDefaultInputColorSpace() const;

  /**
   * @brief 设置项目的默认输入色彩空间。
   * @param s 要设置的色彩空间名称。会触发 DefaultInputChanged 信号。
   */
  void SetDefaultInputColorSpace(const QString& s) const; // const 标记原因同 SetConfigFilename

  /**
   * @brief 获取项目的参考/工作色彩空间。
   * 这是项目内部进行色彩计算和混合时使用的色彩空间。
   * @return QString 类型的参考色彩空间名称。
   */
  [[nodiscard]] QString GetReferenceColorSpace() const;

  /**
   * @brief 获取与给定色彩空间名称相符（或最接近）的、在当前 OCIO 配置中定义的色彩空间名称。
   * 用于确保色彩空间名称在当前配置中的有效性。
   * @param s 输入的色彩空间名称。
   * @return QString 类型的有效色彩空间名称。
   */
  [[nodiscard]] QString GetCompliantColorSpace(const QString& s) const;

  /**
   * @brief 获取一个确保与当前显示设置兼容的色彩变换。
   * 如果原始变换的目标色彩空间不适合直接显示，此方法可能会调整变换以匹配显示视图。
   * @param transform 原始的 ColorTransform 对象。
   * @param force_display 是否强制转换到显示色彩空间。
   * @return 调整后的 ColorTransform 对象。
   */
  ColorTransform GetCompliantColorSpace(const ColorTransform& transform, bool force_display = false);

  /**
   * @brief (静态方法) 列出给定 OCIO 配置中所有可用的色彩空间名称。
   * @param config 一个 OCIO 配置对象的智能指针。
   * @return QStringList 包含色彩空间名称的列表。
   */
  static QStringList ListAvailableColorspaces(const OCIO::ConstConfigRcPtr& config);

  /**
   * @brief 获取默认的亮度系数 (luma coefficients)。
   * 这些系数用于从 RGB 计算亮度值，例如在 YUV 转换或灰度化时。
   * @param rgb 一个 double 数组 (至少3个元素)，用于存储 R, G, B 各通道的亮度系数。
   */
  void GetDefaultLumaCoefs(double* rgb) const;

  /**
   * @brief 获取与此色彩管理器关联的项目对象。
   * @return Project* 指向项目对象的指针。
   */
  [[nodiscard]] Project* project() const;

  /**
   * @brief 根据之前设置的配置文件名，重新加载并更新 OCIO 配置。
   * 当外部 OCIO 配置文件发生变化时，可以调用此方法。
   */
  void UpdateConfigFromFilename();

 signals: // Qt 信号声明区域
  /**
   * @brief 当 OCIO 配置文件发生改变时发射此信号。
   * @param s 新的配置文件路径或标识。
   */
  void ConfigChanged(const QString& s);

  /**
   * @brief 当项目的参考色彩空间发生改变时发射此信号。
   * @param s 新的参考色彩空间名称。
   */
  void ReferenceSpaceChanged(const QString& s);

  /**
   * @brief 当默认输入色彩空间发生改变时发射此信号。
   * @param s 新的默认输入色彩空间名称。
   */
  void DefaultInputChanged(const QString& s);

 private:
  OCIO::ConstConfigRcPtr config_; ///< 当前项目加载的 OCIO 配置对象的智能指针。

  static OCIO::ConstConfigRcPtr default_config_; ///< 静态成员，存储全局默认的 OCIO 配置。
};

}  // namespace olive

#endif  // COLORSERVICE_H