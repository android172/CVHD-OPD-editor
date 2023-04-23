#include "main_window.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    MainWindow   w {};
    w.show();
    return a.exec();
}

void some_func(size_t* address_in /* possibly a object reference */) {
    int     some_val = 56;
    size_t* address  = address_in;
    do {
        *(address + 92) = 0; // Sets field to 0
        if (some_val == 1) return;
        some_val -= 2;

        address += 8; // Next 8 byte value

        *(address + 96) = 0; // Set this other filed to 0
    } while (some_val != 0);
}