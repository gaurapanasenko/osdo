#ifndef BEZIATORPANEL_H
#define BEZIATORPANEL_H

#include <beziator.h>

class BeziatorPanel : public Beziator
{
public:
    BeziatorPanel(const char *name, shared_ptr<Shader> editmode);

    void edit_panel(mat4 matr);
};

#endif // BEZIATORPANEL_H
