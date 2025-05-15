#ifndef PROJECT_H // 防止头文件被重复包含的宏
#define PROJECT_H // 定义 PROJECT_H 宏

#include <QObject>   // Qt 对象模型基类
#include <QUuid>     // Qt 通用唯一标识符类
#include <memory>    // 标准库内存管理头文件 (例如 std::unique_ptr, std::shared_ptr)

#include "node/color/colormanager/colormanager.h" // 色彩管理器类
#include "node/output/viewer/viewer.h"           // 查看器节点类 (可能用于预览)
#include "node/project/footage/footage.h"         // 素材/片段节点类
#include "window/mainwindow/mainwindowlayoutinfo.h" // 主窗口布局信息 (可能用于保存/加载窗口状态)

namespace olive { // olive 项目的命名空间

/**
 * @brief Project 类代表一个项目实例，包含与用户项目相关的所有数据。
 *
 * 项目实例使用 Item 对象的父子层级结构。项目通常包含以下内容：
 *
 * * 素材 (Footage)
 * * 序列 (Sequences)
 * * 文件夹 (Folders)
 * * 项目设置 (Project Settings)
 * * 窗口布局 (Window Layout)
 */
class Project : public QObject { // Project 类继承自 QObject 以使用 Qt 的元对象系统
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  // 缓存设置的枚举类型
  enum CacheSetting {
    kCacheUseDefaultLocation,    // 使用默认缓存位置
    kCacheStoreAlongsideProject, // 缓存存储在项目文件旁边
    kCacheCustomPath             // 使用自定义缓存路径
  };

  // 构造函数
  Project();

  // 析构函数
  ~Project() override;

  /**
   * @brief 彻底销毁图中所有的节点。
   * 这是一个破坏性操作。
   */
  void Clear();

  /**
   * @brief 获取属于此图 (项目) 的所有节点的完整列表。
   * @return 返回一个包含 Node 指针的 QVector 的常量引用。
   */
  [[nodiscard]] const QVector<Node *> &nodes() const { return node_children_; }

  // 初始化项目 (可能在创建或加载后调用)
  void Initialize();

  // 从 XML 流加载项目数据
  // @param reader XML 流读取器。
  // @return 返回 SerializedData 结构体，可能包含加载过程中的额外信息。
  SerializedData Load(QXmlStreamReader *reader);
  // 将项目数据保存到 XML 流
  // @param writer XML 流写入器。
  void Save(QXmlStreamWriter *writer) const;

  // 获取一个节点存在于多少个上下文 (其他节点的内部图) 中
  // @param node 要检查的节点。
  // @param except_itself 如果为 true，则不计算节点自身作为上下文的情况。
  // @return 返回节点所在的上下文数量。
  int GetNumberOfContextsNodeIsIn(Node *node, bool except_itself = false) const;

  // 获取项目名称 (通常从文件名派生)
  [[nodiscard]] QString name() const;

  // 获取项目文件的完整路径和名称
  [[nodiscard]] const QString &filename() const;
  // 获取项目文件的“美化”名称 (例如，仅文件名部分)
  [[nodiscard]] QString pretty_filename() const;
  // 设置项目的文件名 (路径)
  void set_filename(const QString &s);

  // 获取项目的根文件夹
  [[nodiscard]] Folder *root() const { return root_; }
  // 获取项目的色彩管理器
  [[nodiscard]] ColorManager *color_manager() const { return color_manager_; }

  // 检查项目是否已被修改 (自上次保存以来)
  [[nodiscard]] bool is_modified() const { return is_modified_; }
  // 设置项目的修改状态
  void set_modified(bool e);

  // 检查自动恢复文件是否已被保存
  [[nodiscard]] bool has_autorecovery_been_saved() const;
  // 设置自动恢复文件的保存状态
  void set_autorecovery_saved(bool e);

  // 检查项目是否为新创建的 (尚未保存过)
  [[nodiscard]] bool is_new() const;

  // 获取当缓存设置是“存储在项目旁边”时的缓存路径
  [[nodiscard]] QString get_cache_alongside_project_path() const;
  // 获取当前生效的缓存路径 (根据缓存设置决定)
  [[nodiscard]] QString cache_path() const;

  // 获取项目的唯一标识符 (UUID)
  [[nodiscard]] const QUuid &GetUuid() const { return uuid_; }

  // 设置项目的 UUID
  void SetUuid(const QUuid &uuid) { uuid_ = uuid; }

  // 重新生成项目的 UUID (例如，当项目被另存为时)
  void RegenerateUuid();

  /**
   * @brief 返回项目上次保存时的文件名 (URL/路径)，不一定是当前位置。
   *
   * 可能有助于解析相对路径。
   * @return 返回上次保存的 URL 字符串。
   */
  [[nodiscard]] const QString &GetSavedURL() const { return saved_url_; }

  // 设置项目上次保存的 URL
  void SetSavedURL(const QString &url) { saved_url_ = url; }

  /**
   * @brief 从一个 QObject 对象查找其所属的 Project 父对象。
   *
   * 如果一个对象预期是项目的子对象，此函数将遍历其父对象树直到找到 Project。
   * @param o 要查找其项目的 QObject 对象。
   * @return 如果找到，返回 Project 指针；否则返回 nullptr。
   */
  static Project *GetProjectFromObject(const QObject *o);

