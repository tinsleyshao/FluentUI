import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0
import "../global"

FluContentPage{
    id: root
    launchMode: FluPageType.SingleTask
    header: FluText{ text: qsTr("Asset Management") ; font: FluTextStyle.Title }
    
    property var assetData: []

    Component.onCompleted: {
        loadAssetData()
    }

    function loadAssetData() {
        assetData = [
            { _key: "1", itemName: qsTr("Transformer Model"), itemValue: "Model A" },
            { _key: "2", itemName: qsTr("Capacity"), itemValue: "100 MVA" },
            { _key: "3", itemName: qsTr("Short Circuit Impedance"), itemValue: "12%" },
            { _key: "4", itemName: qsTr("Cooling Method"), itemValue: "ONAN" },
            { _key: "5", itemName: qsTr("Winding Type"), itemValue: "YNyn0" },
            { _key: "6", itemName: qsTr("Manufacturer"), itemValue: "ABC Corp" },
            { _key: "7", itemName: qsTr("Manufacturing Date"), itemValue: "2020-01-15" },
            { _key: "8", itemName: qsTr("Commissioning Date"), itemValue: "2020-06-20" },
            { _key: "9", itemName: qsTr("Site Information"), itemValue: "Station X" }
        ]
        asset_table.dataSource = assetData
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
        
        var sensorData = buildSensorRows()
        tab_view.appendTab("", qsTr("Temperature Points"), com_sensor_table, {cols: columns, rows: sensorData})
        
        sensorData = buildSensorRows()
        tab_view.appendTab("", qsTr("Stress Points"), com_sensor_table, {cols: columns, rows: sensorData})
        
        sensorData = buildSensorRows()
        tab_view.appendTab("", qsTr("Vibration Points"), com_sensor_table, {cols: columns, rows: sensorData})
        
        sensorData = buildSensorRows()
        tab_view.appendTab("", qsTr("Noise Points"), com_sensor_table, {cols: columns, rows: sensorData})
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
                onEditFinished: (row, column, dataIndex, newValue) => {
                    console.log("传感器表编辑完成:", "行=" + row + ", 列=" + dataIndex + ", 新值=" + newValue)
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
                        columnSource: [
                            { title: qsTr("Item"), dataIndex: "itemName", readOnly: true },
                            { title: qsTr("Content"), dataIndex: "itemValue" }
                        ]
                        onEditFinished: (row, column, dataIndex, newValue) => {
                            console.log("编辑完成:", "行=" + row + ", 列=" + dataIndex + ", 新值=" + newValue)
                        }
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
                    Item{
                         Layout.fillWidth: true
                         Layout.fillHeight: true
                         FluTabView{
                             id: tab_view
                             anchors.fill: parent
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
}
