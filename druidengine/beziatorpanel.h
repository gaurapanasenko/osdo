#ifndef BEZIATORPANEL_H
#define BEZIATORPANEL_H

#include <beziator.h>

class BeziatorPanel : public Beziator
{
public:
    BeziatorPanel(const string& path);

    void edit_panel() override;
};

#endif // BEZIATORPANEL_H
