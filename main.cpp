#define _CRT_SECURE_NO_WARNINGS

#include "publics.h"
#include "event/event.h"



//全局渲染对象
Window* win;
Draw draw;
Event event;
//四大图集
SpriteAtlas bgAtlas(&draw);
SpriteAtlas iconAtlas(&draw);
SpriteAtlas playerAtlas(&draw);
SpriteAtlas chickenAtlas(&draw);
// 动画状态标记
int g_playerAnim = 0; // 0待机 1出牌攻击
int g_chickenAnim = 0;// 0待机 1怪物攻击



// ===================== 全局常量、卡牌数据、结构体统一 =====================
#define MAXSIZE 8    //手牌最大数量
#define P 8          //回合出牌记录上限
#define RMAXCNUM 6
#define CARDNUM 7

// 卡牌信息表
typedef struct
{
	char name[20];
	int id;
} CardInfo;
CardInfo cardarr[] =
{
	{"attack",2},
	{"defence",9},
	{"again",17},
	{"charge",23},
	{"recycle",30},
	{"keycard",38},
	{"divination",46}
};

// 卡组双向链表
typedef struct Card
{
	int data;
	struct Card* last;
	struct Card* next;
} Card, * CardsHead;

// 弃牌堆双向链表
typedef struct DCard
{
	int data;
	struct DCard* last;
	struct DCard* next;
} DCard, * DCardsHead;

// 手牌顺序表
typedef struct
{
	int data[MAXSIZE];
	int HCnum;
} HCards;

// 回合出牌记录表
typedef struct
{
	int data[P];
	int size;
} CRCards;

// 全局游戏状态
int R = 0;        // 回合数
int PBlood = 60;  // 玩家血量
int MBlood = 100; // 怪物血量
int AP = 0;       // 行动点
int EAP = 0;      // 额外行动点
int AR = 0;       // 玩家护甲
int Mdun = 0;     // 怪物护盾
int Msp = 3;      // 怪物魔力

//=================================================================================  功  能  函  数  =====================================================================================================================
//=======================卡组=======================
CardsHead CardsInit()
{
	CardsHead L = (Card*)malloc(sizeof(Card));
	L->last = L;
	L->next = L;
	return L;
}

void CHeadInsert(CardsHead L, int C)
{
	Card* NewCard = (Card*)malloc(sizeof(Card));
	NewCard->data = C;
	NewCard->next = L->next;
	NewCard->last = L;
	L->next->last = NewCard;
	L->next = NewCard;
}

int CHeadDelete(CardsHead L)
{
	if (L->next == L)
	{
		return 0;
	}
	Card* del = L->next;
	int C = del->data;
	L->next = del->next;
	del->next->last = L;
	free(del);
	printf("抽卡成功\n");
	return C;
}

int ShowTopOfDrawPile(CardsHead Card)
{
	if (Card->next == Card) return 0;
	return Card->next->data;
}

int CCount(CardsHead L)
{
	int count = 0;
	Card* p = L;
	while (p->next != L)
	{
		count++;
		p = p->next;
	}
	return count;
}

// 释放卡组内存
void FreeCardList(CardsHead L)
{
	Card* p = L->next;
	while (p != L)
	{
		Card* del = p;
		p = p->next;
		free(del);
	}
	free(L);
}

//=======================弃牌堆=======================
DCardsHead DCardsInit()
{
	DCardsHead L = (DCard*)malloc(sizeof(DCard));
	L->last = L;
	L->next = L;
	return L;
}

void DCHeadInsert(DCardsHead L, int C)
{
	DCard* NewCard = (DCard*)malloc(sizeof(DCard));
	NewCard->data = C;
	NewCard->next = L->next;
	NewCard->last = L;
	L->next->last = NewCard;
	L->next = NewCard;
}

// 释放弃牌堆内存
void FreeDCardList(DCardsHead L)
{
	DCard* p = L->next;
	while (p != L)
	{
		DCard* del = p;
		p = p->next;
		free(del);
	}
	free(L);
}

//========================手牌==========================
void HCInsert(DCardsHead L, HCards* q, int C)
{
	if (q->HCnum >= MAXSIZE)
	{
		DCHeadInsert(L, C);
		printf("手牌已满，卡牌直接进入弃牌堆\n");
	}
	else
	{
		q->data[q->HCnum++] = C;
	}
}

