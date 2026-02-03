#pragma once

#include <QString>
#include <QVariantMap>
#include <QObject>

/**
 * @brief 算法适配层 - 统一接口处理不同的算法
 */
class AlgorithmAdapter : public QObject {
    Q_OBJECT

public:
    explicit AlgorithmAdapter(QObject *parent = nullptr);
    
    /**
     * @brief 验证算法可用性
     */
    static bool validateAlgorithm(const QString &algorithmName);
    
    /**
     * @brief 获取算法输入参数规范
     */
    static QVariantMap getAlgorithmInputSpec(const QString &algorithmName);
    
    /**
     * @brief 获取算法输出参数规范
     */
    static QVariantMap getAlgorithmOutputSpec(const QString &algorithmName);
    
    /**
     * @brief 验证输入参数
     */
    static bool validateInputs(const QString &algorithmName, const QVariantMap &inputs);
    
    /**
     * @brief 验证输出结果
     */
    static bool validateOutputs(const QString &algorithmName, const QVariantMap &outputs);
    
    /**
     * @brief 获取所有可用算法列表
     */
    static QStringList getAvailableAlgorithms();

private:
    // 具体算法的规范定义
    static QVariantMap getPowerFlowSpec();
    static QVariantMap getFaultAnalysisSpec();
    static QVariantMap getStateEstimationSpec();
};
