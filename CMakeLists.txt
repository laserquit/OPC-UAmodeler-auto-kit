cmake_minimum_required(VERSION 3.10)

project(myNamespace)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_definitions(-std=c99)

# 查找 Python 解释器
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# 设置路径变量
set(NODESET_COMPILER ${PROJECT_SOURCE_DIR}/open62541/nodeset_compiler/nodeset_compiler.py)
set(BASE_NODESET ${PROJECT_SOURCE_DIR}/open62541/deps/ua-nodeset/Schema/Opc.Ua.NodeSet2.xml)
set(XML_FILE ${PROJECT_SOURCE_DIR}/src/example.xml)
set(OUTPUT_PREFIX ${PROJECT_SOURCE_DIR}/src/myNS)

# 添加自定义命令：从 XML 自动生成 C 代码
add_custom_command(
    OUTPUT ${OUTPUT_PREFIX}.c ${OUTPUT_PREFIX}.h
    COMMAND ${Python3_EXECUTABLE} ${NODESET_COMPILER}
            --types-array=UA_TYPES
            --existing ${BASE_NODESET}
            --xml ${XML_FILE}
            ${OUTPUT_PREFIX}
    COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/fix_header.py ${OUTPUT_PREFIX}.h
    DEPENDS ${XML_FILE}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/open62541/nodeset_compiler
    COMMENT "Generating myNS.c and myNS.h from example.xml using nodeset compiler"
)

include_directories(${PROJECT_SOURCE_DIR}/open62541)
include_directories(${PROJECT_SOURCE_DIR}/src)

# 添加可执行文件，包含生成的源文件
add_executable(server 
    ${PROJECT_SOURCE_DIR}/src/server.c 
    ${OUTPUT_PREFIX}.c
    ${PROJECT_SOURCE_DIR}/open62541/open62541.c
)

if(WIN32)
    target_link_libraries(server ws2_32 iphlpapi)
endif()

