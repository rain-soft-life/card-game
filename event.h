#pragma once
#include "publics.h"


//tool/vector//自主实现vector类
#ifndef MYVECTOR_H
#define MYVECTOR_H

#include <cstddef>//无符号整型
#include <stdexcept>//提供runtime_error/越界空容器报错

template<typename T>//使用模板特性完成模板类封装
class MyVector
{
private:
    T* _start;//手动实现三个底层指针
    T* _finish;//有效元素个数=size()=_finish-_start
    T* _end_of_storage;//总内存容量=capacity()=_end_of_storage-_start/分配内存终点用于判断是否扩容
    //类内私有方法
    T* allocate(size_t n);//开辟指定大小内存
    void deallocate(T* ptr);//释放内存
    void copy_data(T* dest, const T* src, size_t n);//将src指定元素拷贝到新数组
    void destroy(T* first, T* last);//调用析构/销毁对象

public:
    typedef T* iterator;//普通迭代器
    typedef const T* const_iterator;//常量迭代器/不可修改/只读

    MyVector();
    explicit MyVector(size_t n);//带参构造/开辟指定空间/同时禁止隐式类型转换
    MyVector(const MyVector& other);//拷贝构造/两容器互不干扰
    MyVector& operator=(const MyVector& other);
    ~MyVector();

    size_t size() const;//有效元素数量
    size_t capacity() const;//总容量
    bool empty() const;//判空

    void reserve(size_t new_cap);//预分配容量
    void resize(size_t new_size);//修改有效元素数量
    void push_back(const T& val);//尾部元素
    void pop_back();//删除尾部元素
    void clear();//清理所有元素/内存不释放
    //下标访问重载
    T& operator[](size_t idx);
    const T& operator[](size_t idx) const;//常量容器
    //迭代器接口
    iterator begin();//返回首元素
    iterator end();//返回末尾下一个元素
    const_iterator begin() const;//常量容器的部分
    const_iterator end() const;
};

//模板类实现必须放在头文件否则链接报错
template<typename T>
T* MyVector<T>::allocate(size_t n)
{
    return (T*)malloc(n * sizeof(T));
}

template<typename T>
void MyVector<T>::deallocate(T* ptr)
{
    free(ptr);
}

template<typename T>
void MyVector<T>::copy_data(T* dest, const T* src, size_t n)
{
    for (size_t i = 0; i < n; ++i)
        dest[i] = src[i];
}

template<typename T>
void MyVector<T>::destroy(T* first, T* last)
{
    for (; first != last; ++first)
        first->~T();
}

template<typename T>
MyVector<T>::MyVector()
    : _start(nullptr), _finish(nullptr), _end_of_storage(nullptr)
{
}

template<typename T>
MyVector<T>::MyVector(size_t n)
{
    _start = allocate(n);
    _finish = _start;
    _end_of_storage = _start + n;
}

template<typename T>
MyVector<T>::MyVector(const MyVector& other)//深拷贝
{
    size_t cap = other.capacity();
    size_t sz = other.size();
    _start = allocate(cap);
    copy_data(_start, other._start, sz);
    _finish = _start + sz;
    _end_of_storage = _start + cap;
}

template<typename T>
MyVector<T>& MyVector<T>::operator=(const MyVector& other)//赋值运算符重载
{
    if (this == &other)
        return *this;

    destroy(_start, _finish);
    deallocate(_start);

    size_t cap = other.capacity();
    size_t sz = other.size();
    _start = allocate(cap);
    copy_data(_start, other._start, sz);
    _finish = _start + sz;
    _end_of_storage = _start + cap;
    return *this;
}

template<typename T>
MyVector<T>::~MyVector()
{
    destroy(_start, _finish);
    deallocate(_start);
    _start = _finish = _end_of_storage = nullptr;
}

template<typename T>
size_t MyVector<T>::size() const
{
    return _finish - _start;
}

template<typename T>
size_t MyVector<T>::capacity() const
{
    return _end_of_storage - _start;
}

template<typename T>
bool MyVector<T>::empty() const
{
    return _start == _finish;
}

template<typename T>
void MyVector<T>::reserve(size_t new_cap)
{
    if (new_cap <= capacity())
        return;
    T* new_start = allocate(new_cap);
    size_t sz = size();
    copy_data(new_start, _start, sz);

    destroy(_start, _finish);
    deallocate(_start);

    _start = new_start;
    _finish = _start + sz;
    _end_of_storage = _start + new_cap;
}

template<typename T>
void MyVector<T>::resize(size_t new_size)
{
    if (new_size < size())
    {
        destroy(_start + new_size, _finish);
        _finish = _start + new_size;
        return;
    }
    if (new_size > capacity())
        reserve(new_size);

    for (size_t i = size(); i < new_size; ++i)
        new (_start + i) T{};
    _finish = _start + new_size;
}

template<typename T>
void MyVector<T>::push_back(const T& val)
{
    if (_finish == _end_of_storage)
    {
        size_t new_cap = capacity() == 0 ? 1 : capacity() * 2;
        reserve(new_cap);
    }
    *_finish = val;
    ++_finish;
}

template<typename T>
void MyVector<T>::pop_back()
{
    if (empty())
        throw std::runtime_error("vector empty, cannot pop_back");
    --_finish;
    _finish->~T();
}

template<typename T>
void MyVector<T>::clear()
{
    destroy(_start, _finish);
    _finish = _start;
}

template<typename T>
T& MyVector<T>::operator[](size_t idx)
{
    if (idx >= size())
        throw std::runtime_error("index out of range");
    return _start[idx];
}

