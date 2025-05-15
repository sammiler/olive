#ifndef PROJECTSERIALIZER_H // 防止头文件被多次包含的宏定义开始
#define PROJECTSERIALIZER_H

#include <vector> // C++ 标准库向量容器

#include "common/define.h"      // 通用定义，可能包含 DISABLE_COPY_MOVE 等宏
#include "node/project.h"       // 项目类定义，序列化器与项目紧密相关
#include "typeserializer.h"     // 类型序列化器定义，用于处理特定数据类型的序列化/反序列化

// 可能需要的前向声明
// class QHash; // 假设
// class QMap; // 假设
// class Node; // 假设
// class QString; // 假设
// class NodeKeyframe; // 假设
// struct MainWindowLayoutInfo; // 假设
// namespace Node { struct OutputConnection; } // 假设
// class QXmlStreamReader; // 假设
// class QXmlStreamWriter; // 假设
// class QFile; // 假设

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 一个用于序列化/反序列化项目数据的抽象基类。
 * 其目标是将序列化的项目数据与其内存中的表示进一步抽象分离。
 * 这允许多种序列化格式或版本共存。
 */
class ProjectSerializer {
 public:
  /**
   * @brief 定义加载项目数据时的加载类型/范围。
   */
  enum LoadType {
    kProject,        ///< 加载整个项目
    kOnlyNodes,      ///< 仅加载节点
    kOnlyClips,      ///< 仅加载剪辑 (媒体块)
    kOnlyMarkers,    ///< 仅加载标记
    kOnlyKeyframes   ///< 仅加载关键帧
  };

  /** @brief 默认构造函数。 */
  ProjectSerializer() = default;
  /** @brief 默认虚析构函数，允许派生类正确销毁。 */
  virtual ~ProjectSerializer() = default;

  DISABLE_COPY_MOVE(ProjectSerializer) // 禁用拷贝构造和移动构造/赋值操作

  /**
   * @brief 定义加载/保存操作的结果代码。
   */
  enum ResultCode {
    kSuccess,         ///< 操作成功
    kProjectTooOld,   ///< 项目文件版本过旧，无法加载
    kProjectTooNew,   ///< 项目文件版本过新，无法加载
    kUnknownVersion,  ///< 未知的项目文件版本
    kFileError,       ///< 文件读写错误
    kXmlError,        ///< XML 解析错误
    kOverwriteError,  ///< 覆盖文件时发生错误 (例如权限问题)
    kNoData           ///< 没有数据可供加载/保存 (例如，剪贴板为空)
  };

  // 类型别名，用于存储序列化过程中节点的属性键值对
  // QHash<Node *, QMap<QString, QString> > 表示每个节点 (Node*) 对应一个属性名 (QString) 到属性值 (QString) 的映射。
  using SerializedProperties = QHash<Node *, QMap<QString, QString> >;
  // 类型别名，用于存储序列化过程中关键帧数据
  // QHash<QString, QVector<NodeKeyframe *> > 表示每个参数轨道ID (QString) 对应一个关键帧指针的向量。
  using SerializedKeyframes = QHash<QString, QVector<NodeKeyframe *> >;

  /**
   * @brief 存储加载操作结果数据的辅助类。
   */
  class LoadData {
   public:
    /** @brief LoadData 默认构造函数。 */
    LoadData() = default;

    SerializedProperties properties; ///< 加载的节点属性。
    std::vector<TimelineMarker *> markers; ///< 加载的时间线标记。
    SerializedKeyframes keyframes;  ///< 加载的关键帧。
    MainWindowLayoutInfo layout;    ///< 加载的主窗口布局信息。
    QVector<Node *> nodes;          ///< 加载的节点对象列表。
    Node::OutputConnections promised_connections; ///< 加载时“承诺”的节点间连接，可能在 PostLoadEvent 中实际建立。
  };

  /**
   * @brief 封装加载/保存操作结果（包括结果代码和相关数据）的类。
   */
  class Result {
   public:
    /**
     * @brief Result 构造函数。
     * @param code 操作的结果代码。
     */
    explicit Result(const ResultCode &code) : code_(code) {}

    /** @brief 比较 Result 对象是否等于给定的 ResultCode。 */
    bool operator==(const ResultCode &code) { return code_ == code; }
    /** @brief 比较 Result 对象是否不等于给定的 ResultCode。 */
    bool operator!=(const ResultCode &code) { return code_ != code; }

