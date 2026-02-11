import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import "../global"

FluPage{
    launchMode: FluPageType.SingleTask
    header: Item{}

    Image {
        id: bg
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        source: "qrc:/example/res/image/bg_overview_header.png"
        asynchronous: true
    }
}


