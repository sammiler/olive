#include "keyframeproperties.h"

#include <QDialogButtonBox>
#include <QGridLayout>

#include "core.h"
#include "node/nodeundo.h"
#include "widget/keyframeview/keyframeviewundo.h"

namespace olive {

KeyframePropertiesDialog::KeyframePropertiesDialog(const std::vector<NodeKeyframe*>& keys, const rational& timebase,
                                                   QWidget* parent)
    : QDialog(parent), keys_(keys), timebase_(timebase) {
  setWindowTitle(tr("Keyframe Properties"));

  auto* layout = new QGridLayout(this);

  int row = 0;

  layout->addWidget(new QLabel("Time:"), row, 0);

  time_slider_ = new RationalSlider();
  time_slider_->SetDisplayType(RationalSlider::kTime);
  time_slider_->SetTimebase(timebase_);
  layout->addWidget(time_slider_, row, 1);

  row++;

  layout->addWidget(new QLabel("Type:"), row, 0);

  type_select_ = new QComboBox();
  connect(type_select_, SIGNAL(currentIndexChanged(int)), this, SLOT(KeyTypeChanged(int)));
  layout->addWidget(type_select_, row, 1);

  row++;

  // Bezier handles
  bezier_group_ = new QGroupBox();

  auto* bezier_group_layout = new QGridLayout(bezier_group_);

  bezier_group_layout->addWidget(new QLabel(tr("In:")), 0, 0);

  bezier_in_x_slider_ = new FloatSlider();
  bezier_group_layout->addWidget(bezier_in_x_slider_, 0, 1);

  bezier_in_y_slider_ = new FloatSlider();
  bezier_group_layout->addWidget(bezier_in_y_slider_, 0, 2);

  bezier_group_layout->addWidget(new QLabel(tr("Out:")), 1, 0);

  bezier_out_x_slider_ = new FloatSlider();
  bezier_group_layout->addWidget(bezier_out_x_slider_, 1, 1);

  bezier_out_y_slider_ = new FloatSlider();
  bezier_group_layout->addWidget(bezier_out_y_slider_, 1, 2);

  layout->addWidget(bezier_group_, row, 0, 1, 2);

  bool all_same_time = true;
  bool can_set_time = true;

  bool all_same_type = true;

  bool all_same_bezier_in_x = true;
  bool all_same_bezier_in_y = true;
  bool all_same_bezier_out_x = true;
  bool all_same_bezier_out_y = true;

  for (size_t i = 0; i < keys_.size(); i++) {
    if (i > 0) {
      NodeKeyframe* prev_key = keys_.at(i - 1);
      NodeKeyframe* this_key = keys_.at(i);

      // Determine if the keyframes are all the same time or not
      if (all_same_time) {
        all_same_time = (prev_key->time() == this_key->time());
      }

      // Determine if the keyframes are all the same type
      if (all_same_type) {
        all_same_type = (prev_key->type() == this_key->type());
      }

      // Check all four bezier control points
      if (all_same_bezier_in_x) {
        all_same_bezier_in_x = (prev_key->bezier_control_in().x() == this_key->bezier_control_in().x());
      }

      if (all_same_bezier_in_y) {
        all_same_bezier_in_y = (prev_key->bezier_control_in().y() == this_key->bezier_control_in().y());
      }

      if (all_same_bezier_out_x) {
        all_same_bezier_out_x = (prev_key->bezier_control_out().x() == this_key->bezier_control_out().x());
      }

      if (all_same_bezier_out_y) {
        all_same_bezier_out_y = (prev_key->bezier_control_out().y() == this_key->bezier_control_out().y());
      }
    }

    // Determine if any keyframes are on the same track (in which case we can't set the time)
    if (can_set_time) {
      for (size_t j = 0; j < keys_.size(); j++) {
        if (i != j && keys_.at(j)->track() == keys_.at(i)->track()) {
          can_set_time = false;
          break;
        }
      }
    }

    if (!all_same_time && !all_same_type && !can_set_time && !all_same_bezier_in_x && !all_same_bezier_in_y &&
        !all_same_bezier_out_x && !all_same_bezier_out_y) {
      break;
    }
  }

  if (all_same_time) {
    time_slider_->SetValue(keys_.front()->time());
  } else {
    time_slider_->SetTristate();
  }

  time_slider_->setEnabled(can_set_time);

  if (!all_same_type) {
    // If all keyframes aren't the same type, add an empty item
    type_select_->addItem(QStringLiteral("--"), -1);

    // Ensure UI updates for the index being 0
    KeyTypeChanged(0);
  }

  type_select_->addItem(tr("Linear"), NodeKeyframe::kLinear);
  type_select_->addItem(tr("Hold"), NodeKeyframe::kHold);
  type_select_->addItem(tr("Bezier"), NodeKeyframe::kBezier);

  if (all_same_type) {
    // If all keyframes are the same type, set it here
    for (int i = 0; i < type_select_->count(); i++) {
      if (type_select_->itemData(i).toInt() == keys_.front()->type()) {
        type_select_->setCurrentIndex(i);

        // Ensure UI updates for this index
        KeyTypeChanged(i);
        break;
      }
    }
  }

  SetUpBezierSlider(bezier_in_x_slider_, all_same_bezier_in_x, keys_.front()->bezier_control_in().x());
  SetUpBezierSlider(bezier_in_y_slider_, all_same_bezier_in_y, keys_.front()->bezier_control_in().y());
  SetUpBezierSlider(bezier_out_x_slider_, all_same_bezier_out_x, keys_.front()->bezier_control_out().x());
  SetUpBezierSlider(bezier_out_y_slider_, all_same_bezier_out_y, keys_.front()->bezier_control_out().y());

  row++;

  auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  buttons->setCenterButtons(true);
  layout->addWidget(buttons, row, 0, 1, 2);
  connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void KeyframePropertiesDialog::accept() {
  auto* command = new MultiUndoCommand();

  rational new_time = time_slider_->GetValue();
  int new_type = type_select_->currentData().toInt();

  foreach (NodeKeyframe* key, keys_) {
    if (time_slider_->isEnabled() && !time_slider_->IsTristate()) {
      command->add_child(new NodeParamSetKeyframeTimeCommand(key, new_time));
    }

    if (new_type > -1) {
      command->add_child(new KeyframeSetTypeCommand(key, static_cast<NodeKeyframe::Type>(new_type)));
    }

    if (bezier_group_->isEnabled()) {
      command->add_child(new KeyframeSetBezierControlPoint(
          key, NodeKeyframe::kInHandle, QPointF(bezier_in_x_slider_->GetValue(), bezier_in_y_slider_->GetValue())));

      command->add_child(new KeyframeSetBezierControlPoint(
          key, NodeKeyframe::kOutHandle, QPointF(bezier_out_x_slider_->GetValue(), bezier_out_y_slider_->GetValue())));
    }
  }

  Core::instance()->undo_stack()->push(command, tr("Set Keyframe Properties"));

  QDialog::accept();
}

void KeyframePropertiesDialog::SetUpBezierSlider(FloatSlider* slider, bool all_same, double value) {
  if (all_same) {
    slider->SetValue(value);
  } else {
    slider->SetTristate();
  }
}

void KeyframePropertiesDialog::KeyTypeChanged(int index) {
  bezier_group_->setEnabled(type_select_->itemData(index) == NodeKeyframe::kBezier);
}

}  // namespace olive
