#pragma once
#include<tool/tool.h>

typedef struct CardID {
	int index;//避免无意义的指针内存浪费/不得已向前声明改为直接定义
}cardID;//卡牌唯一ID标识

typedef enum CardType {
	BASE_CARD,//基础卡牌
	COUNTER_CARD,//反制卡牌
	EQUIP_CARD//装备卡牌
}cardtype;

typedef enum EffectType {//所有条件枚举全部置于效果枚举之前便于管理判断
	ID_TRUE,//存在枚举效果ID
	ID_FASLE,//无枚举效果ID
	TRIGGER_NOW,//立即触发
	TRIGGER_OFF,///延时触发
	//条件枚举
	//回合阶段枚举
	PHASE_SELF,//己方回合
	PHASE_ENEMY,//敌方回合
	PHASE_START,//回合开始
	PHASE_IN,//回合进行
	PHASE_END,//回合结束
	PHASE_CARD,//出牌时
	PHASE_DISCARD,//弃牌时
	//作用目标枚举
	TARGET_ALL,//所有角色
	TARGET_SELF,//自身
	TARGET_ENEMY,//敌人
	TARGET_CARD,//手牌
	TARGET_CARDS,//牌组
	TARGET_DISCARDS,//弃牌堆
	TARGET_HP,//血量
	TARGET_SHIELD,//护盾
	TARGET_POWER,//能量点
	//卡牌类型
	TARGET_BASE,//基础卡
	TARGET_COUNTER,//反制卡
	TARGET_EQUIP,//装备卡
	//特殊条件枚举
	CONDITION_ACTIVE,//主动触发
	CONDITION_PASSIVE,//被动触发
	CONDITION_NUMBER,//数量
	CONDITION_GREATER,//大于
	CONDITION_LESS,//小于
	CONDITION_SAME_ID,//卡牌类型相同
	CONDITION_UNSAME_ID,//卡牌类型不同
	CONDITION_TOP,//顶部
	CONDITION_BOTTOM,//底部
	CONDITION_FALSE_STATU,//负面状态
	CONDITION_TRUE_STATU,//正面状态
	CONDITION_CRUIAL_DAMAGE,//致命伤害
	//效果枚举
	//操作
	ACTION_SHOW_CARD,//展示卡牌
	ACTION_GET_CARD,//获得卡牌
	ACTION_DISCARD,//丢弃卡牌
	ACTION_MOVE_CARD,//移动卡牌
	ACTION_GET_HP,//获得血量
	ACTION_GET_SHIELD,//获得护盾
	ACTION_GET_POWER,//获得能量点
	ACTION_REDUCE_VALUE,//数值减少
	//状态/效果
	STATU_STUN,//晕眩
	STATU_WEAK,//虚弱
	EFFECT_MISS_DAMAGE,//无效伤害
	EFFECT_MISS_FALSE_EFFECT//无效负面效果
}effectID;

typedef struct {//延时触发字段
	effectID effect;//执行的效果
	effectID when;//执行时机
	int delay;//延迟回合
	int repeat;//重复次数
}Listener;//延时事件

typedef struct Effect{//效果字段
	effectID condition;//使用条件
	effectID target;//作用目标
	effectID action;//操作效果
	int val;//属性值
	int extra;//扩展属性值
	Listener* listener;//额外事件（后续最好分隔状态或者单独额外内存管理要不然职责轻微耦合）
}effect;

typedef struct Card {//卡牌//配置层规定具体卡牌效果组合
	//唯一标识
	cardID id;//卡牌唯一ID
	cardtype type;//基础/反制/装备
	//基础信息
	const char* name;//卡牌名
	int cost;//费用
	// 触发规则
	effectID trigger_mode;//立即 延时
	effectID trigger_phase;//触发阶段（回合开始/结束/出牌时...）
	// 核心：多效果组合
	int effect_count;//效果段数量
	effect effects[8];//最多8段效果/不足时可拓展
	//全局延时/监听（装备、被动、反制、延时）
	int listener_count;//全局监听数量
	Listener listeners[4];//全局延时/反制/被动
	////拓展字段/暂时可以不使用
	//unsigned is_consume : 1;//是否消耗
	//unsigned is_reusable : 1;//是否复用
	//unsigned back_to_hand : 1;//回手牌
	//unsigned back_to_deck : 1;//回卡组
	//unsigned on_discard_cast : 1;//弃牌时触发
	//unsigned is_permanent : 1;//永久装备
} Card;

typedef struct GameWorld {//全局游戏管理器（唯一）//配置层规定具体数值
	//玩家实体
	struct Player* player;
	struct Player* enemy;
	//卡牌池/所有卡牌实体
	struct Card* card_pool;
	int card_pool_count;
	//全局监听队列（延时/被动/反制，纯数据）
	Listener global_listeners[16];
	int listener_count;
	//全局回合计数
	int turn_count;
	int current_phase;// 当前回合阶段（仅存值）
}GameWorld;

typedef struct Player {//角色(具体数值信息由配置层规定)
	//基础数值
	int hp;//血量
	int max_hp;//最大血量
	int shield;//护盾
	int power;//能量点
	int power_max;//最大能量点
	//卡牌区域（仅存实体，不管理顺序逻辑）
	struct Card* hand[10];//手牌(上限由配置层规定/我这里后续需要修改)
	int hand_count;//手牌数量
	struct Card* deck[30]; //牌组
	int deck_count;
	struct Card* discard[30];//弃牌堆
	int discard_count;
	struct Card* equip[5];//装备区
	int equip_count;
	//状态标记（仅存状态，不处理逻辑）
	int statu_numbers;//状态数量
	effectID* effect_elem;//效果ID数组
} Player;




//———————————————————————以下所有标注由上层决定的，都只是表示可以根据上层需要和实现适当改变接口格式，但不可反向依赖！！！————————————————————————————
//以下所有函数后续全部挪动到配置层/由配置层实现/由配置层或者更上层负责管理



//注册回调函数接口（配置层注册函数具体实现/逻辑层提供回调函数接口/game层调度注册）
//效果回调函数类型
typedef void(*EffectFunc) (//回调函数标准接口（由逻辑层看情况修改格式/此处仅作参考）
	GameWorld* world,
	Player* caster,//释放者
	Player* target,//目标
	int val,//数值
	int extra//扩展值
);
//全局效果系统（注册+查找+调度）
//初始化效果系统
void EffectSystem_Init();
//注册效果：给effectID绑定一个函数
void Effect_Register(effectID id, EffectFunc func);
//查找效果：通过ID拿到函数
EffectFunc Effect_Get(effectID id);
// 执行单个效果（核心！状态机直接调用）
void Effect_Execute(//(也是由逻辑层决定格式)
	effectID id,
	GameWorld* world,
	Player* caster,
	Player* target,
	int val,
	int extra
);
// 执行一整张卡牌的所有效果（状态机调用//逻辑层决定）
void Card_ExecuteAllEffects(
	Card* card,
	GameWorld* world,
	Player* caster,
	Player* target
);
//监听/延时/反制/调度接口
//添加全局监听（延时、被动、反制、装备）
void Listener_Add(GameWorld* world, Listener lis);
//触发当前回合阶段的所有监听
void Listener_TriggerByPhase(GameWorld* world, effectID phase);