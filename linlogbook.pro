DEFINES += QT_NO_CAST_FROM_ASCII
SUBDIRS += src
TEMPLATE = subdirs 
CONFIG += warn_on debug \
          qt \
          thread 
QT += sql \
 network

QMAKE_CXXFLAGS_DEBUG += -O0 \
-g3