#include "interaction/interaction.h"
#include "../tool/tool.h"
#include "../memory/memory.h"
#include "../game_logic/game_logic.h"
extern character cha1, cha2;

int game_start(void) {
    printf("游戏是否开始？\n");
    printf("开始（输入字符Y游戏开始）\n");
    char s;
    scanf_s("%c", &s, 1);
    if (s == 'y' || s == 'Y') {
        printf("游戏开始！\n");
        flush_output();
        return 0;
    }
    return 1;
}

int info_get(int x, int y) {
    memcpy(background->maps + (BG_ROW - 3) * BG_COLUMN + 1, nothing, BG_COLUMN - 2);
    cursor_to_xy(x, y);
    printf("请选择打出的卡牌序号! 选择：1 / 2 / 3");
    cursor_to_xy(x, y + 1);
    int n;
    scanf_s("%d", &n);
    return n;
}

int game_end(void) {
    if (cha1.blood < 1 && cha2.blood < 1) {
        printf("平局！");
        return 0;
    }
    if (cha1.blood < 1) {
        printf("游戏失败！");
        return 0;
    }
    if (cha2.blood < 1) {
        printf("游戏获胜！");
        return 0;
    }
    return 1;
}