#include "app.h"
#include <cstdlib>

int main() {
    int status;
    App app;
    status = app.init();
    if (status) return status;
    return app.loop();
}
