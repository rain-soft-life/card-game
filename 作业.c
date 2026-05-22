#include <stdio.h>
#include <stdlib.h>



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
	//卡组已空
	if(L->next == L)
	{
		return 0;
	}
	Card *del = L->next;
	int C = del->data;
	L->next = del->next;
	free(del);
	return C;
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

//使用顺序表表示手牌
#define MAXSIZE 12

//定义顺序表结构
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




//========================卡名对应卡号========================






//========================卡名检索========================






//============================================================================================  主  函  数  =============================================================================================================


int R = 1; //回合数
int Blood = 60; //玩家血量


int main()
{
	//创建卡组、弃牌堆及手牌
	CardsHead Card = CardsInit();
	DCardsHead DCard = DCardsInit();
	HCards HCard;
	HCard.HCnum = 0;
	
	
	//主循环
	while(Blood != 0)
	{
		//回合开始时抽牌，第一回合抽5张。若抽牌时卡组为空，则结束游戏
		if(R == 1)
		{
			for(int n = 0; n < 5; n++)
			{
				int card = CHeadDelete(Card);
				if(card == 0)
				{
					Blood = 0;
					break;
				}
				HCInsert(DCard, &HCard, card);
			}
		}
		else
		{
			int card = CHeadDelete(Card);
			if(card == 0)
			{
				Blood = 0;
				break;
			}
			HCInsert(DCard, &HCard, card);
		}
		
	}
}



















