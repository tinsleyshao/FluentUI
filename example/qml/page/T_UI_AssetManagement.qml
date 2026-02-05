import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0
import "../global"

FluScrollablePage{
    launchMode: FluPageType.SingleTask
    header: FluText{ text: qsTr("Asset Management") ; font: FluTextStyle.Title }

    ColumnLayout{
        anchors.fill: parent
        spacing: 12
        Layout.margins: 16

        FluFrame{
            Layout.fillWidth: true
            FluText{ text: qsTr("Customer ledger (manual input)") ; font: FluTextStyle.BodyStrong }
            // TODO: replace with TableView bound to actual model
            Rectangle{ height: 160; color: FluTheme.itemNormalColor }
        }

        FluFrame{
            Layout.fillWidth: true
            FluText{ text: qsTr("Sensor configuration") ; font: FluTextStyle.BodyStrong }
            Rectangle{ height: 160; color: FluTheme.itemNormalColor }
        }
    }
}
