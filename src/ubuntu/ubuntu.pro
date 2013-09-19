TEMPLATE = lib
TARGET = Ubuntu
DEFINES += UBUNTU_LIBRARY

QT += network qml quick webkitwidgets script scripttools declarative

PROVIDER = Canonical

## Where the Qt Creator headers are located at
QTCREATOR_SOURCES = $$(QTC_SOURCE)
isEmpty(QTCREATOR_SOURCES):QTCREATOR_SOURCES=/usr/src/qtcreator

## Where our plugin will be compiled to
IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(IDE_BUILD_TREE):IDE_BUILD_TREE=../../builddir

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)
include($$QTCREATOR_SOURCES/src/plugins/coreplugin/coreplugin.pri)
include($$QTCREATOR_SOURCES/src/plugins/projectexplorer/projectexplorer.pri)
include($$QTCREATOR_SOURCES/src/plugins/qmlprojectmanager/qmlprojectmanager.pri)
include($$QTCREATOR_SOURCES/src/plugins/qt4projectmanager/qt4projectmanager.pri)

INCLUDEPATH += $$QTCREATOR_SOURCES/src
LIBS += -L$$[QT_INSTALL_LIBS]/qtcreator
LIBS += -L$$[QT_INSTALL_LIBS]/qtcreator/plugins/QtProject

#####################################
# required for Ubuntu Device Notifier
CONFIG += link_pkgconfig

PKGCONFIG += libudev
#####################################

#####################################
## Project files

FORMS += \
    ubuntudeviceswidget.ui \
    ubuntupackagingwidget.ui \
    ubuntusettingswidget.ui \
    ubuntusecuritypolicypickerdialog.ui

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    UbuntuProject.mimetypes.xml \
    manifest.json.template \
    myapp.json.template \
    manifestlib.js

SOURCES += \
    ubuntuplugin.cpp \
    ubuntuwelcomemode.cpp \
    ubuntuprojectapplicationwizard.cpp \
    ubuntumenu.cpp \
    ubuntuprojectmanager.cpp \
    ubuntuproject.cpp \
    ubuntuprojectfile.cpp \
    ubuntuprojectnode.cpp \
    ubunturunconfiguration.cpp \
    ubunturunconfigurationfactory.cpp \
    ubunturuncontrol.cpp \
    ubunturuncontrolfactory.cpp \
    ubuntuprojectapp.cpp \
    ubuntuversion.cpp \
    ubuntufeatureprovider.cpp \
    ubuntuversionmanager.cpp \
    ubuntuircmode.cpp \
    ubuntuapimode.cpp \
    ubuntucoreappsmode.cpp \
    ubuntuwikimode.cpp \
    ubuntupackagingmode.cpp \
    ubuntupackagingwidget.cpp \
    ubuntubzr.cpp \
    ubuntuclickmanifest.cpp \
    ubuntuwebmode.cpp \
    ubuntupastebinmode.cpp \
    ubuntudeviceswidget.cpp \
    ubuntudevicemode.cpp \
    ubuntuprocess.cpp \
    ubuntudevicenotifier.cpp \
    ubuntusettingspage.cpp \
    ubuntusettingswidget.cpp \
    ubuntusecuritypolicypickerdialog.cpp \
    ubuntupolicygroupmodel.cpp \
    ubuntupolicygroupinfo.cpp

HEADERS += \
    ubuntuplugin.h \
    ubuntu_global.h \
    ubuntuconstants.h \
    ubuntuwelcomemode.h \
    ubuntuprojectapplicationwizard.h \
    ubuntumenu.h \
    ubuntushared.h \
    ubuntuprojectmanager.h \
    ubuntuproject.h \
    ubuntuprojectfile.h \
    ubuntuprojectnode.h \
    ubunturunconfiguration.h \
    ubunturunconfigurationfactory.h \
    ubunturuncontrol.h \
    ubunturuncontrolfactory.h \
    ubuntuprojectapp.h \
    ubuntuversion.h \
    ubuntufeatureprovider.h \
    ubuntuversionmanager.h \
    ubuntuircmode.h \
    ubuntuapimode.h \
    ubuntucoreappsmode.h \
    ubuntuwikimode.h \
    ubuntupackagingmode.h \
    ubuntupackagingwidget.h \
    ubuntubzr.h \
    ubuntuclickmanifest.h \
    ubuntuwebmode.h \
    ubuntupastebinmode.h \
    ubuntudevicemode.h \
    ubuntudeviceswidget.h \
    ubuntuprocess.h \
    ubuntudevicenotifier.h \
    ubuntusettingspage.h \
    ubuntusettingswidget.h \
    ubuntusecuritypolicypickerdialog.h \
    ubuntupolicygroupmodel.h \
    ubuntupolicygroupinfo.h
