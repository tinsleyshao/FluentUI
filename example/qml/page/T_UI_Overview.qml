import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import "../global"

FluPage{
    launchMode: FluPageType.SingleTask

    contentMargins: 0

    Rectangle {
        id: bgContainer
        anchors.fill: parent
        color: FluTheme.backgroundColor

        Image {
            id: bg
            width: parent.width
            height: parent.height
            fillMode: Image.PreserveAspectCrop
            source: "qrc:/example/res/image/bg_overview_header.png"
            asynchronous: true
        }
    }

    FluClip{
        id: banner
        width: Math.max(300, Math.min(bgContainer.width - 80, 600))
        height: 120
        radius: 8
        anchors{
            top: bgContainer.top
            horizontalCenter: bgContainer.horizontalCenter
            topMargin: 40
        }
        z: 10

        FluAcrylic{
            anchors.fill: parent
            target: bg
            tintColor: FluTheme.dark ? Qt.rgba(0,0,0,1) : Qt.rgba(1,1,1,1)
            tintOpacity: FluTheme.dark ? 0.5 : 0.6
            blurRadius: 32
            targetX: banner.mapToItem(bgContainer, 0, 0).x
            targetY: banner.mapToItem(bgContainer, 0, 0).y
            targetWidth: banner.width
            targetHeight: banner.height
        }

        FluText{
            text: qsTr("Welcome to Transformer Warning System")
            font: FluTextStyle.TitleLarge
            color: FluTheme.dark ? Qt.rgba(1,1,1,0.95) : Qt.rgba(0,0,0,0.95)
            anchors.centerIn: parent
            font.weight: Font.DemiBold
            elide: Text.ElideRight
            width: parent.width - 20
        }
    }
}


