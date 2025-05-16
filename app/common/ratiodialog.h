#ifndef RATIODIALOG_H
#define RATIODIALOG_H

#include <QInputDialog>  // Qt 输入对话框类
#include <QString>       // 为了 QString title 参数
#include <QWidget>       // 为了 QWidget* parent 参数

namespace olive {

/**
 * @brief 弹出一个对话框，提示用户输入一个浮点数比例值。
 *
 * 此函数通常会使用 QInputDialog::getDouble() 或类似机制来获取用户的输入。
 *
 * @param parent 指向父 QWidget 对象的指针，对话框将显示在该父窗口之上。
 * 如果为 nullptr，对话框通常会是顶级窗口。
 * @param title 对话框的标题字符串。
 * @param ok_in 指向布尔值的指针 (输出参数)。如果用户点击了“确定”按钮，
 * 则此布尔值将被设置为 true；如果用户点击了“取消”按钮或关闭了对话框，
 * 则被设置为 false。调用者应检查此值以确定输入是否有效。
 * @return double 用户输入的浮点数比例值。如果用户取消了操作 (即 *ok_in 为 false)，
 * 则此返回值的意义未定义或可能是一个默认值 (例如0.0)。
 */
double GetFloatRatioFromUser(QWidget* parent, const QString& title, bool* ok_in);

}  // namespace olive

#endif  // RATIODIALOG_H