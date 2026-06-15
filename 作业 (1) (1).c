


#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//=================================================================================  功  能  函  数  =====================================================================================================================


//=======================卡组=======================


//使用双向链表表示卡组


//单卡结构体
typedef struct Card
{
	int data;
	struct Card *last;
	struct Card *next;
}Card, *CardsHead;


//初始化链表，创建卡组
CardsHead CardsInit()
	{
		CardsHead L = (Card*)malloc(sizeof(Card));
		L->last = L;
		L->next = L;
		return L;
	}


//头插，从卡组顶加入单卡
void CHeadInsert(CardsHead L, int C)
{
	Card *NewCard = (Card*)malloc(sizeof(Card));
	NewCard->data = C;
	NewCard->next = L->next;
	NewCard->last = L;
	L->next->last = NewCard;
	L->next = NewCard; 
}


//头删，从卡组顶抽一张卡
int CHeadDelete(CardsHead L)
{
	//卡组已空返回0
	if(L->next == L)
	{
		return 0;
	}
	//否则返回对应卡号
	Card *del = L->next;
	int C = del->data;
	L->next = del->next;
	del->next->last = L;
	free(del);
	printf("抽卡成功");
	return C;
}


// 展示牌组顶部卡牌（不删除）
int ShowTopOfDrawPile(CardsHead Card) 
{
    if (Card->next == Card) return 0;   // 空牌组
    return Card->next->data;
}



//遍历，返回卡组中现有卡牌数量
int CCount(CardsHead L)
{
	int count = 0;
	Card *p = L;
	while(p->next != L)
	{
		count++;
		p = p->next;
	}
	return count;
}





//=======================弃牌堆=======================


//使用双向链表表示弃牌堆


//单卡结构体
typedef struct DCard
{
	int data;
	struct DCard *last;
	struct DCard *next;
}DCard, *DCardsHead;


//初始化链表，创建弃牌堆
DCardsHead DCardsInit()
	{
		DCardsHead L = (DCard*)malloc(sizeof(DCard));
		L->last = L;
		L->next = L;
		return L;
	}


//头插，将用过的卡放于弃牌堆顶
void DCHeadInsert(DCardsHead L, int C)
{
	DCard *NewCard = (DCard*)malloc(sizeof(DCard));
	NewCard->data = C;
	NewCard->next = L->next;
	NewCard->last = L;
	L->next->last = NewCard;
	L->next = NewCard; 
}




//========================手牌==========================


//使用顺序表表示手牌，最多8张
#define MAXSIZE 8


//定义顺序表结构，创建手牌
typedef struct
{
	int data[MAXSIZE];
	int HCnum;
} HCards;


//尾插，将卡加入手牌
void HCInsert(DCardsHead L, HCards *q, int C)
{
	if(q->HCnum >= MAXSIZE)//手牌已满时弃牌
	{
		DCHeadInsert(L, C);
	}
	else//不满则加入手牌
	{
		q->data[q->HCnum++] = C;
	}
}


//删除并输出指定位置元素，将卡打出并把打出的卡置入弃牌堆
int  UseCard(DCardsHead L, HCards *q, int U)
{
	if(U < 0 || U >= q->HCnum || q->HCnum == 0)//所选位置为空
	{
		return 0;
	}
	else//出牌，并将打出的牌置入弃牌堆
	{
		int C = q->data[U];
		for(int i = U; i < q->HCnum - 1; i++)
		{
			q->data[i] = q->data[i + 1];
		}
		q->HCnum--;
		DCHeadInsert(L, C);
		return C;
	}
}





//=======================回合内出卡记录表==========================


//用顺序表记录回合内出卡
#define P 8


//定义顺序表结构体，创建记录表
typedef struct 
{
	int data[P];
	int size;
}CRCards;


//头插，记录所出卡的卡号
void CRInsert(CRCards *L, int C)
{
	for(int i = L->size; i > 0; i--)
	{
		L->data[i] = L->data[i - 1];
	}
	L->data[0] = C;
	L->size++;
}


//清空记录表以记录下回合出牌
void CRClear(CRCards *L)
{
	L->size = 0;
}





//========================卡名检索========================


//用BF算法对卡表字符串进行检索并返还卡名前的卡号，找到玩家输入的卡名对应卡号

#include <string.h>

#define CARDNUM (sizeof(cardarr)/sizeof(CardInfo))

typedef struct
{
    char name[20];
    int id;
}
CardInfo;
CardInfo cardarr[]=
{
    {"attack",2},{"defence",9},{"again",17},
    {"charge",23},{"recycle",30},{"keycard",38},{"divination",46}
};
int BF(char *m, char *s)
{
    int n = strlen(m);
    int lenS = strlen(s);
    if(lenS == 0) return 0;
    int i; 
    for( i = 0; i <= n-lenS; i++)
    {
        int j = 0;
        while(j < lenS && m[i+j]==s[j])
            j++;
        if(j == lenS)
        //检验前后* 
        {
            char left  = (i==0) ? '*' : m[i-1];
            char right = m[i+lenS];
            if(left == '*' && right == '*')
            {
            	int k;
                for( k=0;k<CARDNUM;k++)
                    if(strcmp(cardarr[k].name,s)==0)
                        return cardarr[k].id;
            }
        }
    }
    return 0;
}



