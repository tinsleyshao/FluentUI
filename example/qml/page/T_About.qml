import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import FluentUI 1.0
import "../component"

FluScrollablePage{

    title: qsTr("About")

    Image{
        id: image_logo
        Layout.topMargin: 20
        Layout.alignment: Qt.AlignHCenter
        width: 80
        height: 80
        source: "qrc:/example/res/image/favicon.ico"
    }

    Item{
        Layout.fillWidth: true
        Layout.preferredHeight: text_content.height + 40
        Layout.topMargin: 20
        
        FluText{
            id: text_content
            anchors{
                left: parent.left
                right: parent.right
                top: parent.top
                margins: 20
            }
            textFormat: Text.MarkdownText
            wrapMode: Text.WordWrap
            onLinkActivated:
                (link)=> {
                    Qt.openUrlExternally(link)
                }
            onLinkHovered:
                (link)=> {
                    if(link === ""){
                        FluTools.restoreOverrideCursor()
                    }else{
                        FluTools.setOverrideCursor(Qt.PointingHandCursor)
                    }
                }
            text:'
# 电力变压器抗短路能力预警平台

**版本：** v' + AppInfo.version + '

---

## 项目简介

电力变压器抗短路能力预警平台是一款基于 FluentUI 开发的专业软件，旨在为电力变压器提供全方位的健康监测与预警服务。

## 主要功能

- **概览 (Overview)**  
  提供设备运行状态的总览仪表板

- **资产管理 (Asset Management)**  
  对电力变压器资产进行集中管理

- **定期点检 (Regular Check)**  
  周期性设备巡检与数据记录

- **振动/噪声处理 (Vib/Noise Processing)**  
  实时监测并分析设备振动与噪声信号

- **多参融合 (Multi-parameter Fusion)**  
  融合多种参数进行智能分析与诊断

- **报告导出 (Report Export)**  
  一键生成专业分析报告

---

## 技术支持

如有技术问题或功能需求，请联系开发团队。
'
        }
    }
}
