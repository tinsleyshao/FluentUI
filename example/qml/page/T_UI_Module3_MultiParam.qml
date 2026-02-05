import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0
import "../global"

FluScrollablePage{
    launchMode: FluPageType.SingleTask
    header: FluText{ text: qsTr("Module 3 - Multi-parameter Fusion") ; font: FluTextStyle.Title }

    ColumnLayout{
        anchors.fill: parent
        spacing: 12
        Layout.margins: 16

        FluFrame{
            Layout.fillWidth: true
            FluText{ text: qsTr("DGA / Oil / PF / Impedance inputs") ; font: FluTextStyle.BodyStrong }
            Rectangle{ height: 160; color: FluTheme.itemNormalColor }
        }

        FluFrame{
            Layout.fillWidth: true
            FluText{ text: qsTr("Fusion result and explanation") ; font: FluTextStyle.BodyStrong }
            Rectangle{ height: 200; color: FluTheme.itemNormalColor }
        }
    }
}
