

#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QSettings>
#include <QWidget>

#include "common/define.h"

namespace olive {

class StyleManager : public QObject {
 public:
  static void Init();

  static const QString& GetStyle();

  static void SetStyle(const QString& style_path);

  static const char* kDefaultStyle;

  static const QMap<QString, QString>& available_themes() { return available_themes_; }

 private:
  static QPalette ParsePalette(const QString& ini_path);

  static void ParsePaletteGroup(QSettings* ini, QPalette* palette, QPalette::ColorGroup group);

  static void ParsePaletteColor(QSettings* ini, QPalette* palette, QPalette::ColorGroup group,
                                const QString& role_name);

  static QString current_style_;

  static QMap<QString, QString> available_themes_;
};

}  // namespace olive

#endif  // STYLEMANAGER_H
