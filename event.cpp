#include "event.h"



bool Window::is_init = false;
bool Window::font_init = false;
bool Event::running = true;


bool Window::windowInit() {
	if (!SDL_Init(SDL_INIT_VIDEO)) { is_init = false; return false; }
	else is_init = true;
	return true;
}
void Window::windowQuit() {
	if (!is_init)return;
	is_init = false;
	SDL_Quit();
}

bool Window::fontInit() {
	if (!TTF_Init()) { font_init = false; return false; }
	else font_init = true;
	return true;
}
void Window::fontQuit() {
	if (!font_init)return;
	font_init = false;
	TTF_Quit();
}

bool Window::isValid() const{
	return is_init && window_init;
}

void Window::hide() {
	if(this->window_init)
	SDL_HideWindow(this->window);
}

void Window::show() {
	if(this->window_init)
	SDL_ShowWindow(this->window);
}

SDL_Renderer* Window::GetRenderer() {
	return this->renderer;
}



Window::Window(int width,int height,const char* title) {
	this->height = height;
	this->width = width;
	size_t size = std::strlen(title);
	int lens = (size > 29) ? 29 : size;
	std::memcpy(this->title, title, lens);
	this->title[lens] = '\0';

	this->window = SDL_CreateWindow(title, width, height, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
	if (!this->window) { SDL_Log("%s", SDL_GetError()); return; }

	this->renderer = SDL_CreateRenderer(this->window, nullptr);
	if (!this->renderer) { SDL_DestroyWindow(this->window); this->window = nullptr;
		SDL_Log("%s", SDL_GetError()); return; }

	this->window_init = true;
}
Window::~Window() {
	this->window_init = false;


	if (this->renderer) {
		SDL_DestroyRenderer(this->renderer);
		this->renderer = nullptr;
	}
	if (this->window)
	{
		SDL_DestroyWindow(this->window);
		this->window = nullptr;
	}
	
}

SpriteAtlas::SpriteAtlas(Draw* pD) {
	pDraw = pD;
	t = nullptr;
}

SpriteAtlas::~SpriteAtlas() {
	destroy();
}

bool SpriteAtlas::loadAtlas(const char* imgpath, const SpriteFrame* framearr, int frameCount) {
	if (!this->pDraw || frameCount <= 0)return false;
	
	t = pDraw->loadTexture(imgpath);
	if (!t)return false;
	frames.clear();
	for (int i = 0; i < frameCount; i++) {
		frames.push_back(framearr[i]);
	}
	return true;
}

bool SpriteAtlas::loadAtlasOnlyTex(const char* imgpath)
{
	if (!pDraw) return false;
	t = pDraw->loadTexture(imgpath);
	if (!t) return false;
	SDL_GetTextureSize(t, &w, &h);
	frames.clear();
	animClips.clear();
	return true;
}

void SpriteAtlas::draw(int frameindex, float dstx, float dsty, float w, float h) {
	if (!t || frameindex < 0 || frameindex >= frames.size())return;

	const SpriteFrame& fra = frames[frameindex];
	this->pDraw->setRect(false, fra.srcx, fra.srcy, fra.w, fra.h);
	this->pDraw->setRect(true, dstx, dsty, w, h);
	this->pDraw->renderTexture(t,false, false);
}

SDL_Texture* SpriteAtlas::getTex() const{
	return t;
}

float SpriteAtlas::getTextureHeight() const{
	return h;
}

float SpriteAtlas::getTextureWidth() const{
	return w;
}

void SpriteAtlas::drawGrid(int col, int row, float dstx, float dsty, float w, float h) {
	const float tileSize = 206.f;
	const float pad = 2.f;
	const float gap = pad * 2;

	float srcx = pad + col * (tileSize + gap);
	float srcy = pad + row * (tileSize + gap);

	pDraw->setRect(false, srcx, srcy, tileSize, tileSize);
	pDraw->setRect(true, dstx, dsty, w, h);
	pDraw->renderTexture(t,false, false);
}

void SpriteAtlas::registerAnimation(int startIndex, int frameCount, Uint32 delay) {
	animClips.push_back({ startIndex,frameCount,delay });
}

void SpriteAtlas::playAnimation(int animIndex, Uint32 tick, float x, float y, float w, float h) {
	if (!t || animIndex >= animClips.size())return;

	AnimClip clip = animClips[animIndex];
	int cframe = (tick / clip.delay) % clip.frameCount;
	int targetFrame = clip.startIdx + cframe;
	draw(targetFrame, x, y, w, h);
}

void SpriteAtlas::playHorizontalUniformAnim(Uint32 tick, int startFrame, int frameCount, Uint32 delay,
	float tileW, float tileH, float dstX, float dstY, float dstW, float dstH) {
	if (!t) return;
	const float pad = 2.f;
	int curFrame = (tick / delay) % frameCount;
	int realFrame = startFrame + curFrame;
	//实时计算横向裁剪坐标
	float srcX = pad + realFrame * (tileW + pad * 2);
	float srcY = pad;

	pDraw->setRect(false, srcX, srcY, tileW, tileH);
	pDraw->setRect(true, dstX, dstY, dstW, dstH);
	pDraw->renderTexture(t, false, false);
}

void SpriteAtlas::destroy() {
	this->frames.clear();
	this->animClips.clear();
	if (t && pDraw) {
		pDraw->destroyTexture(t);
		t = nullptr;
	}
}


bool Draw::GetRendererPtr(SDL_Renderer* ptr) {
	this->ren = ptr;
	if (!this->ren)return false;
	return true;
}

SDL_Texture* Draw::loadTexture(const char* path) {//后续拓展支持渲染队列渲染
	if (!this->ren)return false;
	SDL_Texture* tex = IMG_LoadTexture(this->ren, path);
	if (!tex) { SDL_Log("%s", SDL_GetError()); return nullptr; }
	return tex;
}

bool Draw::loadFont(const char* path,int font_size) {
	this->font = TTF_OpenFont(path, font_size);
	if (!this->font)return false;
	return true;
}

SDL_Texture* Draw::textTexture(const char* text, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	if (!this->font || !this->ren)return nullptr;
	SDL_Color color = { r,g,b,a };
	SDL_Surface* surface = TTF_RenderText_Blended(this->font, text, 0, color);
	if (!surface)return nullptr;
	SDL_Texture* t = SDL_CreateTextureFromSurface(this->ren, surface);
	SDL_DestroySurface(surface);
	return t;
}



void Draw::destroyTexture(SDL_Texture* t) {
	if (t) { 
		SDL_DestroyTexture(t);
		t = nullptr;
	}
	
}

void Draw::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	if (this->ren)
		SDL_SetRenderDrawColor(this->ren, r, g, b, a);
}

