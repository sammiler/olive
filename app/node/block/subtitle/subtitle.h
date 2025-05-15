

#ifndef SUBTITLEBLOCK_H
#define SUBTITLEBLOCK_H

#include "node/block/clip/clip.h"

namespace olive {

class SubtitleBlock : public ClipBlock {
  Q_OBJECT
 public:
  SubtitleBlock();

  NODE_DEFAULT_FUNCTIONS(SubtitleBlock)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  static const QString kTextIn;

  [[nodiscard]] QString GetText() const { return GetStandardValue(kTextIn).toString(); }

  void SetText(const QString &text) { SetStandardValue(kTextIn, text); }
};

}  // namespace olive

#endif  // SUBTITLEBLOCK_H
