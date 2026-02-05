import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0
import "../global"

FluScrollablePage{
    launchMode: FluPageType.SingleTask
    header: FluText{ text: qsTr("Module 1 - Regular Check") ; font: FluTextStyle.Title }

    ColumnLayout{
        anchors.fill: parent
        spacing: 12
        Layout.margins: 16

        FluFrame{
            Layout.fillWidth: true
            FluText{ text: qsTr("Inputs") ; font: FluTextStyle.BodyStrong }
            Rectangle{ height: 120; color: FluTheme.itemNormalColor }
        }

        FluFrame{
            Layout.fillWidth: true
            FluText{ text: qsTr("Model calculation") ; font: FluTextStyle.BodyStrong }
            Rectangle{ height: 200; color: FluTheme.itemNormalColor }
        }

        FluFrame{
            Layout.fillWidth: true
            FluText{ text: qsTr("Output / Check results") ; font: FluTextStyle.BodyStrong }
            Rectangle{ height: 120; color: FluTheme.itemNormalColor }
        }
    }
}
