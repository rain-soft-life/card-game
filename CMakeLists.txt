# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

#设置项目名称
project(mygame LANGUAGES C)
#设置支持语言版本
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
#创建静态库
add_library(base INTERFACE)#公共头文件
add_library(memory STATIC "memory/memory.c")#//内存管理器
add_library(tool STATIC "tool/tool.c")#通用工具
add_library(entity STATIC "entity/entity.c")#实体对象
add_library(config STATIC "config/config.c")#配置数据
add_library(game_logic STATIC "game_logic/game_logic.c")#逻辑
add_library(game_statu STATIC "game_statu/game_statu.c")#状态机
add_library(interaction STATIC "interaction/interaction.c")#交互
#创建主程序
add_executable(main "main.c")
#公示头文件路径
target_include_directories(base INTERFACE ${PROJECT_SOURCE_DIR})
target_include_directories(memory PUBLIC ${PROJECT_SOURCDE_DIR})
target_include_directories(tool PUBLIC ${PROJECT_SOURCDE_DIR})
target_include_directories(entity PUBLIC ${PROJECT_SOURCDE_DIR})
target_include_directories(config PUBLIC ${PROJECT_SOURCDE_DIR})
target_include_directories(game_logic PUBLIC ${PROJECT_SOURCDE_DIR})
target_include_directories(game_statu PUBLIC ${PROJECT_SOURCDE_DIR})
target_include_directories(interaction PUBLIC ${PROJECT_SOURCDE_DIR})
#创建依赖关系
target_link_libraries(memory PUBLIC base)
target_link_libraries(tool PUBLIC memory)
target_link_libraries(entity PUBLIC tool)
target_link_libraries(config PUBLIC entity)
target_link_libraries(game_logic PUBLIC entity)
target_link_libraries(game_statu PUBLIC config)
target_link_libraries(game_statu PUBLIC game_logic)
target_link_libraries(interaction PUBLIC game_statu)
target_link_libraries(main PUBLIC interaction)