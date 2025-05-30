#include "trackviewsplitter.h"

#include <QDebug>
#include <QPainter>

#include "node/output/track/track.h"

namespace olive {

TrackViewSplitter::TrackViewSplitter(Qt::Alignment vertical_alignment, QWidget *parent)
    : QSplitter(Qt::Vertical, parent), alignment_(vertical_alignment), spacer_height_(0) {
  setHandleWidth(1);

  int initial_height = 0;

  // Add empty spacer so we get a splitter handle after the last element
  auto *spacer = new QWidget();
  addWidget(spacer);

  setFixedHeight(initial_height);
}

void TrackViewSplitter::HandleReceiver(TrackViewSplitterHandle *h, int diff) {
  int ele_id = -1;

  for (int i = 0; i < count(); i++) {
    if (handle(i) == h) {
      ele_id = i;
      break;
    }
  }

  // The handle index is actually always one above the element index
  if (alignment_ == Qt::AlignTop) {
    ele_id--;
  } else if (alignment_ == Qt::AlignBottom) {
    diff = -diff;
  }

  QList<int> element_sizes = sizes();

  int old_ele_sz = element_sizes.at(ele_id);

  // Transform element size by diff
  int new_ele_sz = old_ele_sz + diff;

  // Limit by track minimum height
  new_ele_sz = qMax(new_ele_sz, Track::GetMinimumTrackHeightInPixels());

  if (alignment_ == Qt::AlignBottom) {
    ele_id = count() - ele_id - 1;
  }

  SetTrackHeight(ele_id, new_ele_sz);

  emit TrackHeightChanged(ele_id, new_ele_sz);
}

void TrackViewSplitter::SetTrackHeight(int index, int h) {
  QList<int> element_sizes = sizes();

  if (alignment_ == Qt::AlignBottom) {
    index = count() - index - 1;
  }

  int old_ele_sz = element_sizes.at(index);

  int diff = h - old_ele_sz;

  // Set new size on element
  element_sizes.replace(index, h);
  setSizes(element_sizes);

  // Increase height by the difference
  setFixedHeight(height() + diff);
}

void TrackViewSplitter::SetHeightWithSizes(QList<int> sizes) {
  int start_height = 0;

  // Add spacer height too
  if (alignment_ == Qt::AlignBottom) {
    sizes.replace(0, spacer_height_);
  } else {
    sizes.replace(sizes.size() - 1, spacer_height_);
  }

  foreach (int s, sizes) {
    start_height += s + handleWidth();
  }

  // The spacer doesn't need a handle width
  start_height -= handleWidth();

  setFixedHeight(start_height);
  setSizes(sizes);
}

void TrackViewSplitter::Insert(int index, int height, QWidget *item) {
  QList<int> sz = sizes();

  if (alignment_ == Qt::AlignBottom) {
    index = count() - index;
  }

  sz.insert(index, height);
  insertWidget(index, item);

  SetHeightWithSizes(sz);
}

void TrackViewSplitter::Remove(int index) {
  QList<int> sz = sizes();

  if (alignment_ == Qt::AlignBottom) {
    index = count() - 1 - index;
  }

  sz.removeAt(index);
  delete widget(index);

  SetHeightWithSizes(sz);
}

void TrackViewSplitter::SetSpacerHeight(int height) {
  spacer_height_ = height;
  SetHeightWithSizes(sizes());
}

QSplitterHandle *TrackViewSplitter::createHandle() { return new TrackViewSplitterHandle(orientation(), this); }

TrackViewSplitterHandle::TrackViewSplitterHandle(Qt::Orientation orientation, QSplitter *parent)
    : QSplitterHandle(orientation, parent), dragging_(false) {}

void TrackViewSplitterHandle::mousePressEvent(QMouseEvent *) {}

void TrackViewSplitterHandle::mouseMoveEvent(QMouseEvent *) {
  if (dragging_) {
    dynamic_cast<TrackViewSplitter *>(parent())->HandleReceiver(this, QCursor::pos().y() - drag_y_);
  }

  drag_y_ = QCursor::pos().y();
  dragging_ = true;
}

void TrackViewSplitterHandle::mouseReleaseEvent(QMouseEvent *) { dragging_ = false; }

void TrackViewSplitterHandle::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.fillRect(rect(), palette().base());
}

}  // namespace olive
