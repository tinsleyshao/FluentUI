#include "OctaveWorker.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include <QStandardPaths>
#include <QCoreApplication>

OctaveWorker::OctaveWorker(QObject *parent)
    : QObject(parent), m_process(nullptr), m_currentProgress(0)
{
    m_process = new QProcess(this);
    
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &OctaveWorker::onProcessFinished);
    connect(m_process, QOverload<QProcess::ProcessError>::of(&QProcess::error),
            this, &OctaveWorker::onProcessError);
    connect(m_process, &QProcess::readyReadStandardOutput,
            this, &OctaveWorker::onReadyReadStandardOutput);
    connect(m_process, &QProcess::readyReadStandardError,
            this, &OctaveWorker::onReadyReadStandardError);
}

OctaveWorker::~OctaveWorker()
{
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(3000);
    }
}

void OctaveWorker::setOctavePath(const QString &path)
{
    m_octavePath = path;
}

void OctaveWorker::setScriptPath(const QString &path)
{
    m_scriptPath = path;
}

void OctaveWorker::executeScript(const QString &scriptName, const QVariantMap &inputs)
{
    if (m_octavePath.isEmpty()) {
        emit executionError(QStringLiteral("未设置Octave路径"));
        return;
    }
    
    if (m_scriptPath.isEmpty()) {
        emit executionError(QStringLiteral("未设置脚本路径"));
        return;
    }
    
    // 创建临时文件
    QString tempDir = m_tempDir.path();
    QString inputFile = tempDir + "/input_" + QString::number(QCoreApplication::applicationPid()) + ".json";
    QString outputFile = tempDir + "/output_" + QString::number(QCoreApplication::applicationPid()) + ".json";
    
    // 保存输入数据
    if (!saveInputData(inputs, inputFile)) {
        emit executionError(QStringLiteral("保存输入数据失败"));
        return;
    }
    
    // 生成执行脚本
    QString execScript = generateExecutionScript(scriptName, inputFile, outputFile);
    QString scriptFile = tempDir + "/exec_" + QString::number(QCoreApplication::applicationPid()) + ".m";
    
    // 写入执行脚本
    QFile file(scriptFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit executionError(QStringLiteral("无法创建临时脚本文件"));
        return;
    }
    file.write(execScript.toUtf8());
    file.close();
    
    // 构建命令行参数
    QStringList arguments;
    arguments << "--quiet"
              << "--no-gui"
              << "-p" << m_scriptPath
              << "--eval" << QString("run('%1')").arg(scriptFile);
    
    m_currentProgress = 0;
    emit executionProgress(10);
    
    // 启动Octave进程
    m_currentOutput.clear();
    m_currentError.clear();
    m_process->start(m_octavePath, arguments);
    
    if (!m_process->waitForStarted()) {
        emit executionError(QStringLiteral("无法启动Octave进程: ") + m_octavePath);
        return;
    }
    
    // 等待完成 (最多30秒超时)
    if (!m_process->waitForFinished(30000)) {
        m_process->kill();
        emit executionError(QStringLiteral("Octave执行超时"));
        return;
    }
    
    emit executionProgress(90);
    
    // 读取输出结果
    QVariantMap result = loadResultData(outputFile);
    
    // 清理临时文件
    QFile::remove(inputFile);
    QFile::remove(outputFile);
    QFile::remove(scriptFile);
    
    emit executionProgress(100);
    emit resultReady(result);
}

bool OctaveWorker::saveInputData(const QVariantMap &inputs, const QString &matFilePath)
{
    // 为了简化，这里使用JSON格式保存
    // 实际应用中可以考虑使用MAT文件格式
    QJsonObject jsonObj = QJsonObject::fromVariantMap(inputs);
    QJsonDocument doc(jsonObj);
    
    QFile file(matFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    return true;
}

QVariantMap OctaveWorker::loadResultData(const QString &matFilePath)
{
    QVariantMap result;
    
    QFile file(matFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        result["success"] = false;
        result["message"] = QStringLiteral("无法读取结果文件");
        result["stderr"] = m_currentError;
        result["stdout"] = m_currentOutput;
        return result;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
        result = doc.object().toVariantMap();
    } else {
        result["success"] = false;
        result["message"] = QStringLiteral("无效的结果JSON");
        result["raw_output"] = QString::fromUtf8(data);
    }
    
    return result;
}

QString OctaveWorker::generateExecutionScript(const QString &scriptName,
                                             const QString &inputFile,
                                             const QString &outputFile)
{
    QString script;
    script += "try\n";
    script += "    % 加载输入数据\n";
    script += QString("    inputs = jsondecode(fileread('%1'));\n").arg(inputFile);
    script += "\n";
    
    // 调用原始函数
    script += QString("    % 执行算法: %1\n").arg(scriptName);
    script += QString("    [varargout{1:nargout}] = %1(inputs);\n").arg(scriptName);
    script += "\n";
    
    // 准备输出
    script += "    % 保存结果\n";
    script += "    output = struct();\n";
    script += "    output.success = true;\n";
    script += "    if nargout > 0\n";
    script += "        output.result = varargout{1};\n";
    script += "    end\n";
    script += "    if nargout > 1\n";
    script += "        output.result2 = varargout{2};\n";
    script += "    end\n";
    script += "\n";
    
    // 保存为JSON
    script += QString("    % 保存输出\n");
    script += QString("    output_json = jsonencode(output);\n");
    script += QString("    fid = fopen('%1', 'w');\n").arg(outputFile);
    script += QString("    fprintf(fid, '%s', output_json);\n");
    script += QString("    fclose(fid);\n");
    script += "\n";
    
    script += "catch err\n";
    script += QString("    % 错误处理\n");
    script += QString("    output = struct();\n");
    script += QString("    output.success = false;\n");
    script += QString("    output.error = err.message;\n");
    script += QString("    output.error_id = err.identifier;\n");
    script += QString("    output_json = jsonencode(output);\n");
    script += QString("    fid = fopen('%1', 'w');\n").arg(outputFile);
    script += QString("    fprintf(fid, '%s', output_json);\n");
    script += QString("    fclose(fid);\n");
    script += "end\n";
    script += "exit;\n";
    
    return script;
}

void OctaveWorker::parseOctaveOutput(const QString &output)
{
    // 解析Octave的进度相关输出
    if (output.contains("completed", Qt::CaseInsensitive)) {
        m_currentProgress = 80;
        emit executionProgress(m_currentProgress);
    }
}

void OctaveWorker::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        qWarning() << "Octave process exited with code:" << exitCode;
        qWarning() << "Error output:" << m_currentError;
    }
}

void OctaveWorker::onProcessError(QProcess::ProcessError error)
{
    QString errorMsg;
    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = QStringLiteral("无法启动Octave进程");
            break;
        case QProcess::Crashed:
            errorMsg = QStringLiteral("Octave进程崩溃");
            break;
        case QProcess::Timedout:
            errorMsg = QStringLiteral("Octave进程执行超时");
            break;
        case QProcess::WriteError:
            errorMsg = QStringLiteral("写入Octave进程失败");
            break;
        case QProcess::ReadError:
            errorMsg = QStringLiteral("读取Octave输出失败");
            break;
        default:
            errorMsg = QStringLiteral("未知错误");
            break;
    }
    
    emit executionError(errorMsg);
}

void OctaveWorker::onReadyReadStandardOutput()
{
    m_currentOutput += QString::fromUtf8(m_process->readAllStandardOutput());
    parseOctaveOutput(m_currentOutput);
}

void OctaveWorker::onReadyReadStandardError()
{
    m_currentError += QString::fromUtf8(m_process->readAllStandardError());
}
