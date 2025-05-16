#ifndef FRAMERATECOMBOBOX_H
#define FRAMERATECOMBOBOX_H

#include <QComboBox>
#include <QEvent>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>

#include "common/qtutils.h"
#include "render/videoparams.h"
namespace olive {

// using namespace core; // 如果 rational 类型直接来自 core 命名空间，则可能需要。但 VideoParams 通常会处理。

/**
 * @brief FrameRateComboBox 类是一个自定义控件，用于选择视频帧率。
 *
 * 它内部使用一个 QComboBox 来显示标准的预定义帧率列表，
 * 并允许用户通过一个输入对话框指定一个自定义的帧率。
 * 当帧率被更改时，会发出 FrameRateChanged 信号。
 */
class FrameRateComboBox : public QWidget {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit FrameRateComboBox(QWidget* parent = nullptr) : QWidget(parent) {
    inner_ = new QComboBox(); // 创建内部的 QComboBox 实例

    // 设置布局
    auto* layout = new QHBoxLayout(this); // 创建水平布局
    layout->setSpacing(0);                // 设置控件间距为0
    layout->setContentsMargins(0, 0, 0, 0); // 设置布局边距为0
    layout->addWidget(inner_);            // 将 QComboBox 添加到布局中

    RepopulateList(); // 填充帧率列表

    old_index_ = 0; // 初始化上一次选中的索引

    // 连接内部 QComboBox 的 currentIndexChanged 信号到此类的 IndexChanged 槽
    connect(inner_, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &FrameRateComboBox::IndexChanged);
  }

  /**
   * @brief 获取当前选中的帧率。
   *
   * 如果选择的是“自定义...”项，则返回用户输入的自定义帧率；
   * 否则返回从预定义列表中选中的帧率。
   * @return 返回 rational 类型的帧率值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] rational GetFrameRate() const {
    if (inner_->currentIndex() == inner_->count() - 1) { // 如果当前选中的是最后一项（自定义项）
      return custom_rate_; // 返回存储的自定义帧率
    } else {
      return inner_->currentData().value<rational>(); // 否则从选中项的用户数据中获取 rational 类型的帧率
    }
  }

  /**
   * @brief 设置组合框的当前帧率。
   *
   * 如果给定的帧率在预定义列表中，则选中该项。
   * 否则，将给定的帧率设置为自定义帧率，并选中“自定义...”项。
   * @param r 要设置的 rational 类型的帧率。
   */
  void SetFrameRate(const rational& r) {
    int standard_rates = inner_->count() - 1; // 标准帧率的数量（不包括“自定义...”项）
    for (int i = 0; i < standard_rates; i++) { // 遍历所有标准帧率项
      if (inner_->itemData(i).value<rational>() == r) { // 如果找到匹配的标准帧率
        // 设置标准帧率
        old_index_ = i; // 更新旧索引
        SetInnerIndexWithoutSignal(i); // 设置 QComboBox 的当前索引（不触发信号）
        return; // 完成设置
      }
    }

    // 如果执行到这里，说明给定的帧率不在标准列表中，将其设置为自定义帧率
    custom_rate_ = r; // 存储自定义帧率
    old_index_ = inner_->count() - 1; // 更新旧索引为“自定义...”项的索引
    SetInnerIndexWithoutSignal(old_index_); // 设置 QComboBox 的当前索引为“自定义...”项
    RepopulateList(); // 重新填充列表以显示新的自定义帧率文本
  }

 signals:
  /**
   * @brief 当选定的帧率发生改变时发出此信号。
   * @param frame_rate 新的 rational 类型的帧率值。
   */
  void FrameRateChanged(const rational& frame_rate);

 protected:
  /**
   * @brief 处理 Qt 的 changeEvent 事件。
   *
   * 主要用于响应 QEvent::LanguageChange 事件，以便在应用程序语言设置更改时
   * 调用 RepopulateList() 方法来更新组合框中各项的显示文本。
   * @param event 指向 QEvent 对象的指针。
   */
  void changeEvent(QEvent* event) override {
    QWidget::changeEvent(event); // 调用基类的 changeEvent 处理

    if (event->type() == QEvent::LanguageChange) { // 如果是语言更改事件
      RepopulateList(); // 重新填充列表以更新文本
    }
  }

