#-------------------------------------------------
#
# Project created by QtCreator 2019-03-13T22:09:15
#
#-------------------------------------------------

QT += core gui gui-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LSPM
TEMPLATE = app

QMAKE_CXXF

# QMAKE_LIBS_CORE = kernel32.lib user32.lib shell32.lib uuid.lib ole32.lib advapi32.lib ws2_32.lib

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++17

SOURCES += \
    crypto.cxx \
    main.cxx \
    main_window_dialog.cxx \
    password_generator_dialog.cxx

HEADERS += \
    headers/main_window_dialog.hxx \
    headers/password_generator_dialog.hxx \
    headers/crypto.hxx \
    headers/json.hxx

FORMS += \
    uis/load_safe_dialogue.ui \
    uis/main_window_dialog.ui \
    uis/password_generator_dialog.ui

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
