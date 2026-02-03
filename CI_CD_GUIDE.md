# GitHub Actions 完整集成指南

## 📌 概述

本项目包含完整的 GitHub Actions CI/CD 流水线，可自动执行以下任务:

- ✅ 跨平台编译 (Linux/Windows/macOS)
- ✅ 单元测试执行
- ✅ Octave 算法验证
- ✅ PDF 导出测试
- ✅ 代码质量分析
- ✅ 安全扫描
- ✅ 自动发布

## 🎯 发布新版本步骤

### 1. 准备发布

```bash
# 更新版本号
# 更新 CHANGELOG
# 提交更改

git add .
git commit -m "Prepare release v1.0.0"
git push origin main
```

### 2. 创建发布标签

```bash
# 创建带注释的标签
git tag -a v1.0.0 -m "Release version 1.0.0"

# 推送标签到 GitHub
git push origin v1.0.0
```

### 3. GitHub Actions 自动运行

当标签推送到 GitHub 时:

```
push tag v1.0.0
    ↓
release.yml 触发
    ├─→ 创建 GitHub Release
    ├─→ 为三个平台编译
    │   ├─→ Linux 编译
    │   ├─→ Windows 编译
    │   └─→ macOS 编译
    ├─→ 运行完整测试
    ├─→ 生成产物
    ├─→ 上传到 Release
    └─→ 发布文档
    ↓
Release 完成 ✅
```

### 4. 查看发布结果

1. 访问 GitHub 仓库 → **Releases**
2. 找到 v1.0.0 版本
3. 下载平台对应的产物

## 📊 工作流执行矩阵

### Build 工作流 (build.yml)

编译所有支持的平台和配置:

```
Qt 6.5.0
├── Windows MSVC + Octave 8.4.0
├── Linux GCC + Octave
└── macOS Clang + Octave

执行时间: ~30-40 分钟 (三平台并行)
```

### 集成测试 (integration-tests.yml)

测试所有功能模块:

```
Octave 脚本测试
├── 版本 8.4.0
├── 版本 9.1.0 (可选)
└── 运行时间: ~10 分钟

PDF 导出测试
├── 表格渲染
├── 中文字体
└── 运行时间: ~5 分钟

算法适配器测试
├── 规范验证
├── 数据转换
└── 运行时间: ~5 分钟

执行时间: ~15-20 分钟 (并行)
```

### 代码质量 (code-quality.yml)

静态分析和安全检查:

```
Cppcheck      (~2 分钟) - 代码检查
Clang-Tidy    (~3 分钟) - 代码分析
Format Check  (~1 分钟) - 格式检查
Security Scan (~5 分钟) - 安全扫描
Markdown Lint (~1 分钟) - 文档检查

执行时间: ~10 分钟
```

### 发布工作流 (release.yml)

打包和部署:

```
为三个平台编译
├── Windows: ~15 分钟
├── Linux: ~10 分钟
└── macOS: ~15 分钟

生成产物
├── .zip (Windows)
├── .tar.gz (Linux)
└── .tar.gz (macOS)

执行时间: ~25-30 分钟
```

## 📈 常见场景

### 场景 1: 修复 Bug 并发布

```bash
# 1. 创建修复分支
git checkout -b bugfix/issue-123

# 2. 修复代码
# ... 修改文件 ...

# 3. 提交并推送
git add .
git commit -m "Fix: Issue #123"
git push origin bugfix/issue-123

# GitHub Actions 自动:
# - 编译代码
# - 运行测试
# - 检查代码质量

# 4. 创建 PR 并合并
git checkout main
git pull
git merge bugfix/issue-123
git push

# 5. 创建发布标签
git tag -a v1.0.1 -m "Bugfix release"
git push origin v1.0.1

# GitHub Actions 自动:
# - 创建 Release
# - 生成产物
# - 发布到 GitHub Pages
```

### 场景 2: 发布新功能

```bash
# 1. 创建特性分支
git checkout -b feature/new-algorithm

# 2. 实现新功能
# - 在 src/algorithms/bridge_scripts/ 中添加 .m 文件
# - 在 AlgorithmAdapter 中注册
# - 添加单元测试
# ... 代码 ...

# 3. 测试本地构建
mkdir build && cd build
cmake -DUSE_OCTAVE=ON ..
cmake --build .
ctest

# 4. 推送到 GitHub
git add .
git commit -m "Feature: Add new algorithm"
git push origin feature/new-algorithm

# GitHub Actions:
# ✅ 编译成功
# ✅ 测试通过
# ✅ 代码质量检查
# ✅ 生成 PR 报告

# 5. 代码审查后合并
# 6. 创建发布标签
git tag -a v1.1.0 -m "New feature release"
git push origin v1.1.0
```

### 场景 3: 紧急修复

```bash
# 1. 从 main 创建热修复分支
git checkout -b hotfix/critical-bug main

# 2. 修复问题 (小范围)
# ... 修改文件 ...

# 3. 快速提交
git add .
git commit -m "Hotfix: Critical security issue"
git push origin hotfix/critical-bug

# 4. 快速合并
git checkout main
git merge hotfix/critical-bug
git push

# 5. 紧急发布
git tag -a v1.0.2 -m "Security hotfix"
git push origin v1.0.2

# GitHub Actions:
# - 立即开始编译和测试
# - 快速生成发布产物
# - 用户可立即下载新版本
```

