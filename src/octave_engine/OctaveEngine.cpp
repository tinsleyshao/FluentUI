#include "OctaveEngine.h"
#include "OctaveWorker.h"
#include <QThread>
#include <QStandardPaths>
#include <QProcessEnvironment>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

OctaveEngine* OctaveEngine::s_instance = nullptr;

OctaveEngine::OctaveEngine(QObject *parent)
    : QObject(parent), m_isInitialized(false), m_workerThread(nullptr)
{
    m_octavePath = findOctaveExecutable();
}

OctaveEngine::~OctaveEngine()
{
    cleanup();
}

OctaveEngine* OctaveEngine::getInstance()
{
    if (!s_instance) {
        s_instance = new OctaveEngine();
    }
    return s_instance;
}

bool OctaveEngine::initialize(const QString &scriptPath)
{
    if (m_isInitialized) {
        return true;
    }
    
    if (m_octavePath.isEmpty()) {
        emit executionError(QStringLiteral("未找到Octave执行程序"));
        emit initialized(false);
        return false;
    }
    
    m_scriptPath = scriptPath;
    
    // 创建工作线程
    m_workerThread = new QThread();
    m_worker = std::make_unique<OctaveWorker>();
    m_worker->moveToThread(m_workerThread);
    
    // 连接信号槽
    connect(m_workerThread, &QThread::finished, m_worker.get(), &QObject::deleteLater);
    connect(this, &OctaveEngine::destroyed, m_workerThread, &QThread::quit);
    
    connect(m_worker.get(), &OctaveWorker::resultReady, 
            this, &OctaveEngine::resultReady);
    connect(m_worker.get(), &OctaveWorker::executionError, 
            this, &OctaveEngine::executionError);
    connect(m_worker.get(), &OctaveWorker::executionProgress, 
            this, &OctaveEngine::executionProgress);
    
    // 设置worker参数
    QMetaObject::invokeMethod(m_worker.get(), "setOctavePath", Qt::QueuedConnection,
                             Q_ARG(QString, m_octavePath));
    QMetaObject::invokeMethod(m_worker.get(), "setScriptPath", Qt::QueuedConnection,
                             Q_ARG(QString, scriptPath));
    
    m_workerThread->start();
    m_isInitialized = true;
    
    emit initialized(true);
    qDebug() << "Octave Engine initialized successfully";
    qDebug() << "Octave path:" << m_octavePath;
    qDebug() << "Script path:" << scriptPath;
    
    return true;
}

void OctaveEngine::cleanup()
{
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait();
        delete m_workerThread;
        m_workerThread = nullptr;
    }
    m_isInitialized = false;
}

QVariantMap OctaveEngine::executeSync(const QString &scriptName, 
                                       const QVariantMap &inputs)
{
    if (!m_isInitialized) {
        qWarning() << "OctaveEngine not initialized";
        return QVariantMap();
    }
    
    return executeInternal(scriptName, inputs);
}

void OctaveEngine::executeAsync(const QString &scriptName, 
                                 const QVariantMap &inputs)
{
    if (!m_isInitialized) {
        emit executionError(QStringLiteral("OctaveEngine未初始化"));
        return;
    }
    
    if (m_worker) {
        QMetaObject::invokeMethod(m_worker.get(), "executeScript", Qt::QueuedConnection,
                                 Q_ARG(QString, scriptName),
                                 Q_ARG(QVariantMap, inputs));
    }
}

void OctaveEngine::setAlgorithmPath(const QString &path)
{
    m_algorithmPath = path;
    if (m_worker) {
        QMetaObject::invokeMethod(m_worker.get(), "setScriptPath", Qt::QueuedConnection,
                                 Q_ARG(QString, path));
    }
}

QString OctaveEngine::getAlgorithmPath() const
{
    return m_algorithmPath;
}

bool OctaveEngine::isOctaveAvailable() const
{
    return !m_octavePath.isEmpty();
}

void OctaveEngine::setOctavePath(const QString &path)
{
    m_octavePath = path;
    if (m_worker) {
        QMetaObject::invokeMethod(m_worker.get(), "setOctavePath", Qt::QueuedConnection,
                                 Q_ARG(QString, path));
    }
}

QString OctaveEngine::findOctaveExecutable()
{
    // 常见的Octave路径
    QStringList possiblePaths;
    
#ifdef Q_OS_WINDOWS
    possiblePaths << "C:/Octave/Octave-9.1.0/mingw64/bin/octave.exe"
                  << "C:/Octave/bin/octave.exe"
                  << "octave"
                  << "octave-cli";
#elif defined(Q_OS_MACOS)
    possiblePaths << "/usr/local/bin/octave"
                  << "/opt/homebrew/bin/octave"
                  << "octave";
#else  // Linux
    possiblePaths << "/usr/bin/octave"
                  << "/usr/local/bin/octave"
                  << "octave";
#endif
    
    // 检查环境变量中的PATH
    QString pathEnv = QProcessEnvironment::systemEnvironment().value("PATH");
    if (!pathEnv.isEmpty()) {
#ifdef Q_OS_WINDOWS
        QStringList dirs = pathEnv.split(';');
#else
        QStringList dirs = pathEnv.split(':');
#endif
        for (const QString &dir : dirs) {
            QString candidate = dir + "/octave";
#ifdef Q_OS_WINDOWS
            if (QFile::exists(candidate + ".exe")) {
                return candidate + ".exe";
            }
#else
            if (QFile::exists(candidate)) {
                return candidate;
            }
#endif
        }
    }
    
    // 验证候选路径
    for (const QString &path : possiblePaths) {
        if (QFile::exists(path)) {
            return path;
        }
    }
    
    qWarning() << "Octave executable not found";
    return QString();
}

QVariantMap OctaveEngine::executeInternal(const QString &scriptName, 
                                          const QVariantMap &inputs)
{
    // 这个函数会被worker异步调用
    // 这里只是返回空结果，实际工作在worker中完成
    QVariantMap result;
    result["success"] = false;
    result["message"] = QStringLiteral("请使用异步执行方式");
    return result;
}
