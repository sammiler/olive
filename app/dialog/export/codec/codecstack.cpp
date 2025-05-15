

#include "codecstack.h"

namespace olive {

#define super QStackedWidget

CodecStack::CodecStack(QWidget *parent) : super{parent} {
  connect(this, &CodecStack::currentChanged, this, &CodecStack::OnChange);
}

void CodecStack::addWidget(QWidget *widget) {
  super::addWidget(widget);

  OnChange(currentIndex());
}

void CodecStack::OnChange(int index) {
  for (int i = 0; i < count(); i++) {
    if (i == index) {
      widget(i)->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    } else {
      widget(i)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    }
    widget(i)->adjustSize();
  }
  adjustSize();
}

}  // namespace olive
