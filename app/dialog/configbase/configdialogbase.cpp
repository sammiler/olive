#include "configdialogbase.h"

#include <QDialogButtonBox>
#include <QSplitter>
#include <QVBoxLayout>

#include "core.h"

namespace olive {

ConfigDialogBase::ConfigDialogBase(QWidget* parent) : QDialog(parent) {
  auto* layout = new QVBoxLayout(this);

  auto* splitter = new QSplitter();
  splitter->setChildrenCollapsible(false);
  layout->addWidget(splitter);

  list_widget_ = new QListWidget();

  preference_pane_stack_ = new QStackedWidget(this);

  splitter->addWidget(list_widget_);
  splitter->addWidget(preference_pane_stack_);

  auto* button_box = new QDialogButtonBox(this);
  button_box->setOrientation(Qt::Horizontal);
  button_box->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

  layout->addWidget(button_box);

  connect(button_box, &QDialogButtonBox::accepted, this, &ConfigDialogBase::accept);
  connect(button_box, &QDialogButtonBox::rejected, this, &ConfigDialogBase::reject);

  connect(list_widget_, &QListWidget::currentRowChanged, preference_pane_stack_, &QStackedWidget::setCurrentIndex);
}

void ConfigDialogBase::accept() {
  foreach (ConfigDialogBaseTab* tab, tabs_) {
    if (!tab->Validate()) {
      return;
    }
  }

  auto* command = new MultiUndoCommand();

  foreach (ConfigDialogBaseTab* tab, tabs_) {
    tab->Accept(command);
  }

  Core::instance()->undo_stack()->push(command, tr("Set Configuration"));

  AcceptEvent();

  QDialog::accept();
}

void ConfigDialogBase::AddTab(ConfigDialogBaseTab* tab, const QString& title) {
  list_widget_->addItem(title);
  preference_pane_stack_->addWidget(tab);

  tabs_.append(tab);
}

}  // namespace olive
