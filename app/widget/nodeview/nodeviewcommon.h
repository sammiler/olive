#ifndef NODEVIEWCOMMON_H
#define NODEVIEWCOMMON_H

#include <QtGlobal>

#include "common/define.h"

namespace olive {

class NodeViewCommon {
 public:
  enum FlowDirection { kInvalidDirection = -1, kTopToBottom, kBottomToTop, kLeftToRight, kRightToLeft };

  static Qt::Orientation GetFlowOrientation(FlowDirection dir) {
    if (dir == kTopToBottom || dir == kBottomToTop) {
      return Qt::Vertical;
    } else {
      return Qt::Horizontal;
    }
  }

  static bool IsFlowVertical(FlowDirection dir) { return dir == kTopToBottom || dir == kBottomToTop; }

  static bool IsFlowHorizontal(FlowDirection dir) { return dir == kLeftToRight || dir == kRightToLeft; }

  static bool DirectionsAreOpposing(FlowDirection a, FlowDirection b) {
    return ((a == NodeViewCommon::kLeftToRight && b == NodeViewCommon::kRightToLeft) ||
            (a == NodeViewCommon::kRightToLeft && b == NodeViewCommon::kLeftToRight) ||
            (a == NodeViewCommon::kTopToBottom && b == NodeViewCommon::kBottomToTop) ||
            (a == NodeViewCommon::kBottomToTop && b == NodeViewCommon::kTopToBottom));
  }
};

}  // namespace olive

#endif  // NODEVIEWCOMMON_H
