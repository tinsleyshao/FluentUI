#include "AlgorithmAdapter.h"
#include <QDebug>

AlgorithmAdapter::AlgorithmAdapter(QObject *parent)
    : QObject(parent)
{
}

bool AlgorithmAdapter::validateAlgorithm(const QString &algorithmName)
{
    QStringList valid = getAvailableAlgorithms();
    return valid.contains(algorithmName);
}

QVariantMap AlgorithmAdapter::getAlgorithmInputSpec(const QString &algorithmName)
{
    if (algorithmName == "power_flow") {
        return getPowerFlowSpec();
    } else if (algorithmName == "fault_analysis") {
        return getFaultAnalysisSpec();
    } else if (algorithmName == "state_estimation") {
        return getStateEstimationSpec();
    }
    
    return QVariantMap();
}

QVariantMap AlgorithmAdapter::getAlgorithmOutputSpec(const QString &algorithmName)
{
    // 输出规范与输入规范分开
    QVariantMap spec;
    
    if (algorithmName == "power_flow") {
        spec["voltage"] = QVariantMap{{"type", "array"}, {"description", "节点电压"}};
        spec["power_flow"] = QVariantMap{{"type", "array"}, {"description", "线路功率"}};
        spec["losses"] = QVariantMap{{"type", "double"}, {"description", "总网损"}};
        spec["converged"] = QVariantMap{{"type", "bool"}, {"description", "收敛标志"}};
        spec["iterations"] = QVariantMap{{"type", "int"}, {"description", "迭代次数"}};
    } else if (algorithmName == "fault_analysis") {
        spec["fault_voltage"] = QVariantMap{{"type", "array"}, {"description", "故障点电压"}};
        spec["fault_current"] = QVariantMap{{"type", "array"}, {"description", "故障电流"}};
        spec["fault_power"] = QVariantMap{{"type", "double"}, {"description", "故障功率"}};
    }
    
    return spec;
}

bool AlgorithmAdapter::validateInputs(const QString &algorithmName, const QVariantMap &inputs)
{
    QVariantMap spec = getAlgorithmInputSpec(algorithmName);
    
    // 简单验证：检查必需的键
    for (const QString &key : spec.keys()) {
        if (!inputs.contains(key)) {
            qWarning() << "Missing required input:" << key;
            return false;
        }
    }
    
    return true;
}

bool AlgorithmAdapter::validateOutputs(const QString &algorithmName, const QVariantMap &outputs)
{
    if (outputs.contains("success") && !outputs["success"].toBool()) {
        qWarning() << "Algorithm execution failed:" << outputs.value("error").toString();
        return false;
    }
    
    // 检查结果中的必需字段
    QVariantMap spec = getAlgorithmOutputSpec(algorithmName);
    // TODO: 验证输出格式
    
    return true;
}

QStringList AlgorithmAdapter::getAvailableAlgorithms()
{
    return {"power_flow", "fault_analysis", "state_estimation"};
}

QVariantMap AlgorithmAdapter::getPowerFlowSpec()
{
    QVariantMap spec;
    
    // 输入规范
    QVariantMap busDataSpec;
    busDataSpec["type"] = "matrix";
    busDataSpec["rows"] = -1;  // 动态行数
    busDataSpec["columns"] = 7;
    busDataSpec["description"] = "N×7母线数据矩阵 [ID, Type, Pd, Qd, Gs, Bs, V_nom]";
    
    QVariantMap lineDataSpec;
    lineDataSpec["type"] = "matrix";
    lineDataSpec["rows"] = -1;
    lineDataSpec["columns"] = 5;
    lineDataSpec["description"] = "M×5线路参数矩阵 [from, to, r, x, b]";
    
    QVariantMap optionsSpec;
    optionsSpec["max_iterations"] = QVariantMap{
        {"type", "int"},
        {"default", 100},
        {"min", 1},
        {"max", 10000}
    };
    optionsSpec["tolerance"] = QVariantMap{
        {"type", "double"},
        {"default", 1e-6},
        {"min", 1e-10},
        {"max", 1e-2}
    };
    
    spec["bus_data"] = busDataSpec;
    spec["line_data"] = lineDataSpec;
    spec["options"] = optionsSpec;
    
    return spec;
}

QVariantMap AlgorithmAdapter::getFaultAnalysisSpec()
{
    QVariantMap spec;
    
    spec["bus_data"] = QVariantMap{
        {"type", "matrix"},
        {"description", "母线数据"}
    };
    spec["line_data"] = QVariantMap{
        {"type", "matrix"},
        {"description", "线路数据"}
    };
    spec["fault_bus"] = QVariantMap{
        {"type", "int"},
        {"description", "故障母线号"}
    };
    
    return spec;
}

QVariantMap AlgorithmAdapter::getStateEstimationSpec()
{
    QVariantMap spec;
    
    spec["measurements"] = QVariantMap{
        {"type", "array"},
        {"description", "测量数据"}
    };
    
    return spec;
}