//指定元素返回并删除对应节点，卡组检索并置入手牌
int CSearch(DCardsHead I, CardsHead L, HCards *q, int C)
{
    Card *i = L->next;
    while(i != L)
	{
	    if(i->data == C)
		{
			HCInsert(I, q, C);
			i->next->last = i->last;
			i->last->next = i->next;
			i->last = i;
			i->next = i;
			free(i);
			break;
		}
		else
	    {
			i = i->next;
		}
	}	
	if(i != L)
	{
		return 1;//检索成功返回1
	}
	else
	{
		return 0;//检索失败返回0
	}



//指定元素返回并删除对应节点，弃牌堆检索置入卡组
int DCSearch(DCardsHead I, CardsHead L, int C)
{
    DCard *i = I->next;
    while(i != I)
	{
	    if(i->data == C)
		{
			CHeadInsert(L, C);
			i->next->last = i->last;
			i->last->next = i->next;
			i->last = i;
			i->next = i;
			free(i);
			break;
		}
		else
	    {
			i = i->next;
		}
	}
	if(i != I)
	{
		return 1;//检索成功返回1
	}
	else
	{
		return 0;//检索失败返回0
	}
}





//========================洗牌========================


//把卡组链表的元素转移至数组中打乱，之后返回卡组链表
void ShuffleCards(CardsHead L)
{
	//统计卡组中卡的个数
	int count = 0;
	Card *p = L->next;
	while(p != L)
	{
		count++;
		p = p->next;
	}
	if (count <= 1) return;
	//把链表内容转移至数组
	int *arr = (int*)malloc(sizeof(int));
	p = L->next;
	for(int i = 0; i < count; i++)
	{
		arr[i] = p->data;
		p = p->next;
	}
	//洗牌
	srand((unsigned int)time(NULL));
	for(int i = count - 1; i > 0; i--)
	{
		int j = rand() % (i + 1);
		int temp = arr[i];
		arr[i] = arr[j];
		arr[j] = temp;
	}
	//清空原卡组链表
	p = L->next;
	while(p != L)
	{
		Card *del = p;
		p = p->next;
		free(del);
	}
	L->next = L;
	L->last = L;
	//把洗过的牌回到卡组
	for(int i = 0; i < count; i++)
	{
		CHeadInsert(L, arr[i]);
	}
	free(arr);
}





//========================卡表========================


//1，2  攻击(attack)
//2，9  防御(defence)
//3，17 再来一次(again)
//4，23 充能(charge)
//5，30 回收利用(recycle)
//6，38 点睛(keycard)
//7，46 占卜(divination)





//==========================================================================================  卡  效  函  数  ===========================================================================================================


//设置全局变量

       int R = 0; //回合数
 int PBlood = 60; //玩家血量
int MBlood = 100; //怪物血量
      int AP = 0; //行动点
     int EAP = 0; //额外行动点
      int AR = 0; //护甲
	  int Mdun = 0;//怪物护盾值
	  int Msp = 3;//怪物的魔力值


//攻击 2
void attack(CRCards *L, int *q, CardsHead c, DCardsHead d, HCards *h)
{
	MBlood -= 5;
	AP--;
}


//防御 9
void defence(CRCards *L, int *q, CardsHead c, DCardsHead d, HCards *h)
{
	AR += 5;
	AP--; 
}


//再来一次 17
void again(CRCards *L, int *q, CardsHead c, DCardsHead d, HCards *h)
{
	for(int i = 0; i < L->size; i++)
	{
		if(q[i] == 17)
		{
			int C = CHeadDelete(c);
			HCInsert(d, h, C);
		}
	}
	int l = CHeadDelete(c);
	HCInsert(d, h, l);
	AP--;
}


//充能 23
void charge(CRCards *L, int *q, CardsHead c, DCardsHead d, HCards *h)
{
	EAP = 2;
	AP--;
}


//回收利用 30
void recycle(CRCards *L, int *q, CardsHead c, DCardsHead d, HCards *h)
{
    for(int i = 0; i < 3; i++)
	    {
		    int C;
		    scanf("%d\n", &C);
    	    DCSearch(d, c, C);
	    }
	int N = CCount(c);
	if(N <= 5)
	{
		for(int i = 0; i < 3; i++)
		{
			int C;
			scanf("%d\n", &C);
		    DCSearch(d, c, C);
		}
	}
	ShuffleCards(c);
	AP--;
}


//点睛 38
void keycard(CRCards *L, int *q, CardsHead c, DCardsHead d, HCards *h)
{
	int H = 0;
	for(int i = 0; i < L->size; i++)
		{
			if(q[i] == 38)
			{
				H++;
			}
		}
	if(H = 0)
	{
		int C1;
		scanf("%d\n", &C1);
		DCSearch(d, c, C1);
	}
	else
	{
		int C2;
		scanf("%d\n", &C2);
		CSearch(d, c, h, C2);
	}
	ShuffleCards(c);
	AP--;
}


//占卜 46
void divination(CRCards *L, int *q, CardsHead c, DCardsHead d, HCards *h)
{
	int C;
	C = ShowTopOfDrawPile(c);
	printf("卡组顶的卡牌是%d，若要将其继续置于卡组顶则输入1，否则输入0使其置入卡组底\n", C);
	int p;
	scanf("%d\n", &p);
	if(p = 1)
	{
		return;
	}
	else
	{
		Card* first = c->next;
		c->next = first->next;
		first->next->last = c;
		first->next = c;
		first->last = c->last;
		c->last->next = first;
		c->last = first;
	}
}


//普通攻击
void Matk() {
	int hurt = 6;
	PBlood -= hurt;
	printf("怪物发起了攻击，对你造成了6点伤害。\n");
}
//防御
void Mdef() {
	//获得六点护盾

	Msp -= 2;
	if (Msp >= 0)
	{
		Mdun += 19;
		printf("怪物获得了9层护盾！\n");
	}
}
//恢复魔力
void Mregain() {
	Msp += 3;
	printf("怪物使用了魔力再生，恢复了3点魔力\n");
}
//撕咬
void bite() {
	Msp -= 4;
	int hurt = 18;
	PBlood -= hurt;
	printf("怪物使用了撕咬，对你造成了18点伤害\n");
}


//决策树
void DecideAction()
{
	int needDEF = (MBlood + Mdun < 7) || (Mdun == 0 && MBlood <= 20);
	if (R == 1) { Matk(); return; }
	if (Msp <= 1) { Mregain(); return; }
	if (needDEF && Msp >= 2) { Mdef(); return; }
	if (Msp >= 4 && PBlood <= 18) { bite(); return; }
	if (PBlood > 18 && Msp < 4) { Mregain(); return; }
	if (Msp >= 0) { Matk(); return; }
}




//==========================================================================================  卡  效  仓  库  ============================================================================================================


//卡效函数的指针存放在链表中等待调用








//=============================================================================================  主  函  数  =============================================================================================================




//主函数
int main()
{
	//创建卡表
	char CardForm[] = "*attack*defence*again*charge*recycle*keycard*divination*";
	
	
	//创建卡组、弃牌堆、手牌、记录表
	CardsHead Card = CardsInit();
	DCardsHead DCard = DCardsInit();
	HCards HCard;
	HCard.HCnum = 0;
	CRCards CRCard;
	CRCard.size = 0;
	
	
	//玩家输入卡名编辑卡组，共携带18张卡
	printf("将你需要的卡写入\n");
	int i; 
	for( i = 0; i < 18; i++)
	{
		char CName[10];
		scanf("%s", CName);
		int C = BF(CardForm, CName);
		if(C > 0 )//检索成功，将该卡对应卡号置入卡组链表
		{
			CHeadInsert(Card, C);
		}
		else//检索失败则重置此次卡名输入
		{
			printf("所输入卡名对应的卡不存在，请重新选择\n");
			i--;
		}
	}
	
	
	//开局洗牌
	ShuffleCards(Card);
	
	
	//创建回合内出牌记录表
	#define RMAXCNUM 6
	
	typedef struct
	{
		int data[RMAXCNUM];
		int CN;
	} RC;
	
	
	
	
	//========================游戏主循环========================
	
	
	while(PBlood > 0 && MBlood > 0)
	{
		//回合开始获得四点行动点，并加上上回合中获得的额外行动点，清空额外行动点，清空护甲，清空记录表，回合数加一
		AP = 4 + EAP;
		EAP = 0;
		AR = 0;
		R++;
		CRClear(&CRCard);
		
		
		//回合开始时抽牌，第一回合抽5张。若抽牌时卡组为空，则结束游戏
		if(R == 1)
		{
			for(int n = 0; n < 5; n++)
			{
				int card = CHeadDelete(Card);
				if(card == 0)
				{
					PBlood = 0;
					break;
				}
				HCInsert(DCard, &HCard, card);
			}
			printf("第1回合\n玩家血量%d\n怪物血量%d\n", PBlood, MBlood);
		}
		else
		{
			int card = CHeadDelete(Card);
			if(card == 0)
			{
				PBlood = 0;
				break;
			}
			HCInsert(DCard, &HCard, card);
			printf("第%d回合\n玩家血量%d\n怪物血量%d\n", R, PBlood, MBlood);
		}
		
		
		//检测玩家出牌并将对应牌从手牌打出，实现卡牌效果
		int Over = 0;
		while(AP != 0 || Over == 0)
		{
			printf("现在，你的手牌有\n");
			for(int i = 0; i <= HCard.HCnum; i++)
			{
				int C = HCard.data[i];
				printf("%d\n", C);
			}
			printf("输入你所要出牌对应的卡号");
			int C;
			
			scanf("%d", &C);
		}
	}
}











