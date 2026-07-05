#pragma once

#include <iostream>//C++头文件
//#include <string>//C++字符串头文件
#include<cstdio>//兼容C语言头文件
#include <cstring>//C++字符串或数组数据拷贝(避免指针赋值内存泄露)
#include<cstdlib>//兼容C语言内存函数
//#include <vector>//C++数组容器头文件
//#include<deque>//队列
#include<time.h>//时间种子
//#include <atomic>//原子化变量防止多线程竞态头文件//暂未用到
//#include <memory>//智能指针头文件
//#include <variant>//带标签联合体头文件（解决事件系统问题）
//#include <functional>//可调对象包装器头文件（std::function<void()>）
//#include <unordered_map>//C++哈希表容器头文件
#include <cstdint>//无符号整型多类型数据定义头文件（用于文本转像素数组使用的中间类型）
//#include <cstdlib>//C++中malloc与free头文件
//#include <fstream>//读取文件内容头文件
//#include <filesystem>//操作文件路径头文件
#include <SDL3/SDL.h>//SDL3头文件
#include <SDL3/SDL_surface.h>//文本转surface资源处理头文件
#include <SDL3_image/SDL_image.h>//SDL3_image库头文件
#include <SDL3_ttf/SDL_ttf.h>
//#include <glad/glad.h>//OpenGL函数加速库glad头文件
//#include <freetype2/ft2build.h>//文字库头文件
//#include FT_FREETYPE_H//由于保护机制需要添加宏
//#include <glm/glm.hpp>//C++数学函数库仅头文件//暂未用到