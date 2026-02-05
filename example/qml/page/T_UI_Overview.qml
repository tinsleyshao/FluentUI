import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0
import "../global"

FluScrollablePage{
    launchMode: FluPageType.SingleTask
    header: FluText{ text: qsTr("Overview") ; font: FluTextStyle.Title }

    ColumnLayout{
        anchors.fill: parent
        spacing: 12
        Layout.margins: 16

        FluFrame{
            Layout.fillWidth: true
            FluText{ text: qsTr("Current health level") ; font: FluTextStyle.BodyStrong }
            FluText{ text: qsTr("Health index: HH (sample)") ; color: FluColors.Grey120 }
        }

        FluFrame{
            Layout.fillWidth: true
            FluText{ text: qsTr("Recent warnings") ; font: FluTextStyle.BodyStrong }
            ListView{ height: 120 }
        }
    }
}
