#ifndef MULTICAMNODE_H  // 防止头文件被多次包含的宏定义开始
#define MULTICAMNODE_H

#include "node/node.h"                    // 引入基类 Node 的定义
#include "node/output/track/tracklist.h"  // 引入 TrackList 定义，多机位可能与轨道列表交互

// 前向声明
// class Sequence; // 假设 Sequence 类定义
// struct ActiveElements; // 假设
// struct NodeValueRow; // 假设
// struct NodeGlobals; // 假设
// class NodeValueTable; // 假设
// class TimeRange; // 假设

namespace olive {  // Olive 编辑器的命名空间

class Sequence;  // 前向声明 Sequence 类，多机位节点通常在序列环境中使用

/**
 * @brief 代表“多机位编辑”功能的节点。
 * 该节点允用户将多个视频源（机位）汇集到一起，并在播放时实时切换显示不同的机位。
 * 它通常用于处理多摄像机拍摄的素材，方便进行同步和剪辑。
 */
class MultiCamNode : public Node {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief MultiCamNode 构造函数。
      */
     MultiCamNode();

  NODE_DEFAULT_FUNCTIONS(MultiCamNode)  // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此多机位节点的名称。
   * @return QString 类型的节点名称 (例如 "多机位" 或 "MultiCam")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此多机位节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点可能属于 "输入" (Input) 或 "特殊" (Special) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此多机位节点的描述信息。
   * @return QString 类型的节点描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 获取在指定时间范围内，特定输入端口上的活动元素。
   *  对于多机位节点，这可能涉及到确定在给定时间哪个源是活动的。
   * @param input 输入端口的名称。
   * @param r 时间范围。
   * @return ActiveElements 描述活动元素信息的对象。
   */
  [[nodiscard]] ActiveElements GetActiveElementsAtTime(const QString &input, const TimeRange &r) const override;

  /**
   * @brief 计算并输出节点在特定时间点的值（通常是当前选定机位的图像数据，或所有机位的预览网格）。
   * @param value 当前输入行数据 (包含当前选定机位索引等参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的图像)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述、参数名)。
   */
  void Retranslate() override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kCurrentInput;  ///< "Current" - 当前选定的源（机位）索引的参数键名。
  static const QString
      kSourcesInput;  ///< "Sources" - 连接多个视频源（机位）的输入端口（通常是一个数组或可变数量的端口）的键名。
  static const QString kSequenceInput;  ///< "Sequence" - 关联的序列对象的参数键名 (如果需要从序列获取信息)。
  static const QString
      kSequenceTypeInput;  ///< "SequenceType" - 序列类型（如视频、音频）的参数键名，可能影响多机位处理方式。

  /**
   * @brief 获取当前选定的源（机位）的索引。
   * @return int 当前源的索引。
   */
  [[nodiscard]] int GetCurrentSource() const { return GetStandardValue(kCurrentInput).toInt(); }

  /**
   * @brief 获取连接到此多机位节点的源（机位）的总数量。
   * @return int 源的数量。
   */
  [[nodiscard]] int GetSourceCount() const;

  /**
   * @brief (静态工具函数) 根据源的数量计算在预览网格中显示的行数和列数。
   * @param sources 源的总数量。
   * @param rows (输出参数) 指向存储计算得到的行数的指针。
   * @param cols (输出参数) 指向存储计算得到的列数的指针。
   */
  static void GetRowsAndColumns(int sources, int *rows, int *cols);
  /**
   * @brief 获取当前多机位节点预览网格的行数和列数。
   * @param rows (输出参数) 指向存储计算得到的行数的指针。
   * @param cols (输出参数) 指向存储计算得到的列数的指针。
   */
  void GetRowsAndColumns(int *rows, int *cols) const { return GetRowsAndColumns(GetSourceCount(), rows, cols); }

  /**
   * @brief 设置与此多机位节点相关的序列类型。
   * @param t Track::Type 枚举值，表示序列类型 (例如视频轨道、音频轨道)。
   */
  void SetSequenceType(Track::Type t) {
    SetStandardValue(kSequenceTypeInput, static_cast<int>(t));
  }  // 假设 Track::Type 可以转为 int

  /**
   * @brief (静态工具函数) 根据源的索引以及总行数和总列数，计算该源在预览网格中的行号和列号。
   * @param index 源的索引 (从0开始)。
   * @param total_rows 网格的总行数。
   * @param total_cols 网格的总列数。
   * @param row (输出参数) 指向存储计算得到的行号的指针。
   * @param col (输出参数) 指向存储计算得到的列号的指针。
   */
  static void IndexToRowCols(int index, int total_rows, int total_cols, int *row, int *col);

  /**
   * @brief (静态工具函数) 根据源在预览网格中的行号和列号，以及总行数和总列数，计算该源的索引。
   * @param row 源的行号 (从0开始)。
   * @param col 源的列号 (从0开始)。
   * @param total_rows 网格的总行数。
   * @param total_cols 网格的总列数。
   * @return int 源的索引。
   */
  static int RowsColsToIndex(int row, int col, int total_rows, int total_cols) { return col + row * total_cols; }

  /**
   * @brief 获取连接到指定输入端口（用于渲染）的输出节点。
   *  重写基类方法，可能对多机位节点的特定输入（如 kSourcesInput）有特殊处理。
   * @param input 输入端口的名称。
   * @param element 输入端口的元素索引 (例如，对于 kSourcesInput 数组中的某个特定源)。
   * @return Node* 指向连接的输出节点的指针。
   */
  [[nodiscard]] Node *GetConnectedRenderOutput(const QString &input, int element = -1) const override;
  /**
   * @brief 检查指定的输入端口（用于渲染）是否已连接。
   * @param input 输入端口的名称。
   * @param element 输入端口的元素索引。
   * @return 如果已连接则返回 true，否则返回 false。
   */
  [[nodiscard]] bool IsInputConnectedForRender(const QString &input, int element = -1) const override;

  /**
   * @brief 获取在渲染时应忽略的输入端口列表。
   *  对于多机位节点，某些输入可能仅用于控制或元数据，而不直接参与最终图像的生成。
   * @return QVector<QString> 包含应忽略的输入端口名称的列表。
   */
  [[nodiscard]] QVector<QString> IgnoreInputsForRendering() const override;

 protected:
  /**
   * @brief 当节点的某个输入端口连接上一个输出节点时调用的事件处理函数。
   * @param input 连接的输入端口名称。
   * @param element 输入端口的元素索引。
   * @param output 连接到的输出节点。
   */
  void InputConnectedEvent(const QString &input, int element, Node *output) override;
  /**
   * @brief 当节点的某个输入端口断开连接时调用的事件处理函数。
   * @param input 断开连接的输入端口名称。
   * @param element 输入端口的元素索引。
   * @param output 之前连接的输出节点。
   */
  void InputDisconnectedEvent(const QString &input, int element, Node *output) override;

 private:
  /**
   * @brief 获取与此多机位节点关联的轨道列表。
   *  可能用于访问源剪辑所在的轨道信息。
   * @return TrackList* 指向 TrackList 对象的指针。
   */
  [[nodiscard]] TrackList *GetTrackList() const;

  Sequence *sequence_;  ///< 指向与此多机位节点关联的 Sequence 对象的指针。
};

}  // namespace olive

#endif  // MULTICAMNODE_H // 头文件宏定义结束