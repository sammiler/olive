#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H

#include <QWidget>

#include "viewerdisplay.h"

namespace olive {

class ViewerWindow : public QWidget {
 public:
  explicit ViewerWindow(QWidget* parent = nullptr);

  [[nodiscard]] ViewerDisplayWidget* display_widget() const;

  /**
   * @brief Used to adjust resulting picture to be the right aspect ratio
   *
   * Equivalent to calling SetResolution and SetPixelAspectRatio, just slightly faster since we
   * only calculate the matrix once rather than twice.
   */
  void SetVideoParams(const VideoParams& params);

  /**
   * @brief Used to adjust resulting picture to be the right aspect ratio
   */
  void SetResolution(int width, int height);

  /**
   * @brief Used to adjust resulting picture to be the right aspect ratio
   */
  void SetPixelAspectRatio(const rational& pixel_aspect);

 protected:
  void keyPressEvent(QKeyEvent* e) override;

  void closeEvent(QCloseEvent* e) override;

 private:
  void UpdateMatrix();

  int width_{};

  int height_{};

  ViewerDisplayWidget* display_widget_;

  rational pixel_aspect_;
};

}  // namespace olive

#endif  // VIEWERWINDOW_H
