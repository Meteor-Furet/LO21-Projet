QT+=widgets
QT+=xml

QMAKE_CXXFLAGS=-std=c++11
QMAKE_LFLAGS=-std=c++11

SOURCES += \
    main.cpp \
    note.cpp \
    noteediteur.cpp \
    relation.cpp

HEADERS += \
    note.h \
    noteediteur.h \
    relation.h
