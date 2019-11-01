QT += core widgets gui gui-private

TARGET = LSPM
TEMPLATE = app

CONFIG += c++17

# Defines DEBUG/RELEASE macro, depending on the build mode.
CONFIG(debug, debug|release):DEFINES += DEBUG
CONFIG(release, debug|release):DEFINES += RELEASE

# Icon used for the binary.
RC_ICONS += resources/LSPM.ico

SOURCES += \
    source/main.cxx \
    source/crypto.cxx \
    source/main_dlg.cxx \
    source/generator_dlg.cxx

HEADERS += \
    source/headers/crypto.hxx \
    source/headers/json.hxx \
    source/headers/main_dlg.hxx \
    source/headers/generator_dlg.hxx

FORMS += \
    source/uis/main_dlg.ui \
    source/uis/generator_dlg.ui

# Windows library linking, and include paths.
win32 {
    # OpenSSL Include directory.
    INCLUDEPATH += C:/OpenSSL-Win64/include/
    DEPENDPATH += C:/OpenSSL-Win64/include/

    # Separate Windows libraries that OpenSSL requires to link properly.
    LIBS += -lUser32 -lAdvapi32

    # OpenSSL Library directory.
    LIBS += -LC:/OpenSSL-Win64/lib/VC/static/

    # OpenSSL Static library linkage (debug/release).
    CONFIG(release, debug|release): LIBS += -llibcrypto64MD
    CONFIG(debug, debug|release): LIBS += -llibcrypto64MDd
}

# Unix library linking, and include paths.
unix {
    INCLUDEPATH += /usr/local/include/
    DEPENDPATH += /usr/local/include/
    LIBS += -L/usr/local/lib/

    # libcrypto.a Should be in /usr/local/lib
    LIBS += -lcrypto
}
