#pragma once

#include <QString>
#include <QVariantMap>
#include <QVariantList>
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <memory>

class OctaveWorker;
class QThread;

/**
 * @brief Octave集成引擎 - 负责调用Octave脚本执行算法
 * 提供同步和异步两种执行方式
 */
class OctaveEngine : public QObject {
    Q_OBJECT
    QML_NAMED_ELEMENT(OctaveEngine)
    QML_SINGLETON

public:
    // 获取单例实例
    static OctaveEngine* getInstance();
    
    /**
     * @brief 初始化Octave环境
     * @param scriptPath Octave脚本所在路径
     * @return 初始化成功返回true
     */
    Q_INVOKABLE bool initialize(const QString &scriptPath);
    
    /**
     * @brief 清理资源
     */
    Q_INVOKABLE void cleanup();
    
    /**
     * @brief 同步执行Octave脚本 (阻塞调用)
     * @param scriptName 脚本名称（无.m后缀）
     * @param inputs 输入数据映射
     * @return 执行结果映射
     */
    Q_INVOKABLE QVariantMap executeSync(const QString &scriptName, 
                                        const QVariantMap &inputs);
    
    /**
     * @brief 异步执行Octave脚本 (非阻塞调用)
     * @param scriptName 脚本名称
     * @param inputs 输入数据
     */
    Q_INVOKABLE void executeAsync(const QString &scriptName, 
                                  const QVariantMap &inputs);
    
    /**
     * @brief 设置算法包路径
     * @param path 完整路径
     */
    Q_INVOKABLE void setAlgorithmPath(const QString &path);
    
    /**
     * @brief 获取当前算法路径
     */
    Q_INVOKABLE QString getAlgorithmPath() const;
    
    /**
     * @brief 检查Octave是否可用
     */
    Q_INVOKABLE bool isOctaveAvailable() const;
    
    /**
     * @brief 设置Octave可执行文件路径
     */
    Q_INVOKABLE void setOctavePath(const QString &path);

signals:
    /**
     * @brief 异步执行完成，发出结果
     */
    void resultReady(const QVariantMap &result);
    
    /**
     * @brief 执行出错
     */
    void executionError(const QString &error);
    
    /**
     * @brief 执行进度 0-100
     */
    void executionProgress(int percentage);
    
    /**
     * @brief 初始化完成
     */
    void initialized(bool success);

private:
    explicit OctaveEngine(QObject *parent = nullptr);
    ~OctaveEngine();
    Q_DISABLE_COPY(OctaveEngine)

    static OctaveEngine* s_instance;
    
    QString m_octavePath;
    QString m_algorithmPath;
    QString m_scriptPath;
    bool m_isInitialized;
    std::unique_ptr<OctaveWorker> m_worker;
    QThread *m_workerThread;
    
    // 查找Octave可执行文件
    QString findOctaveExecutable();
    
    // 内部执行实现
    QVariantMap executeInternal(const QString &scriptName, const QVariantMap &inputs);
};
