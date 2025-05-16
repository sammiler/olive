#ifndef PIXELASPECTRATIOCOMBOBOX_H
#define PIXELASPECTRATIOCOMBOBOX_H

#include <QComboBox>    // Qt 组合框控件基类
#include <QStringList>  // Qt 字符串列表类 (用于 GetStandardPixelAspectRatioNames 返回类型)
#include <QVariant>     // Qt 通用数据类型类 (用于 QComboBox 的用户数据)
#include <QWidget>      // Qt 控件基类 (QComboBox 的基类)
#include <QInputDialog> // Qt 输入对话框 (虽然未直接包含，但 GetFloatRatioFromUser 可能使用)
#include <QMessageBox>  // Qt 消息框 (虽然未直接包含，但 GetFloatRatioFromUser 可能使用)


#include "common/qtutils.h"     // 项目通用的 Qt 工具函数 (可能包含 GetFloatRatioFromUser)
#include "common/ratiodialog.h" // 自定义比例对话框 (GetFloatRatioFromUser 可能使用此类)
#include "render/videoparams.h" // 视频参数定义 (包含 rational, kStandardPixelAspects, VideoParams::FormatPixelAspectRatioString)
#include "olive/core/core.h"    // Olive 核心库 (可能包含 rational 类型定义，videoparams.h 通常会包含它)


namespace olive {

// using namespace core; // 如果 rational 类型直接来自 core 命名空间，则可能需要。但 VideoParams 通常会处理。

/**
 * @brief PixelAspectRatioComboBox 类是一个自定义的 QComboBox，用于选择像素宽高比 (PAR)。
 *
 * 它会填充一系列标准的像素宽高比选项，并在列表末尾提供一个“自定义...”选项，
 * 允许用户通过对话框输入一个自定义的像素宽高比。
 */
class PixelAspectRatioComboBox : public QComboBox {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   *
   * 初始化组合框，从 VideoParams 中获取标准像素宽高比列表并填充。
   * 同时添加一个“自定义...”选项。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit PixelAspectRatioComboBox(QWidget* parent = nullptr) : QComboBox(parent), dont_prompt_custom_par_(false) {
    // 获取标准像素宽高比的名称列表
    QStringList par_names = VideoParams::GetStandardPixelAspectRatioNames();
    // 遍历 VideoParams 中定义的标准像素宽高比
    for (int i = 0; i < VideoParams::kStandardPixelAspects.size(); i++) {
      const rational& ratio = VideoParams::kStandardPixelAspects.at(i); // 获取当前的 rational 宽高比

      // 将格式化后的宽高比名称和对应的 rational 值添加到组合框中
      this->addItem(par_names.at(i), QVariant::fromValue(ratio));
    }

    // 始终在最后添加自定义项，许多逻辑依赖于此。
    // 将其数据设置为当前宽高比，以便如果没有匹配到上述任何项，最终会选中此项。
    this->addItem(QString()); // 添加一个空文本的项作为自定义项的占位符
    UpdateCustomItem(rational()); // 初始化自定义项的文本和数据

    // 连接 currentIndexChanged 信号到 IndexChanged 槽，以便在用户选择“自定义”时查询自定义宽高比
    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &PixelAspectRatioComboBox::IndexChanged);
  }

  /**
   * @brief 获取当前选中的像素宽高比。
   * @return 返回 rational 类型的像素宽高比值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] rational GetPixelAspectRatio() const { return this->currentData().value<rational>(); }

  /**
   * @brief 设置组合框的当前像素宽高比。
   *
   * 如果给定的宽高比在预定义列表中，则选中该项。
   * 否则，将给定的宽高比设置为自定义宽高比，并选中“自定义...”项。
   * @param r 要设置的 rational 类型的像素宽高比。
   */
  void SetPixelAspectRatio(const rational& r) {
    // 确定要选择的初始索引
    for (int i = 0; i < this->count(); i++) { // 遍历所有项
      if (this->itemData(i).value<rational>() == r) { // 如果找到匹配的宽高比
        this->setCurrentIndex(i); // 设置为当前选中项
        return; // 完成设置
      }
    }

    // 如果执行到这里，说明没有找到匹配的标准宽高比，因此它必须是自定义的
    UpdateCustomItem(r); // 更新自定义项的文本和数据以反映这个新的自定义宽高比
    dont_prompt_custom_par_ = true; // 暂时禁止在设置自定义宽高比时弹出对话框
    this->setCurrentIndex(this->count() - 1); // 选中最后一项（自定义项）
    dont_prompt_custom_par_ = false; // 恢复允许弹出对话框
  }

 private slots:
  /**
   * @brief 当组合框的当前选中索引发生改变时调用的槽函数。
   * @param index 新的选中项的索引。
   */
  void IndexChanged(int index) {
    if (dont_prompt_custom_par_) { // 如果当前禁止弹出自定义对话框，则直接返回
      return;
    }

    // 检测是否选中了“自定义...”项，如果是，则查询新的宽高比
    if (index == this->count() - 1) { // 如果选中的是最后一项
      // 查询自定义像素宽高比
      bool ok; // 用于指示用户是否点击了“确定”以及输入是否有效

      // 调用辅助函数 (可能在 qtutils.h 或 ratiodialog.h 中定义) 来获取用户输入的浮点数比例
      double custom_ratio = GetFloatRatioFromUser(this, tr("Set Custom Pixel Aspect Ratio"), &ok);

      if (ok) { // 如果用户点击了“确定”并且输入有效
        UpdateCustomItem(rational::fromDouble(custom_ratio)); // 更新自定义项的文本和数据
      }
      // 注意：如果用户取消或输入无效，此处没有显式恢复到之前的选项，
      // 但 QInputDialog 通常在取消时不会改变原始值，或者逻辑上会停留在“自定义...”上，
      // 直到下一次有效设置或选择其他项。
    }
  }

 private:
  /**
   * @brief 更新“自定义...”列表项的文本和关联数据。
   * @param ratio 新的自定义像素宽高比。如果 ratio 为空 (isNull)，则显示默认的“自定义...”文本。
   */
  void UpdateCustomItem(const rational& ratio) {
    const int custom_index = this->count() - 1; // 获取自定义项的索引（总是最后一项）

    if (ratio.isNull()) { // 如果提供的比例为空
      this->setItemText(custom_index, tr("Custom...")); // 设置文本为“自定义...”

      // 使用 1:1 以防止像素宽高比被实际设置为 0
      this->setItemData(custom_index, QVariant::fromValue(rational(1))); // 将用户数据设置为 1/1
    } else {
      // 使用格式化函数将自定义比例转换为用户可读的字符串，例如 "Custom (1.25)"
      this->setItemText(custom_index, VideoParams::FormatPixelAspectRatioString(tr("Custom (%1)"), ratio));
      this->setItemData(custom_index, QVariant::fromValue(ratio)); // 将实际的 rational 值存储为用户数据
    }
  }

  bool dont_prompt_custom_par_; ///< 标志位，用于在内部设置自定义 PAR 时，临时禁止 IndexChanged 槽弹出输入对话框。
};

}  // namespace olive

#endif  // PIXELASPECTRATIOCOMBOBOX_H
