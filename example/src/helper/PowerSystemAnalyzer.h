#pragma once

#include <QObject>
#include <QVariantMap>
#include <QVariantList>
#include <memory>

class OctaveEngine;
class FluReportBuilder;

/**
 * @brief 应用程序业务逻辑层
 * 演示如何使用OctaveEngine和FluReportBuilder
 */
class PowerSystemAnalyzer : public QObject {
    Q_OBJECT

public:
    explicit PowerSystemAnalyzer(QObject *parent = nullptr);
    
    /**
     * @brief 初始化分析器
     */
    bool initialize();
    
    /**
     * @brief 运行潮流计算
     */
    Q_INVOKABLE void runPowerFlow(const QVariantMap &busData, 
                                  const QVariantMap &lineData,
                                  const QVariantMap &options = QVariantMap());
    
    /**
     * @brief 运行故障分析
     */
    Q_INVOKABLE void runFaultAnalysis(const QVariantMap &busData,
                                      const QVariantMap &lineData,
                                      int faultBus);
    
    /**
     * @brief 生成分析报告
     */
    Q_INVOKABLE void generateReport(const QString &title,
                                   const QString &outputPath,
                                   const QVariantMap &results);
    
    /**
     * @brief 获取计算结果
     */
    QVariantMap getLastResults() const;

signals:
    /**
     * @brief 分析完成
     */
    void analysisComplete(const QVariantMap &results);
    
    /**
     * @brief 分析失败
     */
    void analysisFailed(const QString &error);
    
    /**
     * @brief 报告生成完成
     */
    void reportGenerated(const QString &path);
    
    /**
     * @brief 进度更新
     */
    void progressUpdated(int percentage);

private slots:
    void onOctaveResult(const QVariantMap &result);
    void onOctaveError(const QString &error);
    void onGenerationProgress(int percentage);

private:
    OctaveEngine *m_octaveEngine;
    std::unique_ptr<FluReportBuilder> m_reportBuilder;
    QVariantMap m_lastResults;
    
    // 辅助函数
    void formatResults(QVariantMap &results);
};
