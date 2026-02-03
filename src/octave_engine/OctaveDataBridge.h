#pragma once

#include <QString>
#include <QVariantMap>
#include <QVariantList>
#include <QJsonObject>
#include <QJsonDocument>

/**
 * @brief JSON数据桥接 - 处理QVariant和JSON之间的转换
 */
class OctaveDataBridge {
public:
    /**
     * @brief 将QVariantMap转换为JSON字符串
     */
    static QString variantMapToJson(const QVariantMap &data);
    
    /**
     * @brief 将JSON字符串转换为QVariantMap
     */
    static QVariantMap jsonToVariantMap(const QString &jsonStr);
    
    /**
     * @brief 从JSON文件加载数据
     */
    static QVariantMap loadJsonFile(const QString &filePath);
    
    /**
     * @brief 保存数据到JSON文件
     */
    static bool saveJsonFile(const QString &filePath, const QVariantMap &data);
    
    /**
     * @brief 验证输入数据（根据schema）
     */
    static bool validateInputData(const QVariantMap &data, const QString &schemaJson);
    
    /**
     * @brief 将QVariantList转换为二维数组表示（用于表格）
     */
    static QStringList variantListToStringList(const QVariantList &list);
    
    /**
     * @brief 将二维QVariantList转换为JSON
     */
    static QString matrixToJson(const QVariantList &matrix);
    
    /**
     * @brief 从JSON解析矩阵数据
     */
    static QVariantList jsonToMatrix(const QString &jsonStr);

private:
    OctaveDataBridge() = delete;
    ~OctaveDataBridge() = delete;
    
    static QVariant convertToVariant(const QJsonValue &value);
    static QJsonValue convertToJson(const QVariant &value);
};
