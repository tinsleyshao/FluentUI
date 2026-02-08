import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0
import "../global"

FluScrollablePage{
    launchMode: FluPageType.SingleTask
    header: Item{}

    Image {
        id: bg
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        source: "qrc:/example/res/image/bg_home_header.png"
        asynchronous: true
        z: -1
    }

    ColumnLayout{
        anchors.fill: parent
        spacing: 12
        Layout.margins: 16

        Item{
            Layout.fillWidth: true
            Layout.preferredHeight: 260
            Rectangle{
                anchors.fill: parent
                gradient: Gradient{
                    GradientStop { position: 0.8; color: FluTheme.dark ? Qt.rgba(0,0,0,0) : Qt.rgba(1,1,1,0) }
                    GradientStop { position: 1.0; color: FluTheme.dark ? Qt.rgba(0,0,0,1) : Qt.rgba(1,1,1,1) }
                }
            }
            FluText{
                text: qsTr("Overview")
                font: FluTextStyle.TitleLarge
                anchors{
                    top: parent.top
                    left: parent.left
                    topMargin: 20
                    leftMargin: 20
                }
            }
        }
    }
}
