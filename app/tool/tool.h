#ifndef TOOL_H
#define TOOL_H

#include <QCoreApplication>
#include <QString>

#include "common/define.h"

namespace olive {

class Tool {
 public:
  /**
   * @brief A list of tools that can be used throughout the application
   */
  enum Item {
    /// No tool. This should never be set as the application tool, its only real purpose is to indicate the lack of
    /// a tool somewhere similar to nullptr.
    kNone,

    /// Pointer tool
    kPointer,

    /// Edit tool
    kEdit,

    /// Ripple tool
    kRipple,

    /// Rolling tool
    kRolling,

    /// Razor tool
    kRazor,

    /// Slip tool
    kSlip,

    /// Slide tool
    kSlide,

    /// Hand tool
    kHand,

    /// Zoom tool
    kZoom,

    /// Transition tool
    kTransition,

    /// Record tool
    kRecord,

    /// Add tool
    kAdd,

    /// Track select tool
    kTrackSelect,

    kCount
  };

  /**
   * @brief Tools that can be added using the kAdd tool
   */
  enum AddableObject {
    /// An empty clip
    kAddableEmpty,

    /// A video clip showing a generic video placeholder
    kAddableBars,

    /// A video clip showing a primitive shape
    kAddableShape,

    /// A video clip with a solid connected
    kAddableSolid,

    /// A video clip with a title connected
    kAddableTitle,

    /// An audio clip with a sine connected to it
    kAddableTone,

    /// A subtitle clip
    kAddableSubtitle,

    kAddableCount
  };

  static QString GetAddableObjectName(const AddableObject& a) {
    switch (a) {
      case kAddableEmpty:
        return QCoreApplication::translate("Tool", "Empty");
      case kAddableBars:
        return QCoreApplication::translate("Tool", "Bars");
      case kAddableShape:
        return QCoreApplication::translate("Tool", "Shape");
      case kAddableSolid:
        return QCoreApplication::translate("Tool", "Solid");
      case kAddableTitle:
        return QCoreApplication::translate("Tool", "Title");
      case kAddableTone:
        return QCoreApplication::translate("Tool", "Tone");
      case kAddableSubtitle:
        return QCoreApplication::translate("Tool", "Subtitle");
      case kAddableCount:
        break;
    }

    return QCoreApplication::translate("Tool", "Unknown");
  }

  static QString GetAddableObjectID(const AddableObject& a) {
    switch (a) {
      case kAddableEmpty:
        return QStringLiteral("empty");
      case kAddableBars:
        return QStringLiteral("bars");
      case kAddableShape:
        return QStringLiteral("shape");
      case kAddableSolid:
        return QStringLiteral("solid");
      case kAddableTitle:
        return QStringLiteral("title");
      case kAddableTone:
        return QStringLiteral("tone");
      case kAddableSubtitle:
        return QStringLiteral("subtitle");
      case kAddableCount:
        break;
    }

    return {};
  }
};

}  // namespace olive

#endif  // TOOL_H