void Draw::clear() {
	if (this->ren)
		SDL_RenderClear(this->ren);
}

void Draw::setRect(bool set_dst, float x, float y, float width, float height) {
	if (set_dst)this->dst = { x,y,width,height };
	else this->src = { x,y,width,height };
}

void Draw::renderTexture(SDL_Texture* targetTex,bool all_src,bool all_srceen) {
	if (!this->ren || !targetTex)return;
	const SDL_FRect* sr = static_cast<const SDL_FRect*>(&this->src);
	const SDL_FRect* ds = static_cast<const SDL_FRect*>(&this->dst);
	if (all_src) { sr = nullptr; }
	if (all_srceen) { ds = nullptr; }
	SDL_RenderTexture(this->ren, targetTex, sr, ds);
}

void Draw::present() {
	if (this->ren)
		SDL_RenderPresent(this->ren);
}



Draw::Draw() {
	
}

Draw::~Draw() {
	if (this->font) {
		TTF_CloseFont(this->font);
		this->font = nullptr;
	}

	this->ren = nullptr;
}


Event::Event() {
	SDL_zero(this->lastkeyState);
	SDL_zero(this->event);
}

void Event::poll() {//后续需要继续完善
	while (SDL_PollEvent(&(this->event))) {//轮询SDL3内部事件/同步更新按键当前状态
		//由于本系统采用的是SDL3另一套事件系统因此轮询不处理业务事件
		//只处理退出事件
		if (this->event.type == SDL_EVENT_QUIT) {
			running = false;
		}
	}

	//获取键盘当前状态
	this->pkeyStatu = SDL_GetKeyboardState(nullptr);//SDL3内部静态维护256个状态
	this->modState = SDL_GetModState();

	float mouseX, mouseY;
	Uint32 mouseMask = SDL_GetMouseState(&mouseX,&mouseY);//通过SDL3内部方法获取鼠标当前坐标
	this->mouse_x = mouseX;
	this->mouse_y = mouseY;

	//通过SDL3内部定义红宏对底层32位数字掩码做位运算，分别单独返回左键与右键状态
	this->pMouseLeft = mouseMask & SDL_BUTTON_MASK(SDL_BUTTON_LEFT);
	this->pMouseRight = mouseMask & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT);
}

void Event::saveLastFrameState() {
	//保存上一帧按键状态
	std::memcpy(this->lastkeyState, this->pkeyStatu, sizeof(this->lastkeyState));
	this->leftLast = this->pMouseLeft;;
	this->rightLast = this->pMouseRight;
}

bool Event::GameOverStatus() const{
	return running;
}

void Event::ExitGame() {
	running = false;
}


bool Event::keyHold(SDL_Keycode key)const {
	if (!this->pkeyStatu)return false;
	SDL_Scancode scan = SDL_GetScancodeFromKey(key, nullptr);
	return this->pkeyStatu[scan];
}

bool Event::keyDownOnce(SDL_Keycode key) const{
	if (!this->pkeyStatu)return false;
	SDL_Scancode scan = SDL_GetScancodeFromKey(key, nullptr);
	return this->pkeyStatu[scan] && !this->lastkeyState[scan];
}

SDL_Keymod Event::GetKeymod()const {
	return this->modState;
}

void Event::GetMousePos(float& x, float& y) const
{
	x = this->mouse_x;
	y = this->mouse_y;
}

bool Event::MouseLeftHold() const {
	return this->pMouseLeft;

}

bool Event::MouseRightHold() const {
	return this->pMouseRight;
}

bool Event::MouseLeftClick() const {
	return !this->leftLast && this->pMouseLeft;
}

bool Event::MouseLeftup() const {
	return !this->pMouseLeft && this->leftLast;
}

bool Event::MouseRightClick() const {
	return !this->rightLast && this->pMouseRight;
}

bool Event::MouseRightup() const {
	return !this->pMouseRight && this->rightLast;
}

bool Event::MouseInRect(float x, float y, float w, float h) const {
	float mx, my;
	GetMousePos(mx, my);
	return (mx >= x && mx <= x + w && my <= y + h && my >= y);
}