#include "app.h"

int main() {
    int status;
    App app;
    status = app_init(&app);
    if (status) return status;
    return app_loop(&app);
}
