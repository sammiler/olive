#include "viewerwindow.h"

#include <QKeyEvent>
#include <QVBoxLayout>

namespace olive {

ViewerWindow::ViewerWindow(QWidget *parent) : QWidget(parent, Qt::Window | Qt::WindowStaysOnTopHint), pixel_aspect_(1) {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  display_widget_ = new ViewerDisplayWidget();
  layout->addWidget(display_widget_);
}

ViewerDisplayWidget *ViewerWindow::display_widget() const { return display_widget_; }

void ViewerWindow::SetVideoParams(const VideoParams &params) {
  width_ = params.width();
  height_ = params.height();
  pixel_aspect_ = params.pixel_aspect_ratio();

  UpdateMatrix();
}

void ViewerWindow::SetResolution(int width, int height) {
  width_ = width;
  height_ = height;

  UpdateMatrix();
}

void ViewerWindow::SetPixelAspectRatio(const rational &pixel_aspect) {
  pixel_aspect_ = pixel_aspect;

  UpdateMatrix();
}

void ViewerWindow::keyPressEvent(QKeyEvent *e) {
  QWidget::keyPressEvent(e);

  if (e->key() == Qt::Key_Escape) {
    close();
  }
}

void ViewerWindow::closeEvent(QCloseEvent *e) {
  QWidget::closeEvent(e);

  deleteLater();
}

void ViewerWindow::UpdateMatrix() {
  // Set GL widget matrix to maintain this texture's aspect ratio
  double window_ar = static_cast<double>(this->width()) / static_cast<double>(this->height());
  double image_ar = static_cast<double>(width_) / static_cast<double>(height_) * pixel_aspect_.toDouble();

  QMatrix4x4 mat;

  if (window_ar > image_ar) {
    // Window is wider than image, adjust X scale
    mat.scale(image_ar / window_ar, 1.0f, 1.0f);
  } else if (window_ar < image_ar) {
    // Window is taller than image, adjust Y scale
    mat.scale(1.0f, window_ar / image_ar, 1.0f);
  }

  display_widget_->SetMatrixZoom(mat);
}

}  // namespace olive
