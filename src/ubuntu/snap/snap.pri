
LIBS+=-lyaml-cpp

SOURCES += \
    $$PWD/project/snapcraftproject.cpp \
    $$PWD/project/snapcraftprojectfile.cpp \
    $$PWD/project/snapcraftprojectnode.cpp \
    $$PWD/project/snapcraftprojectmanager.cpp \
    $$PWD/project/snapcraftbuildconfigurationfactory.cpp \
    $$PWD/project/snapcraftbuildconfiguration.cpp

HEADERS += \
    $$PWD/project/snapcraftproject.h \
    $$PWD/project/snapcraftprojectfile.h \
    $$PWD/project/snapcraftprojectnode.h \
    $$PWD/project/snapcraftprojectmanager.h \
    $$PWD/project/snapcraftbuildconfigurationfactory.h \
    $$PWD/project/snapcraftbuildconfiguration.h

