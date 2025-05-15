#ifndef SEQUENCEVIEWERPANEL_H // 防止头文件被重复包含的宏
#define SEQUENCEVIEWERPANEL_H // 定义 SEQUENCEVIEWERPANEL_H 宏

#include "panel/viewer/viewer.h" // 包含 ViewerPanel 基类的定义 (ViewerPanel 自身可能继承自 ViewerPanelBase 或类似类)

namespace olive { // olive 项目的命名空间

/**
 * @brief SequenceViewerPanel 类代表一个专门用于显示和预览序列 (Sequence) 输出的查看器面板。
 *
 * 它继承自 ViewerPanel，这意味着它拥有通用的查看器功能，如播放控制、时间导航、
 * 帧显示等。与 FootageViewerPanel (素材查看器) 不同，SequenceViewerPanel
 * 通常连接到项目中的一个序列节点，显示该序列在特定时间点经过所有效果和编辑处理后的最终结果。
 *
 * 这个面板是用户预览其最终影片的主要窗口。
 */
class SequenceViewerPanel : public ViewerPanel { // SequenceViewerPanel 继承自 ViewerPanel
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  // 构造函数
  SequenceViewerPanel();

public slots: // Qt 公有槽函数
 /**
  * @brief 开始捕获 (或跳转到并准备处理) 指定时间范围和轨道的特定内容。
  * 这个槽函数的功能可能与时间轴和序列编辑器的交互有关，例如，
  * 当用户在时间轴上选择某个片段并希望在查看器中进行特定操作 (如拖拽、插入覆盖) 时，
  * 可能会调用此方法来准备查看器状态。
  * "Capture" 在这里可能指准备进行某种形式的编辑交互，而不仅仅是录制。
  *
  * @param time 要关注的时间范围。
  * @param track 相关的轨道引用 (Track::Reference，可能包含轨道类型和索引等信息)。
  */
 void StartCapture(const TimeRange &time, const Track::Reference &track);

protected:
  /**
   * @brief 重写基类的 Retranslate 方法，用于在语言更改时更新此面板内UI元素的文本。
   * 例如，可能会更新面板标题或内部查看器控件的标签。
   */
  void Retranslate() override;

  // 注意：实际的查看器核心控件 (例如 ViewerWidget 的某个派生类) 通常由 ViewerPanel 基类
  // 或其父类进行管理和创建。此派生类主要负责特定于序列预览的逻辑和交互。
};

}  // namespace olive

#endif  // SEQUENCEVIEWERPANEL_H