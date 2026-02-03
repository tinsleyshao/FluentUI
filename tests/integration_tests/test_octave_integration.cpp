#include <gtest/gtest.h>
#include "octave_engine/OctaveEngine.h"
#include "octave_engine/OctaveDataBridge.h"
#include "algorithms/AlgorithmAdapter.h"
#include <QCoreApplication>
#include <QVariantMap>
#include <QVariantList>
#include <QJsonDocument>
#include <QJsonObject>

/**
 * @brief 单元测试 - Octave引擎功能测试
 */

class OctaveEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = OctaveEngine::getInstance();
    }
    
    void TearDown() override {
        if (engine) {
            engine->cleanup();
        }
    }
    
    OctaveEngine *engine;
};

// 测试Octave可用性
TEST_F(OctaveEngineTest, OctaveAvailability) {
    EXPECT_TRUE(engine != nullptr);
    
    bool available = engine->isOctaveAvailable();
    if (!available) {
        GTEST_SKIP() << "Octave not installed on this system";
    }
}

// 测试初始化
TEST_F(OctaveEngineTest, InitializeEngine) {
    if (!engine->isOctaveAvailable()) {
        GTEST_SKIP() << "Octave not available";
    }
    
    // 假设脚本在src/algorithms/bridge_scripts路径
    bool result = engine->initialize("../src/algorithms/bridge_scripts");
    EXPECT_TRUE(result);
}

// 测试路径设置
TEST_F(OctaveEngineTest, SetAndGetAlgorithmPath) {
    QString testPath = "/path/to/algorithms";
    engine->setAlgorithmPath(testPath);
    EXPECT_EQ(engine->getAlgorithmPath(), testPath);
}

// 测试Octave路径设置
TEST_F(OctaveEngineTest, SetOctavePath) {
    // 测试可以设置路径
    engine->setOctavePath("octave");
    // 验证没有异常抛出
    EXPECT_TRUE(true);
}

/*
// 异步执行测试 (需要实际的Octave环境)
TEST_F(OctaveEngineTest, AsyncExecution) {
    if (!engine->isOctaveAvailable()) {
        GTEST_SKIP() << "Octave not available";
    }
    
    bool initialized = engine->initialize("../src/algorithms/bridge_scripts");
    if (!initialized) {
        GTEST_SKIP() << "Failed to initialize engine";
    }
    
    QVariantMap inputs;
    QVariantList busData;
    busData.append(QVariantList{1, 3, 0, 0, 0, 0, 1.0});
    busData.append(QVariantList{2, 1, 0.5, 0.2, 0, 0, 1.0});
    inputs["bus_data"] = busData;
    
    QVariantList lineData;
    lineData.append(QVariantList{1, 2, 0.02, 0.06, 0.03});
    inputs["line_data"] = lineData;
    
    engine->executeAsync("run_power_flow", inputs);
    
    // 使用事件循环等待结果
    QEventLoop loop;
    connect(engine, &OctaveEngine::resultReady, &loop, &QEventLoop::quit);
    connect(engine, &OctaveEngine::executionError, &loop, &QEventLoop::quit);
    
    QTimer::singleShot(10000, &loop, &QEventLoop::quit);  // 10秒超时
    loop.exec();
}
*/

/**
 * @brief 单元测试 - JSON数据桥接功能测试
 */

class OctaveDataBridgeTest : public ::testing::Test {
};

// 测试QVariantMap转JSON
TEST_F(OctaveDataBridgeTest, VariantMapToJson) {
    QVariantMap data;
    data["name"] = "test";
    data["value"] = 123;
    data["enabled"] = true;
    
    QString json = OctaveDataBridge::variantMapToJson(data);
    EXPECT_FALSE(json.isEmpty());
    EXPECT_TRUE(json.contains("test"));
}

// 测试JSON转QVariantMap
TEST_F(OctaveDataBridgeTest, JsonToVariantMap) {
    QString jsonStr = R"({"name": "test", "value": 123})";
    
    QVariantMap result = OctaveDataBridge::jsonToVariantMap(jsonStr);
    EXPECT_EQ(result["name"].toString(), "test");
    EXPECT_EQ(result["value"].toInt(), 123);
}

// 测试矩阵转JSON
TEST_F(OctaveDataBridgeTest, MatrixToJson) {
    QVariantList matrix;
    matrix.append(QVariantList{1.0, 2.0, 3.0});
    matrix.append(QVariantList{4.0, 5.0, 6.0});
    
    QString json = OctaveDataBridge::matrixToJson(matrix);
    EXPECT_FALSE(json.isEmpty());
    EXPECT_TRUE(json.contains("1"));
}

// 测试JSON转矩阵
TEST_F(OctaveDataBridgeTest, JsonToMatrix) {
    QString jsonStr = R"([[1.0, 2.0], [3.0, 4.0]])";
    
    QVariantList result = OctaveDataBridge::jsonToMatrix(jsonStr);
    EXPECT_EQ(result.size(), 2);
    
    if (result.size() >= 1) {
        QVariantList row1 = result[0].toList();
        EXPECT_EQ(row1.size(), 2);
    }
}

