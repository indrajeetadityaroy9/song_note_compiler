#include "gui/Application.h"
#include <cstdio>

int main(int, char**) {
    Application app;

    if (!app.initialize()) {
        fprintf(stderr, "Failed to initialize application\n");
        return 1;
    }

    app.run();

    return 0;
}
