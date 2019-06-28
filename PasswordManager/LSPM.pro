#-------------------------------------------------
#
# Project created by QtCreator 2019-03-13T22:09:15
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Qt-UEPW
TEMPLATE = app

QMAKE_CXXF

QMAKE_LIBS_CORE = kernel32.lib user32.lib shell32.lib uuid.lib ole32.lib advapi32.lib ws2_32.lib

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++17

SOURCES += \
    source/main.cxx \
    source/wallet.cxx \
    source/basic_aes.cxx \
    source/settings_dialogue.cxx \
    source/password_generator.cxx \
    source/save_file_dialogue.cxx \
    source/load_file_dialogue.cxx \
    source/main_window_dialogue.cxx \
    source/new_account_dialogue.cxx \
    source/password_generator_dialogue.cxx

HEADERS += \
    source/headers/json.hxx \
    source/headers/macros.hxx \
    source/headers/wallet.hxx \
    source/headers/basic_aes.hxx \
    source/headers/settings_dialogue.hxx \
    source/headers/save_file_dialogue.hxx \
    source/headers/password_generator.hxx \
    source/headers/load_file_dialogue.hxx \
    source/headers/main_window_dialogue.hxx \
    source/headers/new_account_dialogue.hxx \
    source/headers/password_generator_dialogue.hxx

FORMS += \
    source/uis/settings_dialogue.ui \
    source/uis/save_file_dialogue.ui \
    source/uis/load_file_dialogue.ui \
    source/uis/main_window_dialogue.ui \
    source/uis/new_account_dialogue.ui \
    source/uis/password_generator_dialogue.ui

RC_ICONS += ./resources/LSPM.ico

# Add debug/release macros depending on the build mode.
win32:CONFIG(debug, debug|release):  DEFINES += DEBUG
else:win32:CONFIG(release, debug|release):  DEFINES += RELEASE

# ====================================================================================================
#       OpenSSL Library Importation
# ====================================================================================================
INCLUDEPATH += C:/OpenSSL-Win64/include/
DEPENDPATH += C:/OpenSSL-Win64/include/

LIBS += -LC:/OpenSSL-Win64/lib/VC/static/
LIBS += -lUser32 -lAdvapi32

win32:CONFIG(release, debug|release): LIBS += -llibcrypto64MD
else:win32:CONFIG(debug, debug|release): LIBS += -llibcrypto64MDd
# ====================================================================================================

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target