int UseCard(DCardsHead L, HCards* q, int U)
{
	if (U < 0 || U >= q->HCnum || q->HCnum == 0)
	{
		return 0;
	}
	int C = q->data[U];
	for (int i = U; i < q->HCnum - 1; i++)
	{
		q->data[i] = q->data[i + 1];
	}
	q->HCnum--;
	DCHeadInsert(L, C);
	g_playerAnim = 1;//出牌切换攻击动画
	return C;
}

// 打印手牌
void PrintHand(HCards* h)
{
	printf("当前手牌：");
	for (int i = 0; i < h->HCnum; i++)
	{
		printf("%d ", h->data[i]);
	}
	printf("\n");
}

//=======================回合内出卡记录表==========================
void CRInsert(CRCards* L, int C)
{
	if (L->size >= P)
	{
		printf("本回合出牌已达上限！\n");
		return;
	}
	for (int i = L->size; i > 0; i--)
	{
		L->data[i] = L->data[i - 1];
	}
	L->data[0] = C;
	L->size++;
}

void CRClear(CRCards* L)
{
	L->size = 0;
}

//========================卡名检索 BF========================
int BF(char* m, char* s)
{
	int n = static_cast<int>(strlen(m));
	int lenS = static_cast<int>(strlen(s));
	if (lenS == 0) return 0;
	for (int i = 0; i <= n - lenS; i++)
	{
		int j = 0;
		while (j < lenS && m[i + j] == s[j])
			j++;
		if (j == lenS)
		{
			char left = (i == 0) ? '*' : m[i - 1];
			char right = m[i + lenS];
			if (left == '*' && right == '*')
			{
				for (int k = 0; k < CARDNUM; k++)
				{
					if (strcmp(cardarr[k].name, s) == 0)
						return cardarr[k].id;
				}
			}
		}
	}
	return 0;
}

// 卡组检索移除加入手牌
int CSearch(DCardsHead I, CardsHead L, HCards* q, int C)
{
	Card* i = L->next;
	while (i != L)
	{
		if (i->data == C)
		{
			HCInsert(I, q, C);
			i->next->last = i->last;
			i->last->next = i->next;
			free(i);
			return 1;
		}
		i = i->next;
	}
	return 0;
}

// 弃牌堆检索移回卡组
int DCSearch(DCardsHead I, CardsHead L, int C)
{
	DCard* i = I->next;
	while (i != I)
	{
		if (i->data == C)
		{
			CHeadInsert(L, C);
			i->next->last = i->last;
			i->last->next = i->next;
			free(i);
			return 1;
		}
		i = i->next;
	}
	return 0;
}

//========================洗牌（修复内存越界）========================
void ShuffleCards(CardsHead L)
{
	int count = 0;
	Card* p = L->next;
	while (p != L)
	{
		count++;
		p = p->next;
	}
	if (count <= 1) return;

	// 修复：分配对应数量int，不再只开1个
	int* arr = (int*)malloc(sizeof(int) * count);
	if (!arr) return;

	p = L->next;
	for (int i = 0; i < count; i++)
	{
		arr[i] = p->data;
		p = p->next;
	}

	srand((unsigned int)time(NULL));
	for (int i = count - 1; i > 0; i--)
	{
		int j = rand() % (i + 1);
		int temp = arr[i];
		arr[i] = arr[j];
		arr[j] = temp;
	}

	// 清空原链表
	p = L->next;
	while (p != L)
	{
		Card* del = p;
		p = p->next;
		free(del);
	}
	L->next = L;
	L->last = L;

	// 重新插入
	for (int i = 0; i < count; i++)
	{
		CHeadInsert(L, arr[i]);
	}
	free(arr);
	printf("洗牌完成\n");
}

//==========================================================================================  卡  效  函  数  ===========================================================================================================
// 怪物受伤统一结算护盾
void MonsterTakeDamage(int hurt)
{
	if (Mdun > 0)
	{
		if (Mdun >= hurt)
		{
			Mdun -= hurt;
			printf("护盾吸收%d伤害，剩余护盾：%d\n", hurt, Mdun);
		}
		else
		{
			hurt -= Mdun;
			printf("护盾全部破碎，额外承受%d伤害\n", hurt);
			Mdun = 0;
			MBlood -= hurt;
		}
	}
	else
	{
		MBlood -= hurt;
		printf("怪物受到%d点伤害，剩余血量：%d\n", hurt, MBlood);
	}
}

