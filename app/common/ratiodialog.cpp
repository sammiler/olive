#include "ratiodialog.h"

#include <QCoreApplication>
#include <QMessageBox>
#include <QRegularExpression>

namespace olive {

double GetFloatRatioFromUser(QWidget* parent, const QString& title, bool* ok_in) {
  QString s;

  forever {
    bool ok;

    s = QInputDialog::getText(
        parent, title, QCoreApplication::translate("RatioDialog", R"(Enter custom ratio (e.g. "4:3", "16/9", etc.):)"),
        QLineEdit::Normal, s, &ok);

    if (!ok) {
      // User cancelled dialog, do nothing
      if (ok_in) {
        *ok_in = false;
      }
      return qSNaN();
    }

    QStringList ratio_components = s.split(QRegularExpression(QStringLiteral(":|;|\\/")));

    if (ratio_components.size() == 1) {
      bool float_ok;

      double flt = ratio_components.at(0).toDouble(&float_ok);

      if (float_ok && flt > 0) {
        if (ok_in) {
          *ok_in = true;
        }
        return flt;
      }
    } else if (ratio_components.size() == 2) {
      bool numer_ok, denom_ok;

      double num = ratio_components.at(0).toDouble(&numer_ok);
      double den = ratio_components.at(1).toDouble(&denom_ok);

      if (numer_ok && denom_ok && num > 0 && den > 0) {
        // Exit loop and set this ratio
        if (ok_in) {
          *ok_in = true;
        }
        return num / den;
      }
    }

    QMessageBox::warning(parent, QCoreApplication::translate("RatioDialog", "Invalid custom ratio"),
                         QCoreApplication::translate("RatioDialog",
                                                     "Failed to parse \"%1\" into an aspect ratio. Please format a "
                                                     "rational fraction with a ':' or a '/' separator.")
                             .arg(s),
                         QMessageBox::Ok);
  }
}

}  // namespace olive
