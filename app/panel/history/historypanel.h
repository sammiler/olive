#ifndef HISTORYPANEL_H  // 防止头文件被重复包含的宏
#define HISTORYPANEL_H  // 定义 HISTORYPANEL_H 宏

#include "panel/panel.h"  // 包含 PanelWidget 基类的定义

namespace olive {  // olive 项目的命名空间

/**
 * @brief HistoryPanel 类代表一个用于显示项目操作历史 (撤销/重做栈) 的面板。
 *
 * 它继承自 PanelWidget，表明它是一个标准的 Olive 编辑器面板。
 * 这个面板通常会列出用户执行过的可撤销操作，允许用户点击某个历史记录项来
 * 回退到该状态，或者重做已撤销的操作。
 *
 * 内部实现通常会与 UndoManager 或 UndoStack 这类撤销系统进行交互，
 * 获取历史记录数据并展示在UI上 (例如使用 QListView 或 QTreeView)。
 */
class HistoryPanel : public PanelWidget {  // HistoryPanel 继承自 PanelWidget
 Q_OBJECT                                  // 声明此类使用 Qt 的元对象系统

     public :
     // 构造函数
     HistoryPanel();

 protected:
  /**
   * @brief 重写基类的 Retranslate 方法，用于在语言更改时更新此面板内UI元素的文本。
   * 例如，可能会更新面板标题或内部控件的标签。
   */
  void Retranslate() override;

  // 注意：实际显示历史记录的UI控件 (如 QListView) 通常会在 .cpp 文件中创建并添加到此面板中。
  // 此头文件主要声明面板的类结构和基本接口。
};

}  // namespace olive

#endif  // HISTORYPANEL_H