//攻击 2
void attack(CRCards* L, int* q, CardsHead c, DCardsHead d, HCards* h)
{
	MonsterTakeDamage(5);
	AP--;
	printf("打出攻击，造成5点伤害\n");
}

//防御 9
void defence(CRCards* L, int* q, CardsHead c, DCardsHead d, HCards* h)
{
	AR += 5;
	AP--;
	printf("获得5点护甲\n");
}

//再来一次 17
void again(CRCards* L, int* q, CardsHead c, DCardsHead d, HCards* h)
{
	for (int i = 0; i < L->size; i++)
	{
		if (q[i] == 17)
		{
			int C = CHeadDelete(c);
			HCInsert(d, h, C);
		}
	}
	int l = CHeadDelete(c);
	HCInsert(d, h, l);
	AP--;
	printf("再来一次效果生效，额外抽一张牌\n");
}

//充能 23
void charge(CRCards* L, int* q, CardsHead c, DCardsHead d, HCards* h)
{
	EAP = 2;
	AP--;
	printf("获得2点额外行动点\n");
}

//回收利用 30
void recycle(CRCards* L, int* q, CardsHead c, DCardsHead d, HCards* h)
{
	int inputC;
	for (int i = 0; i < 3; i++)
	{
		printf("输入要回收的卡牌id：");
		scanf("%d", &inputC);
		DCSearch(d, c, inputC);
	}
	int N = static_cast<int>(CCount(c));
	if (N <= 5)
	{
		for (int i = 0; i < 3; i++)
		{
			printf("输入要回收的卡牌id：");
			scanf("%d", &inputC);
			DCSearch(d, c, inputC);
		}
	}
	ShuffleCards(c);
	AP--;
	printf("回收弃牌堆卡牌并洗牌\n");
}

//点睛 38 修复if(H=0) → if(H==0)
void keycard(CRCards* L, int* q, CardsHead c, DCardsHead d, HCards* h)
{
	int H = 0;
	for (int i = 0; i < L->size; i++)
	{
		if (q[i] == 38)
		{
			H++;
		}
	}
	int inputC;
	if (H == 0)
	{
		printf("输入弃牌堆移回卡组的卡牌：");
		scanf("%d", &inputC);
		DCSearch(d, c, inputC);
	}
	else
	{
		printf("输入卡组移入手牌的卡牌：");
		scanf("%d", &inputC);
		CSearch(d, c, h, inputC);
	}
	ShuffleCards(c);
	AP--;
	printf("点睛效果完成\n");
}

//占卜 46 修复if(p=1) → if(p==1)
void divination(CRCards* L, int* q, CardsHead c, DCardsHead d, HCards* h)
{
	int C = ShowTopOfDrawPile(c);
	printf("卡组顶的卡牌是%d，保留输入1，移到底部输入0\n", C);
	int p;
	scanf("%d", &p);
	if (p == 1)
	{
		printf("卡牌保留在卡组顶部\n");
		return;
	}
	else
	{
		Card* first = c->next;
		// 摘下第一张
		c->next = first->next;
		first->next->last = c;
		// 链接到尾部
		first->next = c;
		first->last = c->last;
		c->last->next = first;
		c->last = first;
		printf("卡牌移至卡组底部\n");
	}
	AP--;
}

//怪物行为
void Matk()
{
	g_chickenAnim = 1;//怪物普通攻击
	int hurt = 6;
	if (AR > 0)
	{
		if (AR >= hurt)
		{
			AR -= hurt;
			printf("护甲抵挡6点伤害，剩余护甲：%d\n", AR);
		}
		else
		{
			hurt -= AR;
			AR = 0;
			PBlood -= hurt;
			printf("护甲破碎，受到%d伤害\n", hurt);
		}
	}
	else
	{
		PBlood -= hurt;
		printf("怪物普通攻击，造成6点伤害\n");
	}
}


void Mdef()
{
	Msp -= 2;
	if (Msp >= 0)
	{
		Mdun += 9;
		printf("怪物获得9层护盾！当前护盾：%d\n", Mdun);
	}
}

void Mregain()
{
	Msp += 3;
	printf("怪物魔力再生，魔力+3\n");
}

void bite()
{
	g_chickenAnim = 1;//怪物撕咬攻击
	Msp -= 4;
	int hurt = 18;
	if (AR > 0)
	{
		if (AR >= hurt)
		{
			AR -= hurt;
			printf("护甲抵挡18伤害，剩余护甲：%d\n", AR);
		}
		else
		{
			hurt -= AR;
			AR = 0;
			PBlood -= hurt;
			printf("护甲破碎，受到%d撕裂伤害\n", hurt);
		}
	}
	else
	{
		PBlood -= hurt;
		printf("怪物撕咬，造成18点伤害\n");
	}
}

//怪物AI决策
void DecideAction()
{+
	int needDEF = (MBlood + Mdun < 7) || (Mdun == 0 && MBlood <= 20);
	printf("-----怪物行动阶段-----\n");
	if (R == 1) { Matk(); return; }
	if (Msp <= 1) { Mregain(); return; }
	if (needDEF && Msp >= 2) { Mdef(); return; }
	if (Msp >= 4 && PBlood <= 18) { bite(); return; }
	if (PBlood > 18 && Msp < 4) { Mregain(); return; }
	if (Msp >= 0) { Matk(); return; }
}

// 根据卡牌ID执行对应卡效
void RunCardEffect(int cardId, CRCards* cr, CardsHead deck, DCardsHead discard, HCards* hand)
{
	switch (cardId)
	{
	case 2: attack(cr, cr->data, deck, discard, hand); break;
	case 9: defence(cr, cr->data, deck, discard, hand); break;
	case 17: again(cr, cr->data, deck, discard, hand); break;
	case 23: charge(cr, cr->data, deck, discard, hand); break;
	case 30: recycle(cr, cr->data, deck, discard, hand); break;
	case 38: keycard(cr, cr->data, deck, discard, hand); break;
	case 46: divination(cr, cr->data, deck, discard, hand); break;
	default: printf("无效卡牌ID\n"); break;
	}
}


#include<windows.h>


//渲染部分接口和资源初始化
//零工具模式：只加载纹理，不用帧数组
bool InitRender()
{

	// 打印程序当前工作目录
	char workDir[1024] = { 0 };
	GetCurrentDirectoryA(1024, workDir);
	printf("【程序运行根目录】%s\n", workDir);

	// 拼接完整图片路径并打印
	char fullPath[1024] = { 0 };
	sprintf_s(fullPath, "%s\\images\\bg.png", workDir);
	printf("【程序寻找图片完整地址】%s\n", fullPath);


	//SDL视频、字体初始化
	if (!Window::windowInit()) return false;
	if (!Window::fontInit()) return false;
	//创建窗口 1280*720
	win = new Window(1280, 720, "卡牌对战");
	win->show();
	//绑定渲染器到Draw
	draw.GetRendererPtr(win->GetRenderer());

	//背景 bg.png
	if (!bgAtlas.loadAtlasOnlyTex("images/bg.png"))
	{
		printf("背景图片加载失败\n");
		return false;
	}
	//卡牌图标 signals.png
	if (!iconAtlas.loadAtlasOnlyTex("images/signals.png"))
	{
		printf("图标图集加载失败\n");
		return false;
	}
	//玩家角色 cha.png 均匀长动画
	if (!playerAtlas.loadAtlasOnlyTex("images/cha.png"))
	{
		printf("角色动画加载失败\n");
		return false;
	}
	//小鸡怪物 monster.png
	if (!chickenAtlas.loadAtlasOnlyTex("images/monster.png"))
	{
		printf("怪物动画加载失败\n");
		return false;
	}
	return true;
}
//释放所有渲染资源
void FreeRender()
{
	bgAtlas.destroy();
	iconAtlas.destroy();
	playerAtlas.destroy();
	chickenAtlas.destroy();
	delete win;
	win = nullptr;
	Window::fontQuit();
	Window::windowQuit();
}
//渲染前置结构体、工具函数
struct GridPos
{
	int col;
	int row;
	GridPos(int c = 0, int r = 0) : col(c), row(r) {}
};
//卡牌ID映射网格坐标
GridPos CardIdToGrid(int cardId)
{
	switch (cardId)
	{
	case 2:  return GridPos(0, 0);
	case 9:  return GridPos(4, 2);
	case 17: return GridPos(2, 0);
	case 23: return GridPos(1, 0);
	case 30: return GridPos(1, 3);
	case 38: return GridPos(0, 3);
	case 46: return GridPos(3, 3);
	default: return GridPos(0, 0);
	}
}
//批量渲染手牌
void RenderHand(HCards* hand, SpriteAtlas& iconAtlas, float baseY)
{
	float startX = 30.f;
	float cardW = 120.f;
	float cardH = 180.f;
	float gap = 15.f;
	for (int i = 0; i < hand->HCnum; i++)
	{
		int cid = hand->data[i];
		GridPos pos = CardIdToGrid(cid);
		float x = startX + i * (cardW + gap);
		iconAtlas.drawGrid(pos.col, pos.row, x, baseY, cardW, cardH);
	}
}




