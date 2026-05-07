#include "game_logic.h" 
#include "tool/tool.h" 
#include "config/config.h" 
#include "game_statu/game_statu.h" 

// 全局游戏角色定义
character cha1, cha2; // cha1: 玩家, cha2: 敌人/怪物
basiccard card[3];   // 玩家手牌 (3张)
basiccard monster[5]; // 怪物卡池/手牌 (5张)
int lens = 0;        // 全局长度缓存 (用于绘制)

// --- 函数声明 ---
// renew_draw: 负责将单张卡牌的状态绘制到屏幕缓冲区
// renew_map: 负责刷新血量、护盾和状态文字
// renew_card: 负责卡牌的补给和位置重排
// compute: 核心战斗逻辑计算
// game_data_init: 游戏数据初始化
void renew_draw(int index, int monster_or_character, char* middle) {
    int len = 0;
    switch (monster_or_character) {
    case 0: // 绘制玩家卡牌
        if (card[index].statu == 0) return; // 卡牌消失则不绘制
        // 根据卡牌类型生成对应的字符串 [A:攻击, P:防御, E:处决, W:削弱, H:治疗]
        switch (card[index].type) {
        case attack:
            // 清除旧图像位置 (15 * BG_COLUMN + 3 + index * CARD_GAP)
            memcpy(background->maps + 15 * BG_COLUMN + 3 + index * CARD_GAP, nothing, CARD_GAP);
            len = snprintf(NULL, 0, "[A%d]", card[index].value);
            sprintf(middle, "[A%d]", card[index].value);
            // 绘制新图像
            memcpy(background->maps + 15 * BG_COLUMN + 3 + index * CARD_GAP, middle, len);
            break;
            // ... (protect, execute, weaken, heal 逻辑类似，仅标签不同)
        }
        break;
    case 1: // 绘制怪物卡牌 (位置在屏幕下方)
        if (monster[index].statu == 0) return;
        switch (monster[index].type) {
            // ... (逻辑同上，但坐标偏移不同：16 * BG_COLUMN - 9 - index * CARD_GAP)
        }
        break;
    }
}
void renew_map(void) {
    char middle[BLOOD_MAX + 3 + PROTECT_MAX];
    int len = 0;

    // 修正护盾值（防止负数）
    if (cha2.protection < 0) cha2.protection = 0;

    // 更新玩家血量/护盾 (绘制在 maps[109] 位置)
    len = snprintf(NULL, 0, "%d/%d", cha1.blood, cha1.protection);
    sprintf(middle, "%d/%d", cha1.blood, cha1.protection);
    memcpy(background->maps + 109, nothing, 8);
    memcpy(background->maps + 109, middle, len);

    // 更新怪物血量/护盾 (绘制在 maps[186] 位置)
    len = snprintf(NULL, 0, "%d/%d", cha2.blood, cha2.protection);
    sprintf(middle, "%d/%d", cha2.blood, cha2.protection);
    memcpy(background->maps + 186, nothing, 8);
    memcpy(background->maps + 193 - len, middle, len);
    lens = len;

    char ms[20];

    // 玩家状态显示 (如：Bleedlust, Berserk)
    switch (cha1.statu) {
    case c_default:
        memcpy(background->maps + 709, nothing, 20);
        sprintf(ms, "Default TURNS:~");
        memcpy(background->maps + 709, ms, 15);
        break;
    case c_bleedlust:
        memcpy(background->maps + 709, nothing, 20);
        sprintf(ms, "Bleedlust TURNS:%d", cha1.statu_lasting);
        memcpy(background->maps + 709, ms, 17);
        break;
        // ... (其他状态：berserk, weakened, holy)
    }

    // 怪物状态显示 (位置在 maps[771])
    switch (cha2.statu) {
    case c_default:
        memcpy(background->maps + 771, nothing, 20);
        sprintf(ms, "TURNS:~ Default");
        memcpy(background->maps + 771 + (20 - 15), ms, 15);
        break;
    case c_bleedlust:
        memcpy(background->maps + 771, nothing, 20);
        sprintf(ms, "TURNS:%d Bleedlust", cha2.statu_lasting);
        memcpy(background->maps + 771 + (20 - 17), ms, 17);
        break;
        // ... (其他状态)
    }

    // 循环刷新所有卡牌
    for (int t = 0; t < 2; t++) {
        int num = t ? 5 : 3; // 玩家3张，怪物5张
        for (int i = 0; i < num; i++)
            renew_draw(i, t, ms); // 调用绘制函数
    }
}
void compute(int orders, int max_blood, int pro_max) {
    if (orders < 1 || orders > 3) return; // 输入校验
    int o = orders - 1; // 数组下标修正
    int a, b, c; // 用于接收状态影响的临时变量

    statu_change(); // 全局状态变化钩子

    // --- 玩家出牌阶段 (card[o]) ---
    card[o].statu = 0; // 打出后卡牌消失
    compute_statu(0, &a, &b, &c, max_blood); // 计算玩家当前状态对伤害的修正

    switch (card[o].type) {
    case attack:
        // 状态修正：如果有增益(a/b>0)则伤害翻倍；如果有减益(c>0)则伤害减半
        if (a > 0 || b > 0) card[o].value *= 2;
        if (c > 0) card[o].value = (card[o].value / 2 < 1) ? 0 : card[o].value / 2;
        // 怪物处于狂暴状态则受到双倍伤害
        if (cha2.statu == c_berserk && cha2.statu_lasting > 0) card[o].value *= 2;

        // 攻击结算：先扣护盾，护盾不足扣血
        if (cha2.protection > 0) {
            cha2.protection -= card[o].value;
            if (cha2.protection < 0) {
                cha2.blood += cha2.protection; // 负数护盾转为扣血
                cha2.protection = 0;
            }
        }
        else {
            cha2.blood -= card[o].value;
        }
        break;

    case protect:
        // 增加护盾，不能超过上限
        cha1.protection = (cha1.protection + card[o].value > pro_max) ? pro_max : cha1.protection + card[o].value;
        break;

    case execute:
        // 处决效果：当怪物血量低于一定比例时，直接斩杀
        if ((card[o].value == 2 && cha2.blood <= max_blood * 0.2) ||
            (card[o].value == 1 && cha2.blood <= max_blood * 0.1))
            cha2.blood = 0;
        break;

    case heal:
        // 治疗：如果处于某种状态(a>0)则无法治疗
        if (a > 0) break;
        cha1.blood = (cha1.blood + card[o].value > max_blood) ? max_blood : cha1.blood + card[o].value;
        break;

    case weaken:
        // 削弱：直接削减怪物血量百分比
        cha2.blood /= card[o].value;
        if (cha2.blood < 1) cha2.blood = 1; // 保留至少1点血
        break;
    }

    // --- 怪物出牌阶段 (monster[0]) ---
    // (逻辑同上，角色互换)
    int p = (b > 0) ? 1 : 0; // 获取玩家的状态p值
    monster[0].statu = 0; // 怪物打出第一张牌
    compute_statu(1, &a, &b, &c, max_blood); // 计算怪物状态

    switch (monster[0].type) {
    case attack:
        // 怪物伤害修正：流血(a>0)伤害x3，狂暴(b>0)伤害x2，虚弱(c>0)伤害/2
        if (a > 0) monster[0].value *= 3;
        if (b > 0) monster[0].value *= 2;
        if (c > 0) monster[0].value = (monster[0].value / 2 < 1) ? 0 : monster[0].value / 2;
        if (p) monster[0].value *= 2; // 玩家有某种状态则怪物伤害翻倍

        // 玩家受到伤害结算
        if (cha1.protection > 0) {
            cha1.protection -= monster[0].value;
            if (cha1.protection < 0) {
                cha1.blood += cha1.protection;
                cha1.protection = 0;
            }
        }
        else {
            cha1.blood -= monster[0].value;
        }
        break;
        // ... (protect, execute, heal, weaken 逻辑类似)
    }

    // 状态回合数递减
    if (cha1.statu != c_default) cha1.statu_lasting--;
    if (cha2.statu != c_default) cha2.statu_lasting--;
}
// 补充手牌逻辑
void renew_card(void) {
    for (int t = 0; t < 2; t++) {
        int num = t ? 5 : 3; // 玩家补3张，怪物补5张
        for (int i = 0; i < num; i++) {
            if (t == 0 && card[i].statu == 0) { // 玩家卡牌空缺
                get_card(&card[i], 0); // 获取新卡
                card_order_change(0, 2 - i, i); // 调整位置
            }
            if (t == 1 && monster[i].statu == 0) { // 怪物卡牌空缺
                get_card(&monster[i], 1); // 获取新卡
                card_order_change(1, 4 - i, i); // 调整位置
            }
        }
    }
}

