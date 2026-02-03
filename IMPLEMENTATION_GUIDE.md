# FluentUI MATLAB/Octave 算法集成与PDF导出实现

## 📦 项目结构

本实现为FluentUI项目添加了以下功能模块：

```
src/
├── octave_engine/              # Octave集成层
│   ├── OctaveEngine.h/cpp      # 引擎主类 (单例)
│   ├── OctaveWorker.h/cpp      # 后台工作线程
│   └── OctaveDataBridge.h/cpp  # 数据格式转换
│
├── pdf_export/                 # PDF报告生成
│   └── FluReportBuilder.h/cpp  # 报告构建器
│
└── algorithms/                 # 算法适配层
    ├── AlgorithmAdapter.h/cpp  # 算法规范管理
    └── bridge_scripts/         # Octave脚本
        ├── run_power_flow.m    # 潮流计算
        └── run_fault_analysis.m # 故障分析

tests/
├── integration_tests/
│   └── test_octave_integration.cpp      # 集成测试
├── pdf_export_tests/
│   └── test_pdf_export.cpp              # PDF导出测试
└── CMakeLists.txt              # 测试配置
```

## 🚀 核心类说明

### 1. OctaveEngine (引擎管理)

单例模式，管理Octave解释器生命周期。

**关键方法：**
```cpp
bool initialize(const QString &scriptPath);
void executeAsync(const QString &scriptName, const QVariantMap &inputs);
QVariantMap executeSync(const QString &scriptName, const QVariantMap &inputs);
bool isOctaveAvailable() const;
```

**使用示例：**
```cpp
OctaveEngine *engine = OctaveEngine::getInstance();
engine->initialize("/path/to/scripts");

QVariantMap inputs;
inputs["bus_data"] = busDataMatrix;
input["line_data"] = lineDataMatrix;

engine->executeAsync("power_flow", inputs);
```

### 2. OctaveDataBridge (数据格式转换)

处理QVariant、JSON、MAT文件之间的转换。

**关键方法：**
```cpp
QString variantMapToJson(const QVariantMap &data);
QVariantMap jsonToVariantMap(const QString &jsonStr);
QString matrixToJson(const QVariantList &matrix);
QVariantList jsonToMatrix(const QString &jsonStr);
```

### 3. FluReportBuilder (PDF报告生成)

支持生成包含表格、图表、文本的多页中文PDF报告。

**关键方法：**
```cpp
void addCoverPage(const QString &title, ...);
void addSection(const QString &title);
void addTable(const QVariantList &headers, const QVariantList &rows);
void addText(const QString &htmlContent);
bool generate(const QString &filePath);
```

**使用示例：**
```cpp
FluReportBuilder builder;
builder.setTitle("电力系统报告");
builder.addCoverPage("潮流计算", "2026年分析", "2026-02-03", "分析员");
builder.addSection("结果数据");
builder.addTable(headers, rows);
builder.generate("/output/report.pdf");
```

### 4. AlgorithmAdapter (算法规范)

定义和验证算法的输入输出接口规范。

**关键方法：**
```cpp
bool validateAlgorithm(const QString &algorithmName);
QVariantMap getAlgorithmInputSpec(const QString &algorithmName);
QVariantMap getAlgorithmOutputSpec(const QString &algorithmName);
bool validateInputs(const QString &algorithmName, const QVariantMap &inputs);
```

## 🔧 集成步骤

### 步骤1：更新CMakeLists.txt

在 `src/CMakeLists.txt` 中添加：

```cmake
# 添加源文件
set(OCTAVE_ENGINE_SOURCES
    octave_engine/OctaveEngine.h
    octave_engine/OctaveEngine.cpp
    octave_engine/OctaveWorker.h
    octave_engine/OctaveWorker.cpp
    octave_engine/OctaveDataBridge.h
    octave_engine/OctaveDataBridge.cpp
)

set(PDF_EXPORT_SOURCES
    pdf_export/FluReportBuilder.h
    pdf_export/FluReportBuilder.cpp
)

set(ALGORITHM_SOURCES
    algorithms/AlgorithmAdapter.h
    algorithms/AlgorithmAdapter.cpp
)

list(APPEND CPP_FILES 
    ${OCTAVE_ENGINE_SOURCES}
    ${PDF_EXPORT_SOURCES}
    ${ALGORITHM_SOURCES}
)

# 确保Qt PrintSupport被链接
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS PrintSupport)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport)

# 使用Octave配置（可选）
option(USE_OCTAVE "Build with Octave support" ON)
```

### 步骤2：在FluentUI.cpp中注册组件

```cpp
#include "octave_engine/OctaveEngine.h"
#include "pdf_export/FluReportBuilder.h"

void FluentUI::registerTypes(const char *uri)
{
    // 注册Octave引擎（单例）
    qmlRegisterSingletonType<OctaveEngine>(
        uri, major, minor, "OctaveEngine",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            return OctaveEngine::getInstance();
        }
    );
    
    // 注册PDF报告生成器
    qmlRegisterType<FluReportBuilder>(
        uri, major, minor, "FluReportBuilder"
    );
    
    // ... 其他注册 ...
}
```

### 步骤3：测试编译

