

#ifndef GAPBLOCK_H
#define GAPBLOCK_H

#include "node/block/block.h"

namespace olive {

/**
 * @brief Node that represents nothing in its respective track for a certain period of time
 */
class GapBlock : public Block {
  Q_OBJECT
 public:
  GapBlock();

  NODE_DEFAULT_FUNCTIONS(GapBlock)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QString Description() const override;
};

}  // namespace olive

#endif  // TIMELINEBLOCK_H