## 🔥 实时监控

### 查看工作流状态

**方式 1: GitHub Web 界面**
```
GitHub 仓库 → Actions → 选择工作流 → 查看运行状态
```

**方式 2: GitHub CLI**
```bash
# 列出所有工作流运行
gh run list

# 查看特定运行的详细信息
gh run view <run-id>

# 查看实时日志
gh run view <run-id> --log
```

**方式 3: 在 PR 中查看**
```
PR 页面 → Checks 标签 → 查看各项检查状态
```

### 添加徽章到 README

在 README.md 中添加工作流状态徽章:

```markdown
## CI/CD Status

[![Build Status](https://github.com/your-org/FluentUI/workflows/Build%20with%20Octave%20and%20Qt/badge.svg?branch=main)](https://github.com/your-org/FluentUI/actions/workflows/build.yml)
[![Integration Tests](https://github.com/your-org/FluentUI/workflows/Integration%20Tests/badge.svg?branch=main)](https://github.com/your-org/FluentUI/actions/workflows/integration-tests.yml)
[![Code Quality](https://github.com/your-org/FluentUI/workflows/Code%20Quality%20&%20Security/badge.svg?branch=main)](https://github.com/your-org/FluentUI/actions/workflows/code-quality.yml)

[查看所有工作流](https://github.com/your-org/FluentUI/actions)
```

## 🐛 故障排除

### 工作流运行失败

**步骤 1: 查看错误日志**
```bash
# 在 Actions 页面中查看失败步骤的日志
# 或使用 CLI 查看
gh run view <run-id> --log | grep -A 20 "FAILED"
```

**常见错误和解决方案:**

| 错误信息 | 原因 | 解决方案 |
|---------|------|---------|
| `octave: command not found` | Octave 未安装 | 检查 action 配置 |
| `Qt: Command not found` | Qt 安装失败 | 检查 Qt 版本/架构 |
| `CMake configuration failed` | CMakeLists.txt 有错误 | 本地验证 CMake 配置 |
| `Test failed: timeout` | 测试耗时过长 | 增加 timeout-minutes |

### 如何本地重现问题

```bash
# 使用 act 工具在本地运行工作流
brew install act

# 运行特定工作流
act -j build

# 运行特定 OS
act -l  # 列出所有可用的 job
act --matrix os:ubuntu-latest

# 启用调试模式
act --debug
```

## 🔧 自定义工作流

### 添加新的检查步骤

编辑相应的 `.yml` 文件:

```yaml
- name: Custom Check
  run: |
    # 您的检查命令
    ./scripts/my-check.sh
```

### 修改编译参数

在 `build.yml` 中修改 CMake 配置:

```yaml
- name: Configure CMake
  run: |
    cmake -DUSE_OCTAVE=ON \
          -DCMAKE_BUILD_TYPE=Release \
          -DCUSTOM_FLAG=ON \  # 添加自定义标志
          ..
```

### 添加新的测试矩阵

```yaml
strategy:
  matrix:
    os: [ubuntu-latest, windows-latest, macos-latest]
    octave-version: ['8.4.0', '9.1.0']  # 添加新版本
    cmake-version: ['3.20', '3.25']     # 添加 CMake 版本
```

## 📊 性能优化

### 1. 启用构建缓存

```yaml
- name: Ccache for C++
  uses: hendrikmuhs/ccache-action@v1
  with:
    key: ${{ matrix.os }}-${{ matrix.compiler }}
```

### 2. 缓存依赖

```yaml
- name: Cache Qt
  uses: actions/cache@v3
  with:
    path: ~/Qt
    key: ${{ runner.os }}-qt-${{ matrix.qt-version }}
```

### 3. 并行执行

```yaml
jobs:
  build:
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
    # 三个平台并行编译
```

## 📚 相关文档

- [工作流详细文档](./README.md)
- [快速参考](./QUICK_REFERENCE.md)
- [项目实现指南](../IMPLEMENTATION_GUIDE.md)
- [交付清单](../DELIVERY_CHECKLIST.md)

## ✅ 检查清单

部署 GitHub Actions 时:

- ✅ 所有工作流文件在 `.github/workflows/`
- ✅ YAML 文件语法验证无错误
- ✅ CMakeLists.txt 已正确配置
- ✅ Secrets 已配置 (如适用)
- ✅ 分支保护规则已设置
- ✅ 构建和测试在本地通过
- ✅ 徽章已添加到 README
- ✅ 文档已更新

## 🎓 最佳实践

1. **定期运行**: 不只在发布时运行流水线，每次 PR 都要验证
2. **快速反馈**: 使流水线尽可能快地完成
3. **明确错误**: 提供清晰的错误消息便于调试
4. **版本控制**: 产物和报告应该版本化
5. **安全第一**: 定期扫描安全问题
6. **监视成本**: 注意计算时间和存储成本
7. **文档完善**: 保持工作流文档最新

---

**指南版本**: 1.0.0  
**最后更新**: 2026-02-03  
**编码**: UTF-8 + BOM
