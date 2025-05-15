#include "colorcoding.h"

namespace olive {

QVector<Color> ColorCoding::colors_ = {
    Color(0.545f, 0.255f, 0.255f), Color(0.412f, 0.188f, 0.259f), Color(0.561f, 0.427f, 0.239f),
    Color(0.486f, 0.306f, 0.235f), Color(0.631f, 0.612f, 0.212f), Color(0.404f, 0.478f, 0.243f),
    Color(0.349f, 0.576f, 0.275f), Color(0.224f, 0.459f, 0.251f), Color(0.259f, 0.471f, 0.541f),
    Color(0.184f, 0.376f, 0.329f), Color(0.259f, 0.365f, 0.541f), Color(0.196f, 0.216f, 0.412f),
    Color(0.612f, 0.294f, 0.502f), Color(0.404f, 0.220f, 0.459f), Color(0.800f, 0.800f, 0.800f),
    Color(0.502f, 0.502f, 0.502f)};

QString ColorCoding::GetColorName(int c) {
  // FIXME: I'm sure we could come up with more creative names for these colors
  switch (c) {
    case kRed:
      return tr("Red");
    case kMaroon:
      return tr("Maroon");
    case kOrange:
      return tr("Orange");
    case kBrown:
      return tr("Brown");
    case kYellow:
      return tr("Yellow");
    case kOlive:
      return tr("Olive");
    case kLime:
      return tr("Lime");
    case kGreen:
      return tr("Green");
    case kCyan:
      return tr("Cyan");
    case kTeal:
      return tr("Teal");
    case kBlue:
      return tr("Blue");
    case kNavy:
      return tr("Navy");
    case kPink:
      return tr("Pink");
    case kPurple:
      return tr("Purple");
    case kSilver:
      return tr("Silver");
    case kGray:
      return tr("Gray");
  }

  return {};
}

Color ColorCoding::GetColor(int c) { return colors_.at(c); }

Qt::GlobalColor ColorCoding::GetUISelectorColor(const Color &c) {
  if (c.GetRoughLuminance() > 0.40f) {
    return Qt::black;
  } else {
    return Qt::white;
  }
}

}  // namespace olive
