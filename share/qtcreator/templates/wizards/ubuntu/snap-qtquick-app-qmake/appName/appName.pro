TEMPLATE = app
TARGET = %{ProjectNameL}

QT += qml quick

# enables/disabled the extra targets to build a snapcraft package
# also tells the IDE this is a snapcraft project
CONFIG += snapcraft

SOURCES += main.cpp

RESOURCES += %{ProjectNameL}.qrc

QML_FILES += $$files(*.qml,true) \
             $$files(*.js,true)

CONF_FILES +=  %{ProjectNameL}.png

#show all the files in QtCreator
OTHER_FILES += $${CONF_FILES} \
               $${QML_FILES} \
               %{ProjectNameL}.desktop \
               %{ProjectNameL}.wrapper

snapcraft {
    wrapper.files = %{ProjectNameL}.wrapper
    wrapper.path  = /bin

    target.path = /bin
    INSTALLS+=target wrapper
}
