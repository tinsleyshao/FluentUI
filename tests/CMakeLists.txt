# CMakeLists.txt for tests

cmake_minimum_required(VERSION 3.16)

# Include GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Find Qt
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS 
    Core Gui Pdf PrintSupport
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/octave_engine
    ${CMAKE_SOURCE_DIR}/src/pdf_export
    ${CMAKE_SOURCE_DIR}/src/algorithms
)

# 集成测试
add_executable(test_octave_integration 
    integration_tests/test_octave_integration.cpp
    ${CMAKE_SOURCE_DIR}/src/octave_engine/OctaveEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/octave_engine/OctaveWorker.cpp
    ${CMAKE_SOURCE_DIR}/src/octave_engine/OctaveDataBridge.cpp
    ${CMAKE_SOURCE_DIR}/src/algorithms/AlgorithmAdapter.cpp
)

target_link_libraries(test_octave_integration
    Qt${QT_VERSION_MAJOR}::Core
    GTest::gtest_main
)

# PDF导出测试
add_executable(test_pdf_export 
    pdf_export_tests/test_pdf_export.cpp
    ${CMAKE_SOURCE_DIR}/src/pdf_export/FluReportBuilder.cpp
)

target_link_libraries(test_pdf_export
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Pdf
    Qt${QT_VERSION_MAJOR}::PrintSupport
    GTest::gtest_main
)

# 注册测试
enable_testing()
add_test(NAME OctaveIntegration COMMAND test_octave_integration)
add_test(NAME PdfExport COMMAND test_pdf_export)
