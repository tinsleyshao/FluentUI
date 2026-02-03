#pragma once

#include <QObject>
#include <QVariantMap>
#include <QProcess>
#include <QTemporaryDir>

/**
 * @brief Octave工作线程 - 在独立线程中执行Octave脚本
 */
class OctaveWorker : public QObject {
    Q_OBJECT

public:
    explicit OctaveWorker(QObject *parent = nullptr);
    ~OctaveWorker();

public slots:
    /**
     * @brief 设置Octave可执行文件路径
     */
    void setOctavePath(const QString &path);
    
    /**
     * @brief 设置脚本路径
     */
    void setScriptPath(const QString &path);
    
    /**
     * @brief 执行Octave脚本
     */
    void executeScript(const QString &scriptName, const QVariantMap &inputs);

signals:
    /**
     * @brief 执行完成，发出结果
     */
    void resultReady(const QVariantMap &result);
    
    /**
     * @brief 执行出错
     */
    void executionError(const QString &error);
    
    /**
     * @brief 进度更新
     */
    void executionProgress(int percentage);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();

private:
    // 保存输入数据为MAT文件
    bool saveInputData(const QVariantMap &inputs, const QString &matFilePath);
    
    // 从MAT文件加载结果
    QVariantMap loadResultData(const QString &matFilePath);
    
    // 生成执行脚本
    QString generateExecutionScript(const QString &scriptName, 
                                   const QString &inputFile,
                                   const QString &outputFile);
    
    // 解析过程输出
    void parseOctaveOutput(const QString &output);

    QProcess *m_process;
    QString m_octavePath;
    QString m_scriptPath;
    QString m_currentOutput;
    QString m_currentError;
    QTemporaryDir m_tempDir;
    int m_currentProgress;
};
