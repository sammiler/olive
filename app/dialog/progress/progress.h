#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include <QProgressBar>

#include "common/debug.h"
#include "widget/taskview/elapsedcounterwidget.h"

namespace olive {

class ProgressDialog : public QDialog {
  Q_OBJECT
 public:
  ProgressDialog(const QString& message, const QString& title, QWidget* parent = nullptr);

 protected:
  void showEvent(QShowEvent* e) override;

  void closeEvent(QCloseEvent*) override;

 public slots:
  void SetProgress(double value);

 signals:
  void Cancelled();

 protected:
  void ShowErrorMessage(const QString& title, const QString& message);

 private:
  QProgressBar* bar_;

  ElapsedCounterWidget* elapsed_timer_lbl_;

  bool show_progress_;

  bool first_show_;

 private slots:
  void DisableSenderWidget();

  void DisableProgressWidgets();
};

}  // namespace olive

#endif  // PROGRESSDIALOG_H
