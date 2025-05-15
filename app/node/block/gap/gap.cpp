#include "gap.h"

namespace olive {

GapBlock::GapBlock() = default;

QString GapBlock::Name() const { return tr("Gap"); }

QString GapBlock::id() const { return QStringLiteral("org.olivevideoeditor.Olive.gap"); }

QString GapBlock::Description() const { return tr("A time-based node that represents an empty space."); }

}  // namespace olive
