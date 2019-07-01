#include "headers/main_window_dialogue.hxx"
#include "ui_main_window_dialogue.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this); 
}

MainWindow::~MainWindow() {
    delete ui;
}


