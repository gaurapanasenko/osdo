#include "app.h"

int main() {
    int status;
    App app;
    status = app.init();
    if (status) return status;
    return app.loop();
}