 private slots:
  /**
   * @brief 当内部 QComboBox 的当前选中索引发生改变时调用的槽函数。
   * @param index 新的选中项的索引。
   */
  void IndexChanged(int index) {
    if (index == inner_->count() - 1) { // 如果选中的是最后一项（“自定义...”）
      // 处理自定义帧率输入
      QString s; // 用于存储用户输入的字符串
      bool ok;   // 用于指示转换是否成功

      if (!custom_rate_.isNull()) { // 如果之前已有自定义帧率
        s = QString::number(custom_rate_.toDouble()); // 将其转换为字符串作为输入对话框的默认值
      }

      while (true) { // 循环直到用户输入有效值或取消
        // 弹出输入对话框让用户输入自定义帧率
        s = QInputDialog::getText(this, tr("Custom Frame Rate"), tr("Enter custom frame rate:"), QLineEdit::Normal, s,
                                  &ok);

        if (ok) { // 如果用户点击了“确定”
          rational r; // 用于存储转换后的 rational 帧率

          // 尝试将输入转换为 double，假设大多数用户会以这种方式输入帧率
          double d = s.toDouble(&ok);

          if (ok) { // 如果成功转换为 double
            // 尝试从 double 转换为 rational
            r = rational::fromDouble(d, &ok);
          } else {
            // 如果转换为 double 失败，尝试直接从字符串转换为 rational (以防用户输入了 "24/1" 这样的格式)
            r = rational::fromString(s.toUtf8().constData(), &ok);
          }

          if (ok) { // 如果成功转换为 rational
            custom_rate_ = r; // 更新存储的自定义帧率
            emit FrameRateChanged(r); // 发出帧率改变信号
            old_index_ = index;       // 更新旧索引
            RepopulateList();         // 重新填充列表以显示新的自定义帧率文本
            break;                    // 退出循环

          } else {
            // 如果转换失败，显示错误消息并继续循环
            QMessageBox::critical(this, tr("Invalid Input"), tr("Failed to convert \"%1\" to a frame rate.").arg(s));
          }

        } else {
          // 用户点击了“取消”，恢复到之前的选项
          SetInnerIndexWithoutSignal(old_index_);
          break; // 退出循环
        }
      }
    } else { // 如果选中的是标准帧率项
      old_index_ = index; // 更新旧索引
      emit FrameRateChanged(GetFrameRate()); // 发出帧率改变信号
    }
  }

 private:
  /**
   * @brief 重新填充组合框中的帧率列表。
   *
   * 此函数会清除现有项，然后从 VideoParams::kSupportedFrameRates 添加标准帧率，
   * 并在列表末尾添加“自定义...”选项（如果已设置自定义帧率，则显示该自定义帧率）。
   */
  void RepopulateList() {
    int temp_index = inner_->currentIndex(); // 保存当前选中的索引

    inner_->blockSignals(true); // 暂时阻塞信号，防止在清空和添加项时触发 currentIndexChanged

    inner_->clear(); // 清空组合框中的所有项

    // 遍历所有支持的标准帧率
    foreach (const rational& fr, VideoParams::kSupportedFrameRates) {
      // 将 rational 帧率转换为用户可读的字符串，并添加到组合框中
      // 同时，将原始的 rational 值作为 QVariant 存储在对应项的用户数据中
      inner_->addItem(VideoParams::FrameRateToString(fr), QVariant::fromValue(fr));
    }

    if (custom_rate_.isNull()) { // 如果当前没有设置自定义帧率
      inner_->addItem(tr("Custom...")); // 添加“自定义...”选项
    } else {
      // 如果有自定义帧率，则显示它，例如 "Custom (29.97)"
      inner_->addItem(tr("Custom (%1)").arg(VideoParams::FrameRateToString(custom_rate_)));
    }

    inner_->setCurrentIndex(temp_index); // 恢复之前选中的索引（如果仍然有效）

    inner_->blockSignals(false); // 解除信号阻塞
  }

  /**
   * @brief 在不发出 currentIndexChanged 信号的情况下设置内部 QComboBox 的当前索引。
   * @param index 要设置的索引。
   */
  void SetInnerIndexWithoutSignal(int index) {
    inner_->blockSignals(true);  // 阻塞信号
    inner_->setCurrentIndex(index); // 设置索引
    inner_->blockSignals(false); // 解除阻塞
  }

  QComboBox* inner_; ///< 内部实际使用的 QComboBox 控件。

  rational custom_rate_; ///< 存储用户输入的自定义帧率。

  int old_index_; ///< 用于在用户取消自定义输入时恢复之前的选中项。
};

}  // namespace olive

#endif  // FRAMERATECOMBOBOX_H
