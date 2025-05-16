#ifndef SCOPE_PANEL_H  // 防止头文件被重复包含的宏
#define SCOPE_PANEL_H  // 定义 SCOPE_PANEL_H 宏

#include <QComboBox>       // Qt 下拉框控件
#include <QStackedWidget>  // Qt 堆叠窗口控件 (用于在同一区域切换不同视图)

#include "panel/panel.h"                       // 包含 PanelWidget 基类的定义
#include "panel/viewer/viewerbase.h"           // 包含 ViewerPanelBase (查看器面板基类) 的定义
#include "widget/scope/histogram/histogram.h"  // 包含直方图示波器控件 (HistogramScope) 的定义
#include "widget/scope/waveform/waveform.h"    // 包含波形示波器控件 (WaveformScope) 的定义

namespace olive {  // olive 项目的命名空间

/**
 * @brief ScopePanel 类代表一个用于显示视频分析示波器 (如波形图、直方图) 的面板。
 *
 * 它继承自 PanelWidget，是 Olive 编辑器中的一个标准面板。
 * 这个面板允许用户选择不同类型的示波器来分析当前查看器中视频帧的亮度、颜色分布等信息，
 * 以辅助进行色彩校正和曝光调整等工作。
 */
class ScopePanel : public PanelWidget {  // ScopePanel 继承自 PanelWidget
 Q_OBJECT                                // 声明此类使用 Qt 的元对象系统

     public :
     // 定义示波器类型的枚举
     enum Type {
       kTypeWaveform,   // 波形示波器类型
       kTypeHistogram,  // 直方图示波器类型

       kTypeCount  // 示波器类型总数 (用于迭代或数组大小)
     };

  // 构造函数
  ScopePanel();

  /**
   * @brief 设置当前面板显示的示波器类型。
   * @param t 要设置的示波器类型 (ScopePanel::Type)。
   */
  void SetType(Type t);

  /**
   * @brief (静态) 将示波器类型枚举值转换为用户可读的名称字符串。
   * @param t 示波器类型枚举值。
   * @return 返回对应的名称字符串 (例如 "Waveform", "Histogram")。
   */
  static QString TypeToName(Type t);

  /**
   * @brief 设置此示波器面板关联的查看器面板。
   * 示波器通常需要从一个查看器获取视频帧数据进行分析。
   * @param vp 指向 ViewerPanelBase 的指针。
   */
  void SetViewerPanel(ViewerPanelBase* vp);

  /**
   * @brief 获取当前与此示波器面板连接的查看器面板。
   * @return 返回连接的 ViewerPanelBase 指针，如果没有连接则返回 nullptr。
   */
  [[nodiscard]] ViewerPanelBase* GetConnectedViewerPanel() const { return viewer_; }

 public slots:  // Qt 公有槽函数
  /**
   * @brief 设置示波器用于分析的参考视频帧 (纹理)。
   * 此槽通常由关联的查看器在帧更新时调用。
   * @param frame 包含视频帧数据的 TexturePtr。
   */
  void SetReferenceBuffer(const TexturePtr& frame);

  /**
   * @brief 设置示波器使用的色彩管理器。
   * 色彩管理器用于处理色彩空间转换等。
   * @param manager 指向 ColorManager 的指针。
   */
  void SetColorManager(ColorManager* manager);

 protected:
  /**
   * @brief 重写基类的 Retranslate 方法，用于在语言更改时更新此面板内UI元素的文本。
   * 例如，可能会更新面板标题、下拉框中的示波器类型名称等。
   */
  void Retranslate() override;

 private:
  Type type_;  // 当前选择的示波器类型

  QStackedWidget* stack_;  // 堆叠控件，用于在波形图和直方图等不同示波器视图之间切换

  QComboBox* scope_type_combobox_;  // 下拉框控件，用于用户选择要显示的示波器类型

  WaveformScope* waveform_view_;  // 指向波形示波器控件的指针

  HistogramScope* histogram_;  // 指向直方图示波器控件的指针

  ViewerPanelBase* viewer_;  // 指向关联的查看器面板的指针，示波器从此获取数据
};

}  // namespace olive

#endif  // SCOPE_PANEL_H