#ifndef KD_DROP_AREA_WITH_CENTRAL_FRAME_P_H
#define KD_DROP_AREA_WITH_CENTRAL_FRAME_P_H

#include "DropArea_p.h"

#include "kddockwidgets/QWidgetAdapter.h"

namespace KDDockWidgets {
class DOCKS_EXPORT DropAreaWithCentralFrame : public DropArea
{
    Q_OBJECT
public:
    explicit DropAreaWithCentralFrame(QWidgetOrQuick *parent = {}, MainWindowOptions options = MainWindowOption_HasCentralFrame);
    ~DropAreaWithCentralFrame() override;

    static Frame *createCentralFrame(MainWindowOptions options);

private:
    friend class MainWindowBase;
    friend class Frame;
    friend class ::TestDocks;
    Frame *const m_centralFrame = nullptr;
};

}

#endif
