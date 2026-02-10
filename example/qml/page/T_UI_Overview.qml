import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import "../global"

FluScrollablePage{
    id: page
    launchMode: FluPageType.SingleTask
    header: Item{}

    Image {
        id: bg
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        source: "qrc:/example/res/image/bg_overview_header.png"
        asynchronous: true
    }

    FluClip{
        id: banner
        width: page.width - 80
        height: 120
        radius: 8
        anchors{
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            topMargin: 40
        }

        FluAcrylic{
            anchors.fill: parent
            target: bg
            tintColor: FluTheme.dark ? Qt.rgba(0,0,0,1) : Qt.rgba(1,1,1,1)
            tintOpacity: FluTheme.dark ? 0.5 : 0.6
            blurRadius: 32
            targetRect: Qt.rect(banner.x, banner.y, banner.width, banner.height)
        }

        FluText{
            text: qsTr("Welcome to Transformer Warning System")
            font: FluTextStyle.TitleLarge
            color: FluTheme.dark ? Qt.rgba(1,1,1,0.95) : Qt.rgba(0,0,0,0.95)
            anchors.centerIn: parent
            font.weight: Font.DemiBold
        }
    }
}


