#include <QApplication>

#include <openssl/sha.h>
#include <openssl/evp.h>

#include "headers/main_window_dialogue.hxx"

int main(int argc, char** argv) {
    QApplication application(argc, argv);

    MainWindow main_window;
    main_window.show();

    return application.exec();
}






















