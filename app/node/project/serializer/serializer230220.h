#ifndef PROJECTSERIALIZER230220_H // 防止头文件被多次包含的宏定义开始
#define PROJECTSERIALIZER230220_H

#include "serializer.h" // 引入基类 ProjectSerializer 的定义 (文件名可能是 "projectserializer.h")

// 可能需要的前向声明
// class Project; // 假设
// class QXmlStreamReader; // 假设
// enum class LoadType; // 假设 LoadType 枚举在 ProjectSerializer 中定义
// class LoadData; // 假设 LoadData 类在 ProjectSerializer 中定义
// class QXmlStreamWriter; // 假设
// class SaveData; // 假设 SaveData 类在 ProjectSerializer 中定义
// class Node; // 假设
// class SerializedData; // 假设

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 特定版本的项目序列化器，版本号为 230220。
 * 这个类继承自 ProjectSerializer，并实现了针对版本号为 230220 的项目文件的
 * 具体加载和保存逻辑。与之前的版本相比，一个显著变化是此版本也重写了 `Save` 方法，
 * 表明它负责处理此特定格式的写入操作。同时，内部用于暂存加载数据的 `XMLNodeData` 结构体
 * 在此头文件中不再可见，暗示加载和连接过程可能有所调整。
 */
class ProjectSerializer230220 : public ProjectSerializer {
 public:
  /**
   * @brief ProjectSerializer230220 默认构造函数。
   */
  ProjectSerializer230220() = default;

 protected:
  /**
   * @brief 实现从 XML 流加载版本号为 230220 的项目数据的具体逻辑。
   *  重写自 ProjectSerializer 基类。
   * @param project 目标项目对象，加载的数据将被填充到此项目中。
   * @param reader 指向 QXmlStreamReader 的指针，用于读取 XML 数据。
   * @param load_type 加载类型/范围 (例如，加载整个项目、仅节点等)。
   * @param reserved 保留参数，未来可能使用，当前未使用。
   * @return LoadData 包含从 XML 中解析出的项目数据的 LoadData 对象。
   */
  LoadData Load(Project *project, QXmlStreamReader *reader, LoadType load_type, void *reserved) const override;

  /**
   * @brief 实现将项目数据保存为版本号 230220 格式的 XML 流的具体逻辑。
   *  重写自 ProjectSerializer 基类。这是之前版本序列化器未明确重写的方法。
   * @param writer 指向 QXmlStreamWriter 的指针，用于写入 XML 数据。
   * @param data 包含要保存的数据和相关设置的 SaveData 对象。
   * @param reserved 保留参数，未来可能使用，当前未使用。
   */
  void Save(QXmlStreamWriter *writer, const SaveData &data, void *reserved) const override;

  /**
   * @brief 返回此序列化器支持的项目文件版本号。
   * @return uint 版本号，固定为 230220。
   */
  [[nodiscard]] uint Version() const override { return 230220; } // 此序列化器处理版本 230220

 private:
  /**
   * @brief (私有静态成员函数) 在所有节点及其基本属性加载完毕后，建立节点之间的连接。
   *  此版本的 PostConnect 签名与之前版本不同，直接接收已加载的节点列表和项目数据。
   * @param nodes 已加载的 Node 对象指针的向量。
   * @param project_data 指向 SerializedData 的指针，可能包含在加载过程中收集的、用于建立连接的辅助信息。
   */
  static void PostConnect(const QVector<Node *> &nodes, SerializedData *project_data);
};

}  // namespace olive

#endif  // PROJECTSERIALIZER230220_H // 头文件宏定义结束