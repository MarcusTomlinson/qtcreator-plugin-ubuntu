import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0 as Controls

import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1

import Ubuntu.DevicesModel 0.1

ColumnLayout {
    UbuntuListView {
        Layout.alignment: Qt.AlignLeft | Qt.AlignTop
        width: units.gu(50)
        height: childrenRect.height
        model: VisualItemModel {
            ListItem.SingleValue {
                text: i18n.tr("Ubuntu version")
                value: emuUbuntuVersion
            }
            ListItem.SingleValue {
                text: i18n.tr("Device version")
                value: emuDeviceVersion
            }
            ListItem.SingleValue {
                text: i18n.tr("Image version")
                value: emuImageVersion
            }
            ListItem.Standard {
                //show this listitem only when device is not connected
                visible: connectionState !== DeviceConnectionState.ReadyToUse && connectionState !== DeviceConnectionState.Connected
                text: "Scale"
                control: Controls.ComboBox {
                    id: emulatorScaleComboBox
                    model: ["1.0", "0.9", "0.8", "0.7", "0.6","0.5", "0.4", "0.3", "0.2","0.1"]
                    currentIndex: {
                        var idx = find(emulatorScaleFactor);
                        return idx >= 0 ? idx : 0;
                    }
                    onActivated: {
                        emulatorScaleFactor = textAt(index);
                    }
                }
            }

            ListItem.Standard {
                //show this listitem only when device is not connected
                visible: connectionState !== DeviceConnectionState.ReadyToUse && connectionState !== DeviceConnectionState.Connected
                text: "Memory"
                control: Controls.ComboBox {
                    id: emulatorMemoryComboBox
                    model: ["512", "768", "1024"]

                    currentIndex: {
                        var idx = find(emulatorMemorySetting);
                        return idx >= 0 ? idx : 0;
                    }
                    onActivated: {
                        emulatorMemorySetting = textAt(index);
                    }
                }
            }
        }
    }
    ListItem.Divider{}
    DeviceKitManager{
        Layout.alignment: Qt.AlignLeft | Qt.AlignTop
        Layout.fillHeight: true
        width: units.gu(50)
    }
}