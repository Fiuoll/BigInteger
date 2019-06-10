TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    bigint.cpp \
        main.cpp \
    biguint.cpp \
    moduint.cpp \
    parser.cpp \
    primes.cpp

HEADERS += \
    bigint.h \
    biguint.h \
    moduint.h \
    parser.h \
    primes.h
