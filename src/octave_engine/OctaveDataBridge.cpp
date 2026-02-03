#include "OctaveDataBridge.h"
#include <QJsonArray>
#include <QFile>
#include <QDebug>

QString OctaveDataBridge::variantMapToJson(const QVariantMap &data)
{
    QJsonObject jsonObj = QJsonObject::fromVariantMap(data);
    QJsonDocument doc(jsonObj);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}

QVariantMap OctaveDataBridge::jsonToVariantMap(const QString &jsonStr)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    if (!doc.isObject()) {
        qWarning() << "Invalid JSON: not an object";
        return QVariantMap();
    }
    return doc.object().toVariantMap();
}

QVariantMap OctaveDataBridge::loadJsonFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file:" << filePath;
        return QVariantMap();
    }
    
    QString jsonStr = QString::fromUtf8(file.readAll());
    file.close();
    
    return jsonToVariantMap(jsonStr);
}

bool OctaveDataBridge::saveJsonFile(const QString &filePath, const QVariantMap &data)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot write to file:" << filePath;
        return false;
    }
    
    QString jsonStr = variantMapToJson(data);
    file.write(jsonStr.toUtf8());
    file.close();
    
    return true;
}

bool OctaveDataBridge::validateInputData(const QVariantMap &data, const QString &schemaJson)
{
    // 简化的验证 - 实际应该使用JSON Schema库
    if (data.isEmpty()) {
        qWarning() << "Input data is empty";
        return false;
    }
    
    // TODO: 实现完整的JSON Schema验证
    return true;
}

QStringList OctaveDataBridge::variantListToStringList(const QVariantList &list)
{
    QStringList result;
    for (const QVariant &item : list) {
        result.append(item.toString());
    }
    return result;
}

QString OctaveDataBridge::matrixToJson(const QVariantList &matrix)
{
    QJsonArray jsonArray;
    
    for (const QVariant &item : matrix) {
        if (item.type() == QVariant::List) {
            QJsonArray row;
            for (const QVariant &cell : item.toList()) {
                if (cell.type() == QVariant::Double) {
                    row.append(cell.toDouble());
                } else if (cell.type() == QVariant::Int) {
                    row.append(cell.toInt());
                } else {
                    row.append(cell.toString());
                }
            }
            jsonArray.append(row);
        }
    }
    
    QJsonDocument doc(jsonArray);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}

QVariantList OctaveDataBridge::jsonToMatrix(const QString &jsonStr)
{
    QVariantList result;
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    if (!doc.isArray()) {
        qWarning() << "Invalid matrix JSON: not an array";
        return result;
    }
    
    QJsonArray jsonArray = doc.array();
    for (const QJsonValue &rowValue : jsonArray) {
        if (rowValue.isArray()) {
            QVariantList row;
            for (const QJsonValue &cell : rowValue.toArray()) {
                row.append(convertToVariant(cell));
            }
            result.append(row);
        }
    }
    
    return result;
}

QVariant OctaveDataBridge::convertToVariant(const QJsonValue &value)
{
    if (value.isDouble()) {
        return value.toDouble();
    } else if (value.isBool()) {
        return value.toBool();
    } else if (value.isString()) {
        return value.toString();
    } else if (value.isArray()) {
        QVariantList list;
        for (const QJsonValue &item : value.toArray()) {
            list.append(convertToVariant(item));
        }
        return list;
    } else if (value.isObject()) {
        return value.toObject().toVariantMap();
    }
    
    return QVariant();
}

QJsonValue OctaveDataBridge::convertToJson(const QVariant &value)
{
    if (value.type() == QVariant::Double) {
        return value.toDouble();
    } else if (value.type() == QVariant::Int) {
        return value.toInt();
    } else if (value.type() == QVariant::Bool) {
        return value.toBool();
    } else if (value.type() == QVariant::String) {
        return value.toString();
    } else if (value.type() == QVariant::List) {
        QJsonArray jsonArray;
        for (const QVariant &item : value.toList()) {
            jsonArray.append(convertToJson(item));
        }
        return jsonArray;
    } else if (value.type() == QVariant::Map) {
        return QJsonObject::fromVariantMap(value.toMap());
    }
    
    return QJsonValue();
}
