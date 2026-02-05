import QtQuick 2.15
import FluentUI 1.0

Item{
    property alias label: lbl.text
    width: parent ? parent.width : 400
    RowLayout{
        anchors.fill: parent
        spacing: 8
        FluText{ id: lbl; text: qsTr("Label") ; Layout.preferredWidth: 140 }
        FluTextField{ id: input; Layout.fillWidth: true }
    }
}