    /** @brief 获取结果代码。 */
    [[nodiscard]] const ResultCode &code() const { return code_; }
    /** @brief 获取操作的详细信息或错误描述。 */
    [[nodiscard]] const QString &GetDetails() const { return details_; }
    /** @brief 设置操作的详细信息。 */
    void SetDetails(const QString &s) { details_ = s; }
    /** @brief 获取加载操作的结果数据。 */
    [[nodiscard]] const LoadData &GetLoadData() const { return load_data_; }
    /** @brief 设置加载操作的结果数据。 */
    void SetLoadData(const LoadData &p) { load_data_ = p; }

   private:
    ResultCode code_;      ///< 存储结果代码。
    QString details_;      ///< 存储详细信息或错误描述。
    LoadData load_data_;   ///< 存储加载的数据。
  };

  /**
   * @brief 存储保存操作所需参数和数据的辅助类。
   */
  class SaveData {
   public:
    /**
     * @brief SaveData 构造函数。
     * @param type 保存类型/范围。
     * @param project (可选) 要保存的项目对象指针。
     * @param filename (可选) 目标文件名。
     */
    explicit SaveData(LoadType type, Project *project = nullptr, const QString &filename = QString()) {
      type_ = type;
      project_ = project;
      filename_ = filename;
    }

    /** @brief 获取要保存的项目对象。 */
    [[nodiscard]] Project *GetProject() const { return project_; }
    /** @brief 设置要保存的项目对象。 */
    void SetProject(Project *p) { project_ = p; }
    /** @brief 获取目标文件名。 */
    [[nodiscard]] const QString &GetFilename() const { return filename_; }
    /** @brief 设置目标文件名。 */
    void SetFilename(const QString &s) { filename_ = s; }
    /** @brief 获取保存类型/范围。 */
    [[nodiscard]] LoadType type() const { return type_; }
    /** @brief 获取要保存的主窗口布局信息。 */
    [[nodiscard]] const MainWindowLayoutInfo &GetLayout() const { return layout_; }
    /** @brief 设置要保存的主窗口布局信息。 */
    void SetLayout(const MainWindowLayoutInfo &layout) { layout_ = layout; }
    /** @brief 获取仅需序列化的特定节点列表 (如果不是保存整个项目)。 */
    [[nodiscard]] const QVector<Node *> &GetOnlySerializeNodes() const { return only_serialize_nodes_; }
    /** @brief 设置仅需序列化的特定节点列表。 */
    void SetOnlySerializeNodes(const QVector<Node *> &only) { only_serialize_nodes_ = only; }
    /** @brief 设置仅需序列化的节点列表，并解析这些节点所属的组。 */
    void SetOnlySerializeNodesAndResolveGroups(QVector<Node *> nodes);
    /** @brief 获取仅需序列化的特定标记列表。 */
    [[nodiscard]] const std::vector<TimelineMarker *> &GetOnlySerializeMarkers() const {
      return only_serialize_markers_;
    }
    /** @brief 设置仅需序列化的特定标记列表。 */
    void SetOnlySerializeMarkers(const std::vector<TimelineMarker *> &only) { only_serialize_markers_ = only; }
    /** @brief 获取仅需序列化的特定关键帧列表。 */
    [[nodiscard]] const std::vector<NodeKeyframe *> &GetOnlySerializeKeyframes() const {
      return only_serialize_keyframes_;
    }
    /** @brief 设置仅需序列化的特定关键帧列表。 */
    void SetOnlySerializeKeyframes(const std::vector<NodeKeyframe *> &only) { only_serialize_keyframes_ = only; }
    /** @brief 获取要保存的节点属性。 */
    [[nodiscard]] const SerializedProperties &GetProperties() const { return properties_; }
    /** @brief 设置要保存的节点属性。 */
    void SetProperties(const SerializedProperties &p) { properties_ = p; }

   private:
    LoadType type_;        ///< 保存类型/范围。
    Project *project_;     ///< 要保存的项目对象。
    QString filename_;     ///< 目标文件名。
    MainWindowLayoutInfo layout_; ///< 主窗口布局信息。
    QVector<Node *> only_serialize_nodes_; ///< 仅序列化的节点列表。
    SerializedProperties properties_;      ///< 节点属性。
    std::vector<TimelineMarker *> only_serialize_markers_; ///< 仅序列化的标记列表。
    std::vector<NodeKeyframe *> only_serialize_keyframes_; ///< 仅序列化的关键帧列表。
  };

