#ifndef PROJECTSERIALIZER190219_H // 防止头文件被多次包含的宏定义开始
#define PROJECTSERIALIZER190219_H

#include "serializer.h" // 引入基类 ProjectSerializer 的定义 (文件名可能是 "projectserializer.h")

// 可能需要的前向声明
// class Project; // 假设
// class QXmlStreamReader; // 假设
// enum class LoadType; // 假设 LoadType 枚举在 ProjectSerializer 中定义
// class LoadData; // 假设 LoadData 类在 ProjectSerializer 中定义

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 特定版本的项目序列化器，版本号为 190219。
 * 这个类继承自 ProjectSerializer，并实现了针对版本号为 190219 的项目文件的
 * 具体加载逻辑。版本号通常代表了项目文件格式发生变化的时间点或迭代。
 */
class ProjectSerializer190219 : public ProjectSerializer {
public:
  /**
   * @brief ProjectSerializer190219 默认构造函数。
   */
  ProjectSerializer190219() = default;

protected:
  /**
   * @brief 实现从 XML 流加载版本号为 190219 的项目数据的具体逻辑。
   *  重写自 ProjectSerializer 基类。
   * @param project 目标项目对象，加载的数据将被填充到此项目中。
   * @param reader 指向 QXmlStreamReader 的指针，用于读取 XML 数据。
   * @param load_type 加载类型/范围 (例如，加载整个项目、仅节点、仅剪辑等)。
   * @param reserved 保留参数，未来可能使用，当前未使用。
   * @return LoadData 包含从 XML 中解析出的项目数据的 LoadData 对象。
   */
  LoadData Load(Project *project, QXmlStreamReader *reader, LoadType load_type, void *reserved) const override;

  /**
   * @brief 返回此序列化器支持的项目文件版本号。
   * @return uint 版本号，固定为 190219。
   */
  [[nodiscard]] uint Version() const override { return 190219; } // 此序列化器处理版本 190219
};

}  // namespace olive

#endif  // SERIALIZER190219_H // 头文件宏定义结束 (注意：此处的宏名称与文件名不完全匹配，可能是笔误或特定命名约定)