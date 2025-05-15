#ifndef VIEWERPANELBASE_H
#define VIEWERPANELBASE_H

#include "panel/pixelsampler/pixelsamplerpanel.h"
#include "panel/timebased/timebased.h"
#include "widget/viewer/viewer.h"

namespace olive {

class ViewerPanelBase : public TimeBasedPanel {
  Q_OBJECT
 public:
  explicit ViewerPanelBase(const QString &object_name);

  [[nodiscard]] ViewerWidget *GetViewerWidget() const { return dynamic_cast<ViewerWidget *>(GetTimeBasedWidget()); }

  void PlayPause() override;

  void PlayInToOut() override;

  void ShuttleLeft() override;

  void ShuttleStop() override;

  void ShuttleRight() override;

  void ConnectTimeBasedPanel(TimeBasedPanel *panel) const;

  void DisconnectTimeBasedPanel(TimeBasedPanel *panel) const;

  /**
   * @brief Wrapper for ViewerWidget::SetFullScreen()
   */
  void SetFullScreen(QScreen *screen = nullptr) const;

  [[nodiscard]] ColorManager *GetColorManager() const { return GetViewerWidget()->color_manager(); }

  void UpdateTextureFromNode() const { GetViewerWidget()->UpdateTextureFromNode(); }

  void AddPlaybackDevice(ViewerDisplayWidget *vw) const { GetViewerWidget()->AddPlaybackDevice(vw); }

  void SetTimelineSelectedBlocks(const QVector<Block *> &b) const { GetViewerWidget()->SetTimelineSelectedBlocks(b); }

  void SetNodeViewSelections(const QVector<Node *> &n) const { GetViewerWidget()->SetNodeViewSelections(n); }

  void ConnectMulticamWidget(MulticamWidget *p) const { GetViewerWidget()->ConnectMulticamWidget(p); }

 public slots:
  void SetGizmos(Node *node) const;

  void CacheEntireSequence() const;

  void CacheSequenceInOut() const;

  void RequestStartEditingText() const { GetViewerWidget()->RequestStartEditingText(); }

 signals:
  /**
   * @brief Signal emitted when a new frame is loaded
   */
  void TextureChanged(TexturePtr t);

  /**
   * @brief Wrapper for ViewerGLWidget::ColorProcessorChanged()
   */
  void ColorProcessorChanged(ColorProcessorPtr processor);

  /**
   * @brief Wrapper for ViewerGLWidget::ColorManagerChanged()
   */
  void ColorManagerChanged(ColorManager *color_manager);

 protected:
  void SetViewerWidget(ViewerWidget *vw);

 private slots:
  void FocusedPanelChanged(PanelWidget *panel);
};

}  // namespace olive

#endif  // VIEWERPANELBASE_H
