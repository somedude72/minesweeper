#include "app/app.h"

int main(int argc, char** argv) {
    App instance(argc, argv);
    return instance.exec();
}
