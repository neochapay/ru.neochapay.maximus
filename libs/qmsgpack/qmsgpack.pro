TARGET = qmsgpack
TEMPLATE = lib

SOURCES += \
    msgpack.cpp \
    msgpackcommon.cpp \
    private/pack_p.cpp \
    private/unpack_p.cpp \
    private/qt_types_p.cpp \
    stream/time.cpp \
    stream/geometry.cpp \
    msgpackstream.cpp

target.path = /usr/share/ru.neochapay.maximus/lib

INSTALLS += target
