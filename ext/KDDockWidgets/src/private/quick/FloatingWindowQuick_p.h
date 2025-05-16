#ifndef KD_FLOATING_WINDOWQUICK_P_H
#define KD_FLOATING_WINDOWQUICK_P_H

#ifdef KDDOCKWIDGETS_QTQUICK


#include "../FloatingWindow_p.h"

class QQuickView;

namespace KDDockWidgets {

class DOCKS_EXPORT FloatingWindowQuick : public FloatingWindow
{
    Q_OBJECT
public:
    explicit FloatingWindowQuick(MainWindowBase *parent = nullptr, FloatingWindowFlags flags = FloatingWindowFlag::FromGlobalConfig);
    explicit FloatingWindowQuick(Frame *frame, QRect suggestedGeometry, MainWindowBase *parent = nullptr);
    ~FloatingWindowQuick();

    QSize minimumSize() const override;

protected:
    void setGeometry(QRect) override;

private:
    int contentsMargins() const;
    int titleBarHeight() const;
    QWindow *candidateParentWindow() const;
    void init();
    QQuickView *const m_quickWindow;
    QQuickItem *m_visualItem = nullptr;
    Q_DISABLE_COPY(FloatingWindowQuick)
};

}

#endif

#endif