  /** @brief (静态方法) 初始化项目序列化器系统 (例如，注册所有可用的序列化器版本)。 */
  static void Initialize();
  /** @brief (静态方法) 销毁或清理项目序列化器系统。 */
  static void Destroy();

  /**
   * @brief (静态方法) 从指定文件加载项目数据。
   * @param project 目标项目对象，加载的数据将被填充到此项目中。
   * @param filename 源文件名。
   * @param load_type 加载类型/范围。
   * @return Result 加载操作的结果。
   */
  static Result Load(Project *project, const QString &filename, LoadType load_type);
  /**
   * @brief (静态方法) 从 QXmlStreamReader 加载项目数据。
   * @param project 目标项目对象。
   * @param reader XML 读取器。
   * @param load_type 加载类型/范围。
   * @return Result 加载操作的结果。
   */
  static Result Load(Project *project, QXmlStreamReader *reader, LoadType load_type);
  /**
   * @brief (静态方法) 从剪贴板或类似来源“粘贴”数据。
   * @param load_type 加载类型/范围。
   * @param project (可选) 目标项目对象。
   * @return Result 粘贴操作的结果。
   */
  static Result Paste(LoadType load_type, Project *project = nullptr);

  /**
   * @brief (静态方法) 根据 SaveData 中的信息保存项目数据到文件。
   * @param data 包含保存所需所有信息的 SaveData 对象。
   * @param compress 是否压缩保存的文件。
   * @return Result 保存操作的结果。
   */
  static Result Save(const SaveData &data, bool compress);
  /**
   * @brief (静态方法) 将项目数据保存到 QXmlStreamWriter。
   * @param writer XML 写入器。
   * @param data 包含保存所需所有信息的 SaveData 对象。
   * @return Result 保存操作的结果。
   */
  static Result Save(QXmlStreamWriter *writer, const SaveData &data);
  /**
   * @brief (静态方法) 将数据“复制”到剪贴板或类似目标。
   * @param data 包含要复制的数据的 SaveData 对象。
   * @return Result 复制操作的结果。
   */
  static Result Copy(const SaveData &data);

  /**
   * @brief (静态方法) 检查给定的文件是否是 Olive 使用的压缩项目文件格式。
   *  通常通过检查文件头部的特定标识符（magic number）来实现。
   * @param file 指向 QFile 对象的指针 (文件必须已打开)。
   * @return 如果是压缩项目文件则返回 true，否则返回 false。
   */
  static bool CheckCompressedID(QFile *file);

 protected:
  /**
   * @brief (纯虚保护函数) 派生类必须实现的加载逻辑。
   * @param project 目标项目。
   * @param reader XML 读取器。
   * @param load_type 加载类型。
   * @param reserved 保留参数，未来可能使用。
   * @return LoadData 加载的数据。
   */
  virtual LoadData Load(Project *project, QXmlStreamReader *reader, LoadType load_type, void *reserved) const = 0;

  /**
   * @brief (虚保护函数) 派生类可以重写的保存逻辑。
   * @param writer XML 写入器。
   * @param data 要保存的数据。
   * @param reserved 保留参数。
   */
  virtual void Save(QXmlStreamWriter *writer, const SaveData &data, void *reserved) const {}

  /**
   * @brief (纯虚保护函数) 派生类必须返回其支持的项目文件版本号。
   * @return uint 版本号。
   */
  [[nodiscard]] virtual uint Version() const = 0;

  /** @brief (静态保护函数) 检查是否有取消加载/保存操作的请求。 */
  [[nodiscard]] static bool IsCancelled();

 private:
  /**
   * @brief (私有静态方法) 根据指定的序列化器版本加载项目数据。
   *  此方法会查找并使用对应版本的 ProjectSerializer 实例。
   * @param version 项目文件版本。
   * @param project 目标项目。
   * @param reader XML 读取器。
   * @param load_type 加载类型。
   * @return Result 加载结果。
   */
  static Result LoadWithSerializerVersion(uint version, Project *project, QXmlStreamReader *reader, LoadType load_type);

  // 存储所有已注册的项目序列化器实例 (不同版本对应不同实例)。
  static QVector<ProjectSerializer *> instances_; ///< 所有序列化器实例的列表。
};

}  // namespace olive

#endif  // PROJECTSERIALIZER_H // 头文件宏定义结束