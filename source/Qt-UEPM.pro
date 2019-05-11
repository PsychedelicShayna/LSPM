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
    main.cxx \
    json.txx \
    load_file_dialogue.cxx \
    main_window_dialogue.cxx \
    password_generator.cxx \
    password_generator_dialogue.cxx \
    wallet.cxx \
    new_account_dialogue.cxx \
    logger.cxx \
    save_file_dialogue.cxx \
    basic_aes.cxx \
    settings_dialogue.cxx

HEADERS += \
    rainbowtable.hxx \
    load_file_dialogue.hxx \
    main_window_dialogue.hxx \
    password_generator.hxx \
    password_generator_dialogue.hxx \
    wallet.hxx \
    new_account_dialogue.hxx \
    logger.hxx \
    save_file_dialogue.hxx \
    basic_aes.hxx \
    settings_dialogue.hxx

FORMS += \
    load_file_dialogue.ui \
    main_window_dialogue.ui \
    password_generator_dialogue.ui \
    new_account_dialogue.ui \
    save_file_dialogue.ui \
    settings_dialogue.ui

win32:CONFIG(debug, debug|release):  DEFINES += DEBUG
else:win32:CONFIG(release, debug|release):  DEFINES += RELEASE

# ====================================================================================================
#       OpenSSL & Dependancies library configuration.
# ====================================================================================================
INCLUDEPATH += C:/OpenSSL-Win64/include/
DEPENDPATH += C:/OpenSSL-Win64/include/

LIBS += -LC:/OpenSSL-Win64/lib/VC/static/
LIBS += -lUser32 -lAdvapi32

win32:CONFIG(release, debug|release): LIBS += -llibcrypto64MD
else:win32:CONFIG(debug, debug|release): LIBS += -llibcrypto64MDd
# ====================================================================================================

RC_ICONS += Icon.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


# win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../OpenSSL-Win64/lib/VC/static/ -llibcrypto64MD
# else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../OpenSSL-Win64/lib/VC/static/ -llibcrypto64MDd
#
# INCLUDEPATH += $$PWD/../../../../../../../OpenSSL-Win64/include/
# DEPENDPATH += $$PWD/../../../../../../../OpenSSL-Win64/include/
#
# win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../../OpenSSL-Win64/lib/VC/static/liblibcrypto64MD.a
# else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../../OpenSSL-Win64/lib/VC/static/liblibcrypto64MDd.a
# else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../../OpenSSL-Win64/lib/VC/static/libcrypto64MD.lib
# else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../../OpenSSL-Win64/lib/VC/static/libcrypto64MDd.lib

# LIBS += -L"C:\\OpenSSL-Win64\\lib\\VC\\static\\" -llibssl64MDd
# LIBS += -lUser32
# LIBS += -lAdvapi32
