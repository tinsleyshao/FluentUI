import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0
import "../global"

FluScrollablePage{
    launchMode: FluPageType.SingleTask
    header: FluText{ text: qsTr("Module 2 - Vib/Noise Processing") ; font: FluTextStyle.Title }

    ColumnLayout{
        anchors.fill: parent
        spacing: 12
        Layout.margins: 16

        FluFrame{
            Layout.fillWidth: true
            FluText{ text: qsTr("Data import") ; font: FluTextStyle.BodyStrong }
            Rectangle{ height: 120; color: FluTheme.itemNormalColor }
        }

        FluFrame{
            Layout.fillWidth: true
            FluText{ text: qsTr("Waveform / PSD") ; font: FluTextStyle.BodyStrong }
            Rectangle{ height: 200; color: FluTheme.itemNormalColor }
        }

        FluFrame{
            Layout.fillWidth: true
            FluText{ text: qsTr("Health metric (HH)") ; font: FluTextStyle.BodyStrong }
            Rectangle{ height: 120; color: FluTheme.itemNormalColor }
        }
    }
}