//=============================================================================================  主  函  数  =============================================================================================================
int main()
{
	char CardForm[] = "*attack*defence*again*charge*recycle*keycard*divination*";

	//// ========== 渲染初始化 ==========
	//if (!InitRender())
	//{
	//	printf("渲染资源初始化失败！\n");
	//	return -1;
	//}

	// 初始化卡牌链表资源
	CardsHead CardDeck = CardsInit();
	DCardsHead Discard = DCardsInit();
	HCards Hand;
	Hand.HCnum = 0;
	CRCards CRRecord;
	CRRecord.size = 0;

	// 构建卡组18张
	printf("===== 构建卡组（输入18次卡牌英文名）=====\n");
	for (int i = 0; i < 18; )
	{
		char CName[32];
		scanf("%s", CName);
		int cid = BF(CardForm, CName);
		if (cid > 0)
		{
			CHeadInsert(CardDeck, cid);
			printf("成功加入卡牌id:%d\n", cid);
			i++;
		}
		else
		{
			printf("无此卡牌，重新输入\n");
		}
	}

	ShuffleCards(CardDeck);

	//游戏主循环
	while (PBlood > 0 && MBlood > 0 && event.GameOverStatus())
	{
		//回合初始化
		AP = 4 + EAP;
		EAP = 0;
		AR = 0;
		R++;
		CRClear(&CRRecord);

		printf("\n==================== 第%d回合 ====================\n", R);
		printf("玩家血量：%d | 怪物血量：%d | 怪物护盾：%d | 怪物魔力：%d\n", PBlood, MBlood, Mdun, Msp);
		printf("当前行动点：%d\n", AP);

		// 抽卡逻辑不变
		if (R == 1)
		{
			printf("首回合抽5张牌\n");
			for (int n = 0; n < 5; n++)
			{
				int c = CHeadDelete(CardDeck);
				if (c == 0)
				{
					printf("卡组抽空，游戏失败！\n");
					PBlood = 0;
					break;
				}
				HCInsert(Discard, &Hand, c);
			}
		}
		else
		{
			printf("回合抽1张牌\n");
			int c = CHeadDelete(CardDeck);
			if (c == 0)
			{
				printf("卡组抽空，游戏失败！\n");
				PBlood = 0;
				break;
			}
			HCInsert(Discard, &Hand, c);
		}

		if (PBlood <= 0) break;

		//玩家操作循环
		while (AP > 0)
		{
			printf("\n剩余行动点：%d\n", AP);
			PrintHand(&Hand);
			printf("输入手牌下标出牌（-1结束操作）：");
			int idx;
			scanf("%d", &idx);
			if (idx == -1) break;

			int cid = UseCard(Discard, &Hand, idx);
			if (cid == 0)
			{
				printf("出牌失败，下标错误\n");
				continue;
			}
			CRInsert(&CRRecord, cid);
			printf("打出卡牌id:%d\n", cid);
			RunCardEffect(cid, &CRRecord, CardDeck, Discard, &Hand);
		}

		//玩家操作结束，怪物行动
		DecideAction();


		//每帧结束重置动画为待机
		g_playerAnim = 0;
		g_chickenAnim = 0;

		//胜负判断
		if (MBlood <= 0)
		{
			printf("\n===== 胜利！怪物被击败 =====\n");
			break;
		}
		if (PBlood <= 0)
		{
			printf("\n===== 失败！你被击败 =====\n");
			break;
		}
	}

	//释放业务链表内存
	FreeCardList(CardDeck);
	FreeDCardList(Discard);
	//释放SDL渲染资源
	FreeRender();
	printf("游戏结束，资源释放完毕\n");
	return 0;
}