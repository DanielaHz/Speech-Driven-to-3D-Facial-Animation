add_library(retargeting_lib STATIC
    ${PROJECT_SOURCE_DIR}/pkg/retargeting/src/FacialMesh.cpp
    ${PROJECT_SOURCE_DIR}/pkg/retargeting/src/ActionUnit.cpp
    ${PROJECT_SOURCE_DIR}/pkg/retargeting/src/MathUtils.cpp
    ${PROJECT_SOURCE_DIR}/pkg/retargeting/src/FacialLandmark.cpp
    ${PROJECT_SOURCE_DIR}/pkg/retargeting/include/FacialMesh.h
    ${PROJECT_SOURCE_DIR}/pkg/retargeting/include/MathUtils.h
    ${PROJECT_SOURCE_DIR}/pkg/retargeting/include/ActionUnit.h
    ${PROJECT_SOURCE_DIR}/pkg/retargeting/include/FacialLandmark.h
    ${PROJECT_SOURCE_DIR}/pkg/retargeting/include/Side.h
)

set_target_properties(retargeting_lib PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(retargeting_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(retargeting_lib PRIVATE glm::glm tinyobjloader::tinyobjloader)

# GoogleTest Config
find_package(GTest CONFIG REQUIRED)
include(GoogleTest)
enable_testing()

# Glm Config
find_package(glm CONFIG REQUIRED)

# tinyobjloader Config
find_package(tinyobjloader CONFIG REQUIRED)

# nlohmann-json Config 
find_package(nlohmann_json CONFIG REQUIRED)

# Tests executables
add_executable(RetargetingTests)
target_sources(RetargetingTests PRIVATE 
    ${PROJECT_SOURCE_DIR}/pkg/retargeting/tests/unit-tests/FacialMeshTest.cpp
    ${PROJECT_SOURCE_DIR}/pkg/retargeting/tests/unit-tests/ActionUnitTest.cpp
    ${PROJECT_SOURCE_DIR}/pkg/retargeting/tests/unit-tests/MathUtilsTest.cpp    
)

target_link_libraries(RetargetingTests PRIVATE retargeting_lib tinyobjloader::tinyobjloader nlohmann_json::nlohmann_json glm::glm GTest::gtest GTest::gtest_main)
gtest_discover_tests(RetargetingTests)