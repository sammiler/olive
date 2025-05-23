#include "footagerelinkdialog.h"

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QScrollBar>
#include <QVBoxLayout>

namespace olive {

FootageRelinkDialog::FootageRelinkDialog(const QVector<Footage*>& footage, QWidget* parent)
    : QDialog(parent), footage_(footage) {
  auto* layout = new QVBoxLayout(this);

  layout->addWidget(
      new QLabel("The following files couldn't be found. Clips using them will be "
                 "unplayable until they're relinked."));

  table_ = new QTreeWidget();

  table_->setColumnCount(3);
  table_->setHeaderLabels({tr("Footage"), tr("Filename"), tr("Actions")});
  table_->setRootIsDecorated(false);
  table_->setSelectionBehavior(QAbstractItemView::SelectRows);
  table_->header()->setSectionsMovable(false);

  // Prefer stretching URL column (QHeaderView defaults to stretching the last column, which in
  // our case is just a browse button)
  table_->header()->setSectionResizeMode(1, QHeaderView::Stretch);
  table_->header()->setStretchLastSection(false);

  for (int i = 0; i < footage.size(); i++) {
    Footage* f = footage.at(i);
    auto* item = new QTreeWidgetItem();

    auto* item_actions = new QWidget();
    auto* item_actions_layout = new QHBoxLayout(item_actions);
    auto* item_browse_btn = new QPushButton(tr("Browse"));
    item_browse_btn->setProperty("index", i);
    connect(item_browse_btn, &QPushButton::clicked, this, &FootageRelinkDialog::BrowseForFootage);
    item_actions_layout->addWidget(item_browse_btn);

    item->setIcon(0, f->data(Node::ICON).value<QIcon>());
    item->setText(0, f->GetLabel());
    item->setText(1, f->filename());

    table_->addTopLevelItem(item);

    table_->setItemWidget(item, 2, item_actions);
  }

  layout->addWidget(table_);

  auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttons, &QDialogButtonBox::accepted, this, &FootageRelinkDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, this, &FootageRelinkDialog::reject);
  layout->addWidget(buttons);

  setWindowTitle(tr("Relink Footage"));
}

void FootageRelinkDialog::UpdateFootageItem(int index) {
  Footage* f = footage_.at(index);
  QTreeWidgetItem* item = table_->topLevelItem(index);
  item->setIcon(0, f->data(Node::ICON).value<QIcon>());
  item->setText(1, f->filename());
}

void FootageRelinkDialog::BrowseForFootage() {
  int index = sender()->property("index").toInt();
  Footage* f = footage_.at(index);

  QFileInfo info(f->filename());

  QString new_fn = QFileDialog::getOpenFileName(this, tr("Relink \"%1\"").arg(f->GetLabel()), info.absolutePath());

  // Originally, this function would attempt to filter to the exact filename of the missing file.
  // However, this would break on Windows if the filename had any spaces in it. The reason is
  // Windows separates its extensions with ';' while Qt separates them with ' '. Qt isn't
  // intelligent enough to determine whether it's a list of extensions or a single filename with a
  // space in it, it just does a global replace of ' ' to ';'. There's no way around it, outside of
  // bypassing Qt entirely and using Win32's GetOpenFileName() directly. As annoying as it is, I've
  // just disabled it for now.
  // QStringLiteral("%1 (\"%1\");;%2 (*)").arg(info.fileName(), tr("All Files")));

  // We received a new filename
  if (!new_fn.isEmpty()) {
    // Store original dir since we might be able to use this to find other files
    QDir original_dir = info.dir();
    QDir new_dir = QFileInfo(new_fn).dir();

    // Set new filename since this was set manually by the user
    f->set_filename(new_fn);

    // Assume footage is valid here. We could do some decoder probing to ensure it's a usable file
    // but otherwise we assume the user knows what they're doing here.

    // Set footage to valid and update icon
    f->SetValid();

    // Update item visually
    UpdateFootageItem(index);

    // Check all other footage files for matches
    for (int it = 0; it < footage_.size(); it++) {
      Footage* other_footage = footage_.at(it);

      // Ignore current footage file and footage that's already valid of course
      if (index != it && !other_footage->IsValid()) {
        // Get footage path relative to original directory
        QString relative_to_original = original_dir.relativeFilePath(other_footage->filename());
        QString absolute_to_new = new_dir.filePath(relative_to_original);

        // Second attempt. Try appending the filename to our new filepath
        if (!QFileInfo::exists(absolute_to_new)) {
          QFileInfo file_info(other_footage->filename());
          absolute_to_new = new_dir.filePath(file_info.fileName());
        }

        // Check if file exists
        if (QFileInfo::exists(absolute_to_new)) {
          other_footage->set_filename(absolute_to_new);
          other_footage->SetValid();
          UpdateFootageItem(it);
        }
      }
    }
  }

  // Check where the next invalid footage is. If there is none, accept automatically. Otherwise,
  // jump to that footage so the user knows where it is.
  int next_invalid = -1;
  for (int i = 0; i < footage_.size(); i++) {
    if (!footage_.at(i)->IsValid()) {
      next_invalid = i;
      break;
    }
  }

  if (next_invalid == -1) {
    // No more invalid footage, just accept
    this->accept();
  } else {
    // Jump to next invalid footage
    QModelIndex idx = table_->model()->index(next_invalid, 0);
    table_->selectionModel()->select(idx, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    table_->scrollTo(idx);
  }
}

}  // namespace olive
