TEMPLATE = lib
TARGET = qmaxmessenger
QT +=  core network

INCLUDEPATH += ../qtwebsockets$$QT_MAJOR_VERSION ../qmsgpack
LIBS += -L../qtwebsockets$$QT_MAJOR_VERSION -lqtwebsockets -L../qmsgpack -lqmsgpack

SOURCES += \
    api/chat.cpp \
    api/chatmessage.cpp \
    api/chatmessagereactions.cpp \
    api/connection.cpp \
    api/contact.cpp \
    api/contactsstorage.cpp \
    api/messagesqueue.cpp \
    api/rawapimessage.cpp \
    api/settings.cpp \
    models/chatmessagesmodel.cpp \
    models/chatslistmodel.cpp \
    plugin/serverconnection.cpp \
    plugin/usersession.cpp

HEADERS += \
    api/chat.h \
    api/chatmessage.h \
    api/chatmessagereactions.h \
    api/connection.h \
    api/contact.h \
    api/contactsstorage.h \
    api/messagesqueue.h \
    api/rawapimessage.h \
    api/settings.h \
    models/chatmessagesmodel.h \
    models/chatslistmodel.h \
    plugin/serverconnection.h \
    plugin/usersession.h

greaterThan(QT_MAJOR_VERSION, 5) {
    target.path = /usr/lib
} else {
    target.path = /usr/share/ru.neochapay.maximus/lib
}

INSTALLS += target
