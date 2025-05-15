

#ifndef COLORCODING_H
#define COLORCODING_H

#include <olive/core/core.h>
#include <QObject>

namespace olive {

using namespace core;

class ColorCoding : public QObject {
  Q_OBJECT
 public:
  enum Code {
    kRed,
    kMaroon,
    kOrange,
    kBrown,
    kYellow,
    kOlive,
    kLime,
    kGreen,
    kCyan,
    kTeal,
    kBlue,
    kNavy,
    kPink,
    kPurple,
    kSilver,
    kGray
  };

  static QString GetColorName(int c);

  static Color GetColor(int c);

  static Qt::GlobalColor GetUISelectorColor(const Color& c);

  static const QVector<Color>& standard_colors() { return colors_; }

 private:
  static QVector<Color> colors_;
};

}  // namespace olive

#endif  // COLORCODING_H
