# Auto-generated by IDE. All changes by user will be lost!
# Created at 28.01.13 12:35

BASEDIR = $$_PRO_FILE_PWD_

INCLUDEPATH +=  \
    $$BASEDIR/src \
    $$BASEDIR/src/RoundProgress

SOURCES +=  \
    $$BASEDIR/src/RoundProgress/RoundProgress.cpp \
    $$BASEDIR/src/TestRoundProgress.cpp \
    $$BASEDIR/src/main.cpp

HEADERS +=  \
    $$BASEDIR/src/RoundProgress/RoundProgress.hpp \
    $$BASEDIR/src/TestRoundProgress.hpp

CONFIG += precompile_header
PRECOMPILED_HEADER = $$BASEDIR/precompiled.h

lupdate_inclusion {
    SOURCES += \
        $$BASEDIR/../assets/*.qml
}

LIBS += -lbb

TRANSLATIONS = \
    $${TARGET}.ts

