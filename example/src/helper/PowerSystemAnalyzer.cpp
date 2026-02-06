#include "PowerSystemAnalyzer.h"
#include "octave_engine/OctaveEngine.h"
#include "pdf_export/FluReportBuilder.h"
#include <QDebug>
#include <QDateTime>

PowerSystemAnalyzer::PowerSystemAnalyzer(QObject *parent)
    : QObject(parent), m_octaveEngine(OctaveEngine::getInstance())
{
    m_reportBuilder = std::make_unique<FluReportBuilder>();
}

bool PowerSystemAnalyzer::initialize()
{
    if (!m_octaveEngine) {
        qWarning() << "OctaveEngine is not available";
        return false;
    }
    
    // 初始化Octave引擎
    bool result = m_octaveEngine->initialize("../src/algorithms/bridge_scripts");
    
    // 连接信号
    connect(m_octaveEngine, &OctaveEngine::resultReady,
            this, &PowerSystemAnalyzer::onOctaveResult);
    connect(m_octaveEngine, &OctaveEngine::executionError,
            this, &PowerSystemAnalyzer::onOctaveError);
    
    if (m_reportBuilder) {
        connect(m_reportBuilder.get(), &FluReportBuilder::generationProgress,
                this, &PowerSystemAnalyzer::onGenerationProgress);
    }
    
    qDebug() << "PowerSystemAnalyzer initialized:" << (result ? "success" : "failed");
    return result;
}

void PowerSystemAnalyzer::runPowerFlow(const QVariantMap &busData,
                                      const QVariantMap &lineData,
                                      const QVariantMap &options)
{
    if (!m_octaveEngine || !m_octaveEngine->isOctaveAvailable()) {
        emit analysisFailed(QStringLiteral("Octave引擎不可用"));
        return;
    }
    
    QVariantMap inputs;
    inputs["bus_data"] = busData;
    inputs["line_data"] = lineData;
    
    if (!options.isEmpty()) {
        inputs["options"] = options;
    } else {
        QVariantMap defaultOptions;
        defaultOptions["max_iterations"] = 100;
        defaultOptions["tolerance"] = 1e-6;
        inputs["options"] = defaultOptions;
    }
    
    emit progressUpdated(10);
    m_octaveEngine->executeAsync("run_power_flow", inputs);
}

void PowerSystemAnalyzer::runFaultAnalysis(const QVariantMap &busData,
                                          const QVariantMap &lineData,
                                          int faultBus)
{
    if (!m_octaveEngine || !m_octaveEngine->isOctaveAvailable()) {
        emit analysisFailed(QStringLiteral("Octave引擎不可用"));
        return;
    }
    
    QVariantMap inputs;
    inputs["bus_data"] = busData;
    inputs["line_data"] = lineData;
    inputs["fault_bus"] = faultBus;
    
    emit progressUpdated(10);
    m_octaveEngine->executeAsync("run_fault_analysis", inputs);
}

void PowerSystemAnalyzer::generateReport(const QString &title,
                                        const QString &outputPath,
                                        const QVariantMap &results)
{
    if (!m_reportBuilder) {
        emit analysisFailed(QStringLiteral("报告生成器未就绪"));
        return;
    }
    
    m_reportBuilder->title(title);
    m_reportBuilder->addCoverPage(
        title,
        "分析评估结果",
        QDateTime::currentDateTime().toString("yyyy-MM-dd"),
        "电力系统分析"
    );
    
    // 添加结果数据
    m_reportBuilder->addSection("分析结果");
    
    if (results.contains("voltage")) {
        m_reportBuilder->addSection("节点电压");
        
        QVariantList voltages = results["voltage"].toList();
        QVariantList headers;
        headers << "节点号" << "电压(pu)" << "状态";
        
        QVariantList rows;
        for (int i = 0; i < voltages.size() && i < 10; ++i) {
            rows.append(QVariantList{
                QString::number(i + 1),
                QString::number(voltages[i].toDouble(), 'f', 4),
                "正常"
            });
        }
        
        m_reportBuilder->addTable(headers, rows);
    }
    
    if (results.contains("power_flow")) {
        m_reportBuilder->addSection("线路功率");
        
        QVariantList powers = results["power_flow"].toList();
        QVariantList headers;
        headers << "线路号" << "功率(MW)" << "状态";
        
        QVariantList rows;
        for (int i = 0; i < powers.size() && i < 10; ++i) {
            rows.append(QVariantList{
                QString::number(i + 1),
                QString::number(powers[i].toDouble(), 'f', 2),
                "正常"
            });
        }
        
        m_reportBuilder->addTable(headers, rows);
    }
    
    // 生成PDF
    bool success = m_reportBuilder->generate(outputPath);
    
    if (success) {
        emit reportGenerated(outputPath);
        qDebug() << "Report generated:" << outputPath;
    } else {
        emit analysisFailed(QStringLiteral("报告生成失败"));
    }
}

QVariantMap PowerSystemAnalyzer::getLastResults() const
{
    return m_lastResults;
}

void PowerSystemAnalyzer::onOctaveResult(const QVariantMap &result)
{
    if (!result["success"].toBool()) {
        emit analysisFailed(result["error"].toString());
        return;
    }
    
    m_lastResults = result;
    formatResults(m_lastResults);
    
    emit progressUpdated(100);
    emit analysisComplete(m_lastResults);
}

void PowerSystemAnalyzer::onOctaveError(const QString &error)
{
    emit analysisFailed(error);
}

void PowerSystemAnalyzer::onGenerationProgress(int percentage)
{
    emit progressUpdated(90 + percentage / 10);  // 映射到90-100
}

void PowerSystemAnalyzer::formatResults(QVariantMap &results)
{
    // 格式化结果
    if (results.contains("iterations")) {
        qDebug() << "Iterations:" << results["iterations"].toInt();
    }
    
    if (results.contains("converged")) {
        qDebug() << "Converged:" << results["converged"].toBool();
    }
}
