import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0
import "../global"

FluScrollablePage{
    launchMode: FluPageType.SingleTask
    header: FluText{ text: qsTr("Report Export") ; font: FluTextStyle.Title }

    ColumnLayout{
        anchors.fill: parent
        spacing: 12
        Layout.margins: 16

        FluFrame{
            Layout.fillWidth: true
            FluText{ text: qsTr("One-click generate report") ; font: FluTextStyle.BodyStrong }
            RowLayout{ spacing: 8
                FluButton{ text: qsTr("Export PDF") }
                FluButton{ text: qsTr("Export Word") }
            }
        }
    }
}
