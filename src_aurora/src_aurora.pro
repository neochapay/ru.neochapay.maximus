TARGET = ru.neochapay.maximus

CONFIG += \
    auroraapp

INCLUDEPATH += ../libs/qtwebsockets5 \
               ../libs/qmaxmessenger

LIBS += -L../libs/qtwebsockets5 -lqtwebsockets \
        -L../libs/qmaxmessenger/ -lqmaxmessenger

SOURCES += \
    emojimodel.cpp \
    main.cpp

RESOURCES += qml.qrc

HEADERS += \
    emojimodel.h

DISTFILES += \
    qml/* \
    qml/pages/* \
    qml/covers/* \
    qml/components/* \
    qml/jsFunctions/* \
    translations/*.ts

AURORAAPP_ICONS = 86x86 108x108 128x128 172x172 256x256

CONFIG += auroraapp_i18n

TRANSLATIONS += \
    translations/ru.neochapay.maximus.ts \
    translations/ru.neochapay.maximus-ru.ts
    
VERSION = $$system( egrep "^Version:\|^Release:" ../rpm/ru.neochapay.maximus.spec  |tr -d "[A-Z][a-z]: " | tr "\\\n" "-" | sed "s/\.$//g"| tr -d "[:space:]")
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