// 游戏数据初始化
void game_data_init(int blood, int protection) {
    if (background == NULL || background->maps == NULL) exit(0);
    if (blood < 0 || protection < 0) exit(0);

    // 初始化角色状态
    cha1.blood = blood; cha1.protection = 0; cha1.statu = c_default; cha1.statu_lasting = 0;
    cha2.blood = blood; cha2.protection = protection; cha2.statu = c_default; cha2.statu_lasting = 0;

    // 初始化玩家手牌
    card[0].type = attack; card[1].type = attack;
    card[0].value = random(VALUE_MAX); card[1].value = random(VALUE_MAX);
    card[2].type = (basiccardtype)random(2); // 随机类型
    // 随机数值逻辑
    if (card[2].type == heal) card[2].value = random(VALUE_MAX);
    else card[2].value = random(VALUE_MAX % 5 + 1);
    card[0].statu = card[1].statu = card[2].statu = 1; // 设置为可见

    // 初始化怪物卡牌
    for (int i = 0; i < 4; i++) {
        monster[i].type = (basiccardtype)(random(2) + 2); // 类型偏移
        monster[i].value = random(VALUE_MAX);
        monster[i].statu = 1;
    }
    // 第5张怪物卡牌逻辑
    monster[4].type = (basiccardtype)random(2);
    if (monster[4].type == heal) monster[4].value = random(VALUE_MAX);
    else monster[4].value = random(VALUE_MAX % 5 + 1);
    monster[4].statu = 1;

    renew_map(); // 刷新界面
}


