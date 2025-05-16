#pragma once


#include "kddockwidgets/docks_export.h"
#include "kddockwidgets/KDDockWidgets.h"
#include "Item_p.h"

namespace Layouting {


///@brief An Item container that supports its child widgets to occupy arbitrary positions
///
/// This is unlike ItemBoxContainer, which is used for the default/traditional vertical/horizontal
/// layouting with nesting.
///
/// This free layout can be used to implement MDI style windows
class DOCKS_EXPORT_FOR_UNIT_TESTS ItemFreeContainer : public ItemContainer
{
public:
    Q_OBJECT
public:
    explicit ItemFreeContainer(Widget *hostWidget, ItemContainer *parent);
    explicit ItemFreeContainer(Widget *hostWidget);
    ~ItemFreeContainer() override;

    /// @brief adds the item to the specified position
    void addDockWidget(Item *item, QPoint localPt);

    void clear() override;
    void removeItem(Item *, bool hardRemove = true) override;
    void restore(Item *child) override;
    void onChildMinSizeChanged(Item *child) override;
    void onChildVisibleChanged(Item *child, bool visible) override;
};

}
