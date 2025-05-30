#ifndef EXAMPLEDOCKABLEWIDGET_H
#define EXAMPLEDOCKABLEWIDGET_H

#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
class QPainter;
QT_END_NAMESPACE

class MyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MyWidget(const QString &backgroundFile, const QString &logoFile, QWidget *parent = nullptr);
    ~MyWidget();

    // These two are just for demonstrating how to block the close event, if desired
    void blockCloseEvent();
    void closeEvent(QCloseEvent *) override;

protected:
    void drawLogo(QPainter &);
    QImage m_background;
    QImage m_logo;
    bool m_blocksCloseEvent = false;
};

class MyWidget1 : public MyWidget
{
    Q_OBJECT
public:
    explicit MyWidget1(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
};

class MyWidget2 : public MyWidget
{
    Q_OBJECT
public:
    explicit MyWidget2(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
};

class MyWidget3 : public MyWidget
{
    Q_OBJECT
public:
    explicit MyWidget3(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    QImage m_triangle;
};


#endif
