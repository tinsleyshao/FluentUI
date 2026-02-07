import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0
import "../global"

FluScrollablePage{
    id: root
    launchMode: FluPageType.SingleTask
    header: FluText{ text: qsTr("Asset Management") ; font: FluTextStyle.Title }

    property var assetRows: [
        { _key: "1", field: qsTr("变压器型号"), value: "" },
        { _key: "2", field: qsTr("容量"), value: "" },
        { _key: "3", field: qsTr("短路阻抗"), value: "" },
        { _key: "4", field: qsTr("冷却方式"), value: "" },
        { _key: "5", field: qsTr("绕组型式"), value: "" },
        { _key: "6", field: qsTr("厂家"), value: "" },
        { _key: "7", field: qsTr("出厂日期"), value: "" },
        { _key: "8", field: qsTr("投运日期"), value: "" },
        { _key: "9", field: qsTr("站点信息"), value: "" }
    ]

    property var assetColumns: [
        { title: qsTr("项目"), dataIndex: "field", readOnly: true, width: 140, minimumWidth: 120 },
        { title: qsTr("内容"), dataIndex: "value", editMultiline: true, minimumWidth: 220 }
    ]

    property var sensorColumns: [
        { title: qsTr("传感器型号"), dataIndex: "model", minimumWidth: 120 },
        { title: qsTr("传感器量程"), dataIndex: "range", minimumWidth: 120 },
        { title: qsTr("点位名称"), dataIndex: "point", minimumWidth: 120 },
        { title: qsTr("位置描述"), dataIndex: "location", minimumWidth: 140, editMultiline: true },
        { title: qsTr("采样频率"), dataIndex: "rate", minimumWidth: 120 },
        { title: qsTr("单位"), dataIndex: "unit", minimumWidth: 80 }
    ]

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
            anchors.fill: parent
            FluTableView{
                id: table_view
                anchors.fill: parent
                columnSource: root.sensorColumns
                Component.onCompleted: {
                    table_view.dataSource = argument
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
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        columnSource: root.assetColumns
                        dataSource: root.assetRows
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
                            tab_view.appendTab("", qsTr("温度点位"), com_sensor_table, root.buildSensorRows())
                            tab_view.appendTab("", qsTr("应力点位"), com_sensor_table, root.buildSensorRows())
                            tab_view.appendTab("", qsTr("振动点位"), com_sensor_table, root.buildSensorRows())
                            tab_view.appendTab("", qsTr("噪声点位"), com_sensor_table, root.buildSensorRows())
                        }
                    }
                }
            }
        }
    }
}
