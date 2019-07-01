#include <QApplication>

#include <openssl/sha.h>
#include <openssl/evp.h>

#include "headers/main_window_dialogue.hxx"

/*
 * This comment should only appear on the development branch. */

int main(int argc, char** argv) {
    QApplication application(argc, argv);

    MainWindow main_window;
    main_window.show();

    return application.exec();
}