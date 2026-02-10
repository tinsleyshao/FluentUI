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
        source: "qrc:/example/res/image/bg_overview_header.png"
        asynchronous: true
    }

    FluAcrylic{
        target: bg
        width: parent.width - 80
        height: 120
        anchors{
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            topMargin: 40
        }
        radius: 8
        
        FluText{
            text: qsTr("Welcome to Transformer Warning System")
            font: FluTextStyle.TitleLarge
            color: FluTheme.dark ? Qt.rgba(1,1,1,0.9) : Qt.rgba(0,0,0,0.9)
            anchors.centerIn: parent
            font.weight: Font.DemiBold
        }
    }
}