// 测试字符串列表转换
TEST_F(OctaveDataBridgeTest, VariantListToStringList) {
    QVariantList list;
    list.append("apple");
    list.append("banana");
    list.append("cherry");
    
    QStringList result = OctaveDataBridge::variantListToStringList(list);
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "apple");
}

/*
// 文件I/O测试 (需要临时目录)
TEST_F(OctaveDataBridgeTest, SaveAndLoadJsonFile) {
    QVariantMap data;
    data["test"] = "value";
    data["number"] = 42;
    
    QString testFile = "test_data.json";
    
    bool saved = OctaveDataBridge::saveJsonFile(testFile, data);
    EXPECT_TRUE(saved);
    
    QVariantMap loaded = OctaveDataBridge::loadJsonFile(testFile);
    EXPECT_EQ(loaded["test"].toString(), "value");
    EXPECT_EQ(loaded["number"].toInt(), 42);
    
    // 清理
    QFile::remove(testFile);
}
*/

/**
 * @brief 单元测试 - 算法适配器功能测试
 */

class AlgorithmAdapterTest : public ::testing::Test {
};

// 测试获取可用算法列表
TEST_F(AlgorithmAdapterTest, GetAvailableAlgorithms) {
    QStringList algorithms = AlgorithmAdapter::getAvailableAlgorithms();
    EXPECT_GT(algorithms.size(), 0);
    EXPECT_TRUE(algorithms.contains("power_flow"));
    EXPECT_TRUE(algorithms.contains("fault_analysis"));
}

// 测试算法验证
TEST_F(AlgorithmAdapterTest, ValidateAlgorithm) {
    EXPECT_TRUE(AlgorithmAdapter::validateAlgorithm("power_flow"));
    EXPECT_TRUE(AlgorithmAdapter::validateAlgorithm("fault_analysis"));
    EXPECT_FALSE(AlgorithmAdapter::validateAlgorithm("non_existent"));
}

// 测试获取输入规范
TEST_F(AlgorithmAdapterTest, GetInputSpec) {
    QVariantMap spec = AlgorithmAdapter::getAlgorithmInputSpec("power_flow");
    EXPECT_GT(spec.size(), 0);
    EXPECT_TRUE(spec.contains("bus_data"));
    EXPECT_TRUE(spec.contains("line_data"));
}

// 测试获取输出规范
TEST_F(AlgorithmAdapterTest, GetOutputSpec) {
    QVariantMap spec = AlgorithmAdapter::getAlgorithmOutputSpec("power_flow");
    EXPECT_GT(spec.size(), 0);
    EXPECT_TRUE(spec.contains("voltage"));
    EXPECT_TRUE(spec.contains("converged"));
}

// 测试输入验证
TEST_F(AlgorithmAdapterTest, ValidateInputs) {
    QVariantMap inputs;
    QVariantList busData;
    busData.append(QVariantList{1, 3, 0, 0, 0, 0, 1.0});
    inputs["bus_data"] = busData;
    
    QVariantList lineData;
    inputs["line_data"] = lineData;
    
    bool valid = AlgorithmAdapter::validateInputs("power_flow", inputs);
    EXPECT_TRUE(valid);
}

// 测试输出验证
TEST_F(AlgorithmAdapterTest, ValidateOutputs) {
    QVariantMap outputs;
    outputs["success"] = true;
    outputs["voltage"] = QVariantList{1.0, 1.05};
    outputs["converged"] = true;
    
    bool valid = AlgorithmAdapter::validateOutputs("power_flow", outputs);
    EXPECT_TRUE(valid);
}

// 测试失败的输出
TEST_F(AlgorithmAdapterTest, ValidateFailedOutputs) {
    QVariantMap outputs;
    outputs["success"] = false;
    outputs["error"] = "Test error";
    
    bool valid = AlgorithmAdapter::validateOutputs("power_flow", outputs);
    EXPECT_FALSE(valid);
}

// 测试全局函数
TEST_F(AlgorithmAdapterTest, GlobalFunctionality) {
    // 测试多个算法
    for (const QString &algo : AlgorithmAdapter::getAvailableAlgorithms()) {
        EXPECT_TRUE(AlgorithmAdapter::validateAlgorithm(algo));
        
        QVariantMap inputSpec = AlgorithmAdapter::getAlgorithmInputSpec(algo);
        EXPECT_GT(inputSpec.size(), 0) << "Algorithm: " << algo.toStdString();
        
        QVariantMap outputSpec = AlgorithmAdapter::getAlgorithmOutputSpec(algo);
        EXPECT_GT(outputSpec.size(), 0) << "Algorithm: " << algo.toStdString();
    }
}
