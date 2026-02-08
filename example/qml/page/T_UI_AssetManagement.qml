import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0
import "../global"

FluScrollablePage{
    id: root
    launchMode: FluPageType.SingleTask
    header: FluText{ text: qsTr("Asset Management") ; font: FluTextStyle.Title }

    Component.onCompleted: {
        loadAssetData()
    }

    function loadAssetData() {
        var rows = [
            { _key: "1", field: qsTr("Transformer Model"), value: "" },
            { _key: "2", field: qsTr("Capacity"), value: "" },
            { _key: "3", field: qsTr("Short Circuit Impedance"), value: "" },
            { _key: "4", field: qsTr("Cooling Method"), value: "" },
            { _key: "5", field: qsTr("Winding Type"), value: "" },
            { _key: "6", field: qsTr("Manufacturer"), value: "" },
            { _key: "7", field: qsTr("Manufacturing Date"), value: "" },
            { _key: "8", field: qsTr("Commissioning Date"), value: "" },
            { _key: "9", field: qsTr("Site Information"), value: "" }
        ]
        var columns = [
            { title: qsTr("Item"), dataIndex: "field", readOnly: true, width: 140, minimumWidth: 120 },
            { title: qsTr("Content"), dataIndex: "value", editMultiline: true, minimumWidth: 220 }
        ]
        asset_table.columnSource = columns
        asset_table.dataSource = rows
    }

    function loadSensorTabs() {
        var columns = [
            { title: qsTr("Sensor Model"), dataIndex: "model", minimumWidth: 120 },
            { title: qsTr("Sensor Range"), dataIndex: "range", minimumWidth: 120 },
            { title: qsTr("Point Name"), dataIndex: "point", minimumWidth: 120 },
            { title: qsTr("Location"), dataIndex: "location", minimumWidth: 140, editMultiline: true },
            { title: qsTr("Sampling Rate"), dataIndex: "rate", minimumWidth: 120 },
            { title: qsTr("Unit"), dataIndex: "unit", minimumWidth: 80 }
        ]
        
        tab_view.appendTab("", qsTr("Temperature Points"), com_sensor_table, {cols: columns, rows: buildSensorRows()})
        tab_view.appendTab("", qsTr("Stress Points"), com_sensor_table, {cols: columns, rows: buildSensorRows()})
        tab_view.appendTab("", qsTr("Vibration Points"), com_sensor_table, {cols: columns, rows: buildSensorRows()})
        tab_view.appendTab("", qsTr("Noise Points"), com_sensor_table, {cols: columns, rows: buildSensorRows()})
    }

    function buildSensorRows(){
        var rows = []
        for(var i = 0; i < 6; i++){
            rows.push({
                _key: String(i + 1),
                model: "",
                range: "",
                point: "",
                location: "",
                rate: "",
                unit: ""
            })
        }
        return rows
    }

    Component{
        id: com_sensor_table
        Item{
            property var argument
            anchors.fill: parent
            FluTableView{
                id: table_view
                anchors.fill: parent
                Component.onCompleted: {
                    if(argument) {
                        table_view.columnSource = argument.cols
                        table_view.dataSource = argument.rows
                    }
                }
            }
        }
    }

    ColumnLayout{
        anchors.fill: parent
        spacing: 12
        Layout.margins: 16

        RowLayout{
            Layout.fillWidth: true
            Layout.preferredHeight: 360
            spacing: 12

            FluFrame{
                Layout.fillWidth: true
                Layout.fillHeight: true
                padding: 12
                ColumnLayout{
                    anchors.fill: parent
                    spacing: 10
                    FluText{ text: qsTr("Customer ledger (manual input)") ; font: FluTextStyle.BodyStrong }
                    FluTableView{
                        id: asset_table
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }
            }

            FluFrame{
                Layout.fillWidth: true
                Layout.fillHeight: true
                padding: 12
                ColumnLayout{
                    anchors.fill: parent
                    spacing: 10
                    FluText{ text: qsTr("Sensor configuration") ; font: FluTextStyle.BodyStrong }
                    FluTabView{
                        id: tab_view
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        addButtonVisibility: false
                        closeButtonVisibility: FluTabViewType.Never
                        tabWidthBehavior: FluTabViewType.SizeToContent
                        Component.onCompleted: {
                            root.loadSensorTabs()
                        }
                    }
                }
            }
        }
    }
}