```bash
cd build
cmake -DUSE_OCTAVE=ON ..
cmake --build .
ctest  # 运行单元测试
```

## 📝 QML中的使用示例

### 执行MATLAB算法

```qml
import QtQuick
import FluentUI

Page {
    OctaveEngine {
        id: octaveEngine
        
        Component.onCompleted: {
            octaveEngine.initialize("./algorithms/bridge_scripts")
        }
        
        onResultReady: {
            console.log("计算完成:", result)
            resultText.text = JSON.stringify(result, null, 2)
        }
        
        onExecutionError: {
            resultText.text = "错误: " + error
        }
    }
    
    Button {
        text: "运行潮流计算"
        onClicked: {
            let inputs = {
                bus_data: [[1, 3, 0, 0, 0, 0, 1.0]],
                line_data: [[1, 2, 0.02, 0.06, 0.03]],
                options: {max_iterations: 100, tolerance: 1e-6}
            }
            octaveEngine.executeAsync("run_power_flow", inputs)
        }
    }
}
```

### 生成PDF报告

```qml
import QtQuick
import FluentUI

Page {
    FluReportBuilder {
        id: reportBuilder
        
        title: "电力系统分析报告"
        author: "分析部门"
        fontFamily: "SimSun"
    }
    
    Button {
        text: "生成报告"
        onClicked: {
            reportBuilder.addCoverPage(
                "电力系统分析",
                "潮流计算结果",
                "2026-02-03",
                "分析员"
            )
            
            reportBuilder.addSection("计算结果")
            reportBuilder.addText("潮流计算已完成")
            
            reportBuilder.addTable(
                ["节点", "电压", "功率"],
                [["1", "1.05", "100"], ["2", "1.02", "150"]]
            )
            
            reportBuilder.generate("/output/report.pdf")
        }
    }
}
```

## ✅ 单元测试

本实现包含完整的单元测试套件：

### 运行测试

```bash
cd build
ctest --verbose
```

### 测试覆盖范围

- ✅ OctaveEngine 初始化和执行
- ✅ OctaveWorker 后台处理
- ✅ OctaveDataBridge JSON/MAT转换
- ✅ FluReportBuilder PDF生成
- ✅ AlgorithmAdapter 规范验证

## 🔍 编码注意事项

### UTF-8 + BOM编码

所有文件都使用 **UTF-8 with BOM** 编码，确保中文显示正确：

```cpp
// 文件头自动包含 BOM 标记
#pragma once

#include <QString>
// ...
```

### 中文支持

PDF报告自动支持中文：

```cpp
builder.setFontFamily("SimSun");      // Windows中文字体
builder.addText("中文内容支持");
```

Octave脚本中的中文注释：

```octave
%% 潮流计算主函数
% 功能：计算电网潮流
% 输入：母线数据、线路数据
```

## 📋 数据格式规范

### 输入数据格式（JSON）

```json
{
  "bus_data": [
    [1, 3, 0.0, 0.0, 0.0, 0.0, 1.06],
    [2, 2, 0.217, 0.127, 0.0, 0.0, 1.045]
  ],
  "line_data": [
    [1, 2, 0.01938, 0.05917, 0.0528],
    [1, 5, 0.05403, 0.22304, 0.0492]
  ],
  "options": {
    "max_iterations": 100,
    "tolerance": 1e-6
  }
}
```

### 输出数据格式（JSON）

```json
{
  "success": true,
  "voltage": [1.06, 1.045, 1.038, ...],
  "power_flow": [50.2, 45.8, ...],
  "losses": 2.35,
  "converged": true,
  "iterations": 12,
  "message": "潮流计算完成，迭代次数: 12"
}
```

## 🐛 调试技巧

### 启用详细日志

```cpp
// 在OctaveEngine.cpp中
qDebug() << "Octave path:" << m_octavePath;
qDebug() << "Script path:" << scriptPath;

// 在OctaveWorker中
qWarning() << "Process output:" << m_currentOutput;
qWarning() << "Process error:" << m_currentError;
```

### 检查临时文件

Octave执行过程中生成的临时JSON文件位于：
- Windows: `%TEMP%/input_*.json`, `output_*.json`
- Linux/Mac: `/tmp/input_*.json`, `output_*.json`

### 验证Octave安装

```bash
# Windows
C:\Octave\Octave-9.1.0\mingw64\bin\octave --version

# Linux
octave --version

# macOS
/usr/local/bin/octave --version
```

## 📞 常见问题

### Q: Octave找不到？
A: 检查Windows上是否安装了Octave，或在PATH中添加Octave/bin路径

### Q: PDF中文显示乱码？
A: 确保设置 `SetFontFamily("SimSun")` 或其他支持中文的字体

### Q: 算法执行超时？
A: 检查算法输入数据，可能数据量过大或算法有死循环

## 📄 许可证

本实现遵循FluentUI项目的原始许可证。

## 👥 贡献指南

添加新算法时：
1. 在 `bridge_scripts/` 中创建新的.m文件
2. 在 `AlgorithmAdapter::getPowerFlowSpec()` 中定义规范
3. 添加单元测试到 `tests/integration_tests/`
4. 更新此README

---

**版本**: 1.0.0  
**创建日期**: 2026-02-03  
**编码**: UTF-8 + BOM