template<typename T>
const T& MyVector<T>::operator[](size_t idx) const
{
    if (idx >= size())
        throw std::runtime_error("index out of range");
    return _start[idx];
}

template<typename T>
typename MyVector<T>::iterator MyVector<T>::begin()
{
    return _start;
}

template<typename T>
typename MyVector<T>::iterator MyVector<T>::end()
{
    return _finish;
}

template<typename T>
typename MyVector<T>::const_iterator MyVector<T>::begin() const
{
    return _start;
}

template<typename T>
typename MyVector<T>::const_iterator MyVector<T>::end() const
{
    return _finish;
}

#endif
//————————————————————————————————————————————————————————————————


class Window {
private:
	SDL_Window* window=nullptr;
	SDL_Renderer* renderer=nullptr;
	int width, height;
	char title[30];

	static bool is_init;
	static bool font_init;

	bool window_init = false;
	bool isValid() const;

public:
	static bool windowInit();
	static void windowQuit();
	static bool fontInit();
	static void fontQuit();

    void setWindowSize(int w, int h) { width = w, height = h; }

    int GetWindowWidth() const { return width; }
    int GetWindowHeight() const { return height; }

	void hide();
	void show();
	SDL_Renderer* GetRenderer();//外部直接接收

	Window(int width, int height,const char* title);
	~Window();
};


class Draw {
private:
    TTF_Font* font = nullptr;//字体/后续可以脱离单独成类（图像也可以脱离/渲染为功能不再包揽一切）
    SDL_Renderer* ren = nullptr;//只有只用权限，不可销毁/确保使用周期在window类结束之前
    SDL_FRect src;//裁剪图
    SDL_FRect dst;//屏幕图

public:
    bool GetRendererPtr(SDL_Renderer* ptr);
    SDL_Texture* loadTexture(const char* path);
    bool loadFont(const char* path, int font_size);//目前仅支持单字体

    SDL_Texture* textTexture(const char* text, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

    void destroyTexture(SDL_Texture* t);
    void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void clear();
    void setRect(bool set_dst, float x, float y, float width, float height);//以图片左上角为准
    void renderTexture(SDL_Texture* targetTex, bool all_src, bool all_screen);//搭配渲染索引序列使用
    void present();

    Draw();
    ~Draw();
};


struct SpriteFrame {//精灵图集/只存储数据
public:
	float srcx;
	float srcy;
	float w;
	float h;
};

struct AnimClip//动画帧片段
{
public:
    int startIdx;
    int frameCount;
    Uint32 delay;
};


class SpriteAtlas {
private:
    Draw* pDraw;//绑定单独渲染工具类
    SDL_Texture* t;//大图
    float w = 0, h = 0;
    MyVector<SpriteFrame>frames;//存储精灵帧数据
    MyVector<AnimClip>animClips;//动画片段
public:
    explicit SpriteAtlas(Draw* pDraw);//唯一构造
    ~SpriteAtlas();
    
    bool loadAtlas(const char* imgpath, const SpriteFrame* framearr, int frameCount);
    bool loadAtlasOnlyTex(const char* path);
    void draw(int frameindex, float dstx, float dsty, float w, float h);

    SDL_Texture* getTex() const;
    float getTextureWidth()const;
    float getTextureHeight()const;

    void drawGrid(int col, int row, float dstx, float dsty, float w, float h);
    void registerAnimation(int startIndex, int frameCount, Uint32 delay);
    void playAnimation(int animId, Uint32 tick, float x, float y, float w, float h);
    void playHorizontalUniformAnim(Uint32 tick, int startFrame, int frameCount, Uint32 delay,
        float tileW, float tileH, float dstX, float dstY, float dstW, float dstH);

    void destroy();
};




class Event {
	//SDL_Keycode是逻辑按键码（字母等）/独立映射规则（底层数字）
	//SDL_Scancode是硬件扫描码（0~255）/可作为数组下标（直接一一对应键盘逻辑按键）
	//SDL3内部维护有bool按键状态数组/因此直接转Key_scancode下标访问即可
private:
	SDL_Event event;

	float mouse_x = 0, mouse_y = 0;
	bool pMouseLeft = false, leftLast = false;//pressMouseLeft/leftLast上一帧状态
	bool pMouseRight = false, rightLast = false;
	const bool* pkeyStatu = nullptr;//使用SDL3内置bool数组
	bool lastkeyState[SDL_SCANCODE_COUNT];//上一帧缓存按键状态
	SDL_Keymod modState = 0;//修饰键

	static bool running;

public:
	Event();

	void poll();
    SDL_Event GetLastEvent() const { return event; }
	void saveLastFrameState();//帧末尾调用存储状态
	bool GameOverStatus() const;
	void ExitGame();

	bool keyHold(SDL_Keycode key)const;//持续按住按键//如果SDL3初始化失败会越界访问
	bool keyDownOnce(SDL_Keycode key)const;//单次点击按键//同上
	SDL_Keymod GetKeymod()const;//获取修饰码

	void GetMousePos(float& x, float& y) const;//鼠标坐标
	bool MouseLeftHold() const;//左键按住
	bool MouseRightHold() const;//右键按住
	bool MouseLeftClick() const;//左键开始点击
	bool MouseLeftup() const;//左键松开
	bool MouseRightClick() const;//右键开始点击
	bool MouseRightup() const;//右键松开

    bool MouseInRect(float x, float y, float w, float h) const;//检测鼠标矩形碰撞
};