  // 静态方法：从一个项目复制设置到另一个项目
  static void CopySettings(Project *from, Project *to) { to->settings_ = from->settings_; }

  static const QString kItemMimeType; // 用于拖放操作的 Item 的 MIME 类型字符串常量

  // 项目设置相关的键名常量
  static const QString kCacheLocationSettingKey; // 缓存位置设置的键
  static const QString kCachePathKey;            // 自定义缓存路径设置的键
  static const QString kColorConfigFilename;     // OCIO 颜色配置文件名的键
  static const QString kColorReferenceSpace;     // 颜色参考空间的键 (例如场景线性)
  static const QString kDefaultInputColorSpaceKey; // 默认输入颜色空间的键
  static const QString kRootKey;                 // XML 中根元素或项目根节点的键 (可能)

  // 获取指定键的项目设置值
  [[nodiscard]] QString GetSetting(const QString &key) const { return settings_.value(key); }
  // 设置指定键的项目设置值
  void SetSetting(const QString &key, const QString &value);

  // 获取缓存位置设置 (枚举值)
  [[nodiscard]] CacheSetting GetCacheLocationSetting() const {
    return static_cast<CacheSetting>(GetSetting(kCacheLocationSettingKey).toInt());
  }
  // 设置缓存位置设置
  void SetCacheLocationSetting(CacheSetting s) { SetSetting(kCacheLocationSettingKey, QString::number(s)); }

  // 获取自定义缓存路径
  [[nodiscard]] QString GetCustomCachePath() const { return GetSetting(kCachePathKey); }
  // 设置自定义缓存路径
  void SetCustomCachePath(const QString &path) { SetSetting(kCachePathKey, path); }

  // 获取 OCIO 颜色配置文件名
  [[nodiscard]] QString GetColorConfigFilename() const { return GetSetting(kColorConfigFilename); }
  // 设置 OCIO 颜色配置文件名
  void SetColorConfigFilename(const QString &s) { SetSetting(kColorConfigFilename, s); }

  // 获取默认输入颜色空间
  [[nodiscard]] QString GetDefaultInputColorSpace() const { return GetSetting(kDefaultInputColorSpaceKey); }
  // 设置默认输入颜色空间
  void SetDefaultInputColorSpace(const QString &s) { SetSetting(kDefaultInputColorSpaceKey, s); }

  // 获取颜色参考空间 (例如 "scene_linear")
  [[nodiscard]] QString GetColorReferenceSpace() const { return GetSetting(kColorReferenceSpace); }
  // 设置颜色参考空间
  void SetColorReferenceSpace(const QString &s) { SetSetting(kColorReferenceSpace, s); }

 signals: // Qt 信号声明
  // 项目名称改变时发出的信号
  void NameChanged();

  // 项目修改状态改变时发出的信号
  void ModifiedChanged(bool e);

  /**
   * @brief 当一个节点被添加到图 (项目) 时发出的信号。
   * @param node 被添加的节点。
   */
  void NodeAdded(Node *node);

  /**
   * @brief 当一个节点从图 (项目) 中移除时发出的信号。
   * @param node 被移除的节点。
   */
  void NodeRemoved(Node *node);

  // 当一个输入端口被连接时发出的信号 (通常由 Node 自身发出，Project 可能在此处转发或监听)
  void InputConnected(Node *output, const NodeInput &input);

  // 当一个输入端口断开连接时发出的信号
  void InputDisconnected(Node *output, const NodeInput &input);

  // 当某个节点输入的值改变时发出的信号
  void ValueChanged(const NodeInput &input);

  // 当某个节点输入的值提示 (ValueHint) 改变时发出的信号
  void InputValueHintChanged(const NodeInput &input);

  // 当节点组 (NodeGroup) 添加了输入透传时发出的信号
  void GroupAddedInputPassthrough(NodeGroup *group, const NodeInput &input);

  // 当节点组移除了输入透传时发出的信号
  void GroupRemovedInputPassthrough(NodeGroup *group, const NodeInput &input);

  // 当节点组的输出透传改变时发出的信号
  void GroupChangedOutputPassthrough(NodeGroup *group, Node *output);

  // 当项目设置改变时发出的信号
  void SettingChanged(const QString &key, const QString &value);

 protected:
  // QObject 的子对象事件处理 (用于跟踪 Node 子对象的添加和移除)
  void childEvent(QChildEvent *event) override;

 private:
  QUuid uuid_; // 项目的通用唯一标识符

  Folder *root_; // 项目的根文件夹，所有项目内容都组织在其下

  QString filename_; // 当前项目文件的完整路径和名称

  QString saved_url_; // 项目上次保存时的 URL/路径

  bool is_modified_; // 标记项目是否自上次保存后被修改

  bool autorecovery_saved_; // 标记自动恢复信息是否已保存

  ColorManager *color_manager_; // 项目的色彩管理器实例

  QVector<Node *> node_children_; // 直接属于项目 (不在特定文件夹或序列中) 的节点列表，
                                 // 或者所有节点的扁平列表 (取决于具体实现)

  QMap<QString, QString> settings_; // 存储项目级别的各种设置 (键值对)
};

}  // namespace olive

#endif  // PROJECT_H