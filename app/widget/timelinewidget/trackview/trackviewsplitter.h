#ifndef TRACKVIEWSPLITTER_H
#define TRACKVIEWSPLITTER_H

#include <QSplitter>

#include "common/define.h"

namespace olive {

class TrackViewSplitterHandle : public QSplitterHandle {
  Q_OBJECT
 public:
  TrackViewSplitterHandle(Qt::Orientation orientation, QSplitter *parent);

 protected:
  void mousePressEvent(QMouseEvent *e) override;
  void mouseMoveEvent(QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;

  void paintEvent(QPaintEvent *e) override;

 private:
  int drag_y_{};

  bool dragging_;
};

class TrackViewSplitter : public QSplitter {
  Q_OBJECT
 public:
  explicit TrackViewSplitter(Qt::Alignment vertical_alignment, QWidget *parent = nullptr);

  void HandleReceiver(TrackViewSplitterHandle *h, int diff);

  void SetHeightWithSizes(QList<int> sizes);

  void Insert(int index, int height, QWidget *item);
  void Remove(int index);

  void SetSpacerHeight(int height);

 public slots:
  void SetTrackHeight(int index, int h);

 signals:
  void TrackHeightChanged(int index, int height);

 protected:
  QSplitterHandle *createHandle() override;

 private:
  Qt::Alignment alignment_;

  int spacer_height_;
};

}  // namespace olive

#endif  // TRACKVIEWSPLITTER_H
