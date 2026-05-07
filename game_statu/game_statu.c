#include "game_statu/game_statu.h"
#include "../game_logic/game_logic.h"
#include "../config/config.h"
#include "../tool/tool.h"

// 声明外部全局变量，以便在此文件中访问
extern character cha1, cha2;    // cha1: 玩家, cha2: 怪物
extern basiccard card[3];       // 玩家手牌
extern basiccard monster[5];    // 怪物手牌

/**
 * 状态持续时间检查与重置
 * 功能：每回合检查角色状态是否过期，如果回合数归零，则重置为默认状态
 */
void statu_change(void) {
    // 检查玩家状态
    // 如果当前不是默认状态 且 持续时间小于1（回合结束）
    if (cha1.statu != c_default && cha1.statu_lasting < 1) {
        cha1.statu = c_default;         // 重置为默认状态
        cha1.statu_lasting = 0;         // 重置持续时间
    }
    // 检查怪物状态
    if (cha2.statu != c_default && cha2.statu_lasting < 1) {
        cha2.statu = c_default;
        cha2.statu_lasting = 0;
    }
}

/**
 * 状态刷新与随机生成逻辑
 * 功能：控制游戏节奏，每隔一定回合随机给玩家或怪物施加新状态
 */
void renew_statu(void) {
    static int times = 0;           // 记录游戏总回合数（静态变量，保持持久化）
    static int no_statu_turns = 0;  // 记录距离上次出现状态经过的回合数（用于动态调整概率）

    times++; // 回合数+1

    // --- 第1回合特殊处理 ---
    if (times == 1) {
        if (random(2) == 1) { // 50%概率触发初始状态
            if (random(2) == 1) {
                // 随机给玩家施加状态 (random(3) 对应前3种状态)
                cha1.statu = (c_statu)random(3);
                cha1.statu_lasting = (int)cha1.statu + 1; // 持续时间与状态类型相关
            }
            else {
                // 随机给怪物施加状态
                cha2.statu = (c_statu)random(3);
                cha2.statu_lasting = (int)cha2.statu + 1;
            }
        }
        no_statu_turns += POSIBLE_ADJUST; // 增加未出现状态的计数
        return;
    }

    // --- 常规回合处理 ---
    // 每隔 TIME_STATU 个回合尝试生成新状态
    if (times % TIME_STATU == 0) {
        // 概率判定：随着 no_statu_turns 增加，触发概率会变化
        if (random(1 + no_statu_turns) != 1) {
            no_statu_turns = 1; // 重置计数

            if (random(2) == 1) {
                // 施加给玩家
                cha1.statu = (c_statu)random(4); // random(4) 包含 c_holy
                // 如果是神圣状态(c_holy)持续1回合，否则持续 (类型+1) 回合
                cha1.statu_lasting = (cha1.statu == c_holy) ? 1 : (int)cha1.statu + 1;
            }
            else {
                // 施加给怪物
                cha2.statu = (c_statu)random(3);
                cha2.statu_lasting = (int)cha2.statu + 1;
            }
        }
        no_statu_turns += POSIBLE_ADJUST; // 增加未出现状态的计数
    }
}

/**
 * 状态效果计算核心函数
 * @param types: 0代表计算玩家状态效果，1代表计算怪物状态效果
 * @param pa1: 指针，用于返回状态标志1 (如：流血 Bleedlust)
 * @param pa2: 指针，用于返回状态标志2 (如：狂暴 Berserk)
 * @param pa3: 指针，用于返回状态标志3 (如：虚弱 Weakened)
 * @param max_blood: 最大血量上限 (用于神圣状态计算)
 */
void compute_statu(int types, int* pa1, int* pa2, int* pa3, int max_blood) {
    // 初始化输出参数，-1 表示未触发或无效
    *pa1 = *pa2 = *pa3 = -1;

    switch (types) {
    case 0: // --- 玩家状态计算 ---
        switch (cha1.statu) {
        case c_default: return; // 默认状态无效果

        case c_bleedlust: // 嗜血/流血状态
            if (cha1.statu_lasting > 0) *pa1 = 1; // 激活标志位1
            return;

        case c_berserk: // 狂暴状态
            if (cha1.statu_lasting > 0) *pa2 = 1; // 激活标志位2
            return;

        case c_weakened: // 虚弱状态
            if (cha1.statu_lasting > 0) *pa3 = 1; // 激活标志位3
            return;

        case c_holy: // 神圣状态 (特殊逻辑)
            if (cha1.statu_lasting > 0) {
                int ok = 0;
                // 检查玩家手牌中是否有“处决(Execute)”卡
                for (int i = 0; i < 3; i++)
                    if (card[i].type == execute) ok = 1;

                if (!ok) {
                    // 如果没有处决卡：怪物血量减半
                    cha2.blood /= 2;
                    if (cha2.blood < 1) cha2.blood = 1;
                }
                else {
                    // 如果由处决卡：触发“生命交换”
                    // 怪物获得玩家一半血量（不超过上限）
                    if (cha1.blood / 2 + cha2.blood > max_blood)
                        cha2.blood = max_blood;
                    else
                        cha2.blood += cha1.blood / 2;

                    // 玩家失去一半血量
                    cha1.blood /= 2;
                    if (cha1.blood < 1) cha1.blood = 1;
                }
            }
            return;
        }
        break;

    case 1: // --- 怪物状态计算 (逻辑同上，对象互换) ---
        switch (cha2.statu) {
        case c_default: return;

        case c_bleedlust:
            if (cha2.statu_lasting > 0) *pa1 = 1;
            return;

        case c_berserk:
            if (cha2.statu_lasting > 0) *pa2 = 1;
            return;

        case c_weakened:
            if (cha2.statu_lasting > 0) *pa3 = 1;
            return;

        case c_holy:
            if (cha2.statu_lasting > 0) {
                int ok = 0;
                // 检查怪物手牌中是否有“处决”卡
                for (int i = 0; i < 5; i++)
                    if (monster[i].type == execute) ok = 1;

                if (!ok) {
                    // 无处决卡：玩家血量减半
                    cha1.blood /= 2;
                    if (cha1.blood < 1) cha1.blood = 1;
                }
                else {
                    // 有处决卡：生命交换
                    // 玩家获得怪物一半血量
                    if (cha2.blood / 2 + cha1.blood > max_blood)
                        cha1.blood = max_blood;
                    else
                        cha1.blood += cha2.blood / 2;

                    // 怪物失去一半血量
                    cha2.blood /= 2;
                    if (cha2.blood < 1) cha2.blood = 1;
                }
            }
            return;
        }
        break;
    }
}