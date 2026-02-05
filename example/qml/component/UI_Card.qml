import QtQuick 2.15
import FluentUI 1.0

FluFrame{
    property alias contentItem: content
    property int padding: 12
    id: root
    width: parent ? parent.width : 300
    Column{
        id: content
        anchors.fill: parent
        anchors.margins: root.padding
    }
}
