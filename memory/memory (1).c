#include "memory/memory.h"
#include "../tool/tool.h"
#include "../config/config.h"

//全局地图
map* background = NULL;

//游戏战斗界面
char card_bg[BG_ROW][BG_COLUMN] = {
    "                                                                                                    ",
    "   P1 HP:        **********************player            monster**********************        :P2   ",
    "   +--------------------------------------------------------------------------------------------+   ",
    "   |                                                                                            |   ",
    "   |                                   BATTLE ZONE                                    |   ",
    "   |                                                                                            |   ",
    "   +--------------------------------------------------------------------------------------------+   ",
    "   STATU:                                                                                  :STATU   ",
    "                                                                                                    ",
    "       attack(A)    heal(H)     execute(E)    protect(P)    weaken(W)                               ",
    "                                                                                                    ",
    "                                                                                                    ",
    "                                                                                                    ",
    "     1     2     3                                                                          next    ",
    "                                                                                                    ",
    "   [    ][    ][    ]                                              [    ][    ][    ][    ][    ]   ",
    "    #     #     #                                                   v     v     v     v     #       ",
    "       Your Hand                                                                Enemy               ",
    "                                                                                                    ",
    "                                                                                                    ",
    "                                                                                                    ",
    "                                                                                                    ",
    "                                                                                                    ",
    "                                                                                                    ",
    "                                                                                                    "
};
//填充字符
char nothing[120] = "                                                                                                                        ";

//初始化全局背景
void init_bg(void) {
    if (background != NULL) return;
    //分配地图结构体内存
    background = (map*)malloc(1 * sizeof(map));
    if (background == NULL) return;
    //绑定界面数组及尺寸
    background->maps = card_bg[0];
    background->column = BG_COLUMN;
    background->row = BG_ROW;
}

//创建指定行列的空地图，初始全空格
map* map_init(int columns, int rows) {
    if (columns < 1 || rows < 1) return NULL;
    map* MAP = (map*)malloc(1 * sizeof(map));
    if (MAP == NULL) return NULL;
    MAP->column = columns;
    MAP->row = rows;
    MAP->maps = (char*)malloc(columns * rows * sizeof(char));
    if (MAP->maps == NULL) return NULL;
    memset(MAP->maps, ' ', columns * rows * sizeof(char));
    return MAP;
}

//销毁地图释放内存
void map_destroy(map* ptr) {
    if (ptr != NULL) {
        if (ptr->maps != NULL) free(ptr->maps);
        free(ptr);
    }
}

//批量绘制或擦除
int map_generate(int* x1, int* y1, int* x2, int* y2, int change_times, map* target_area, char class, type types) {
    if (x1 == NULL || x2 == NULL || y1 == NULL || y2 == NULL || target_area == NULL || target_area->maps == NULL)
        return 1;
    int column = target_area->column;
    int row = target_area->row;
    switch (types) {
    case ERASE: memset(target_area->maps, ' ', column * row * sizeof(char)); break;
    case GENERATE: break;
    }
    char* ptr = target_area->maps;
    int pen_x = x1[0], pen_y = y1[0], end_x = x2[0], end_y = y2[0];
    for (int times = 0; times < change_times; times++) {
        pen_x = x1[times];
        pen_y = y1[times];
        end_x = x2[times];
        end_y = y2[times];
        if (pen_x > end_x || pen_y > end_y) continue;
        for (int offset_y = 0; offset_y <= end_y - pen_y; offset_y++) {
            for (int offset_x = 0; offset_x <= end_x - pen_x; offset_x++) {
                if (pen_x + offset_x > column - 1 || pen_y + offset_y > row - 1 || pen_x + offset_x < 0 || pen_y + offset_y < 0)
                    continue;
                ptr[column * (pen_y + offset_y) + (pen_x + offset_x)] = class;
            }
        }
    }
    return 0;
}

//绘制地图边框
void map_border(map* a, type types) {
    if (a == NULL || a->maps == NULL) return;
    int column = a->column;
    int row = a->row;
    char* ptr = a->maps;
    char border = '+', row_line = '|', column_line = '-';
    switch (types) {
    case DEFAULT: break;
    case ERASE: border = ' '; row_line = ' '; column_line = ' '; break;
    case INIT: break;
    }
    for (int rounds = 1; rounds < column - 1; rounds++) {
        ptr[rounds] = column_line;
        ptr[column * (row - 1) + rounds] = column_line;
    }
    for (int round2 = 1; round2 < row - 1; round2++) {
        ptr[column * round2] = row_line;
        ptr[(round2 + 1) * column - 1] = row_line;
    }
    ptr[0] = border;
    ptr[column - 1] = border;
    ptr[column * row - 1] = border;
    ptr[column * (row - 1)] = border;
}

//合并两张地图
map* map_combine(map* a, map* b, int locate_x, int locate_y, char* priority, int char_numbers) {
    if (a == NULL || b == NULL) return NULL;
    if (a->maps == NULL || b->maps == NULL) return NULL;
    int num[4] = { a->column,a->row,b->column,b->row };
    int left = (locate_x > 0) ? 0 : locate_x;
    int right = (num[0] > locate_x + num[2]) ? num[0] : locate_x + num[2];
    int top = (locate_y > 0) ? 0 : locate_y;
    int bottom = (num[1] > locate_y + num[3]) ? num[1] : locate_y + num[3];
    int total_x = right - left;
    int total_y = bottom - top;
    map* big_map = (map*)malloc(1 * sizeof(map));
    if (big_map == NULL) return NULL;
    big_map->column = total_x;
    big_map->row = total_y;
    big_map->maps = (char*)malloc(total_x * total_y * sizeof(char));
    if (big_map->maps == NULL) { free(big_map); return NULL; }
    char* ptr = big_map->maps;
    memset(ptr, ' ', total_x * total_y * sizeof(char));
    if (priority != NULL) set_priority(priority, char_numbers);
    for (int order_y = 0; order_y < total_y; order_y++) {
        for (int order_x = 0; order_x < total_x; order_x++) {
            int global_x = order_x + left;
            int global_y = order_y + top;
            int a_ch = 256, b_ch = 256;
            if (global_x >= 0 && global_x < num[0] && global_y >= 0 && global_y < num[1])
                a_ch = (int)(unsigned char)a->maps[global_y * num[0] + global_x];
            if (global_x >= locate_x && global_x < num[2] + locate_x && global_y >= locate_y && global_y < num[3] + locate_y)
                b_ch = (int)(unsigned char)b->maps[(global_y - locate_y) * num[2] + global_x - locate_x];
            if (priority != NULL) {
                int sa = (a_ch >= 0 && a_ch < 256);
                int sb = (b_ch >= 0 && b_ch < 256);
                if (sa && sb)
                    ptr[order_y * total_x + order_x] = (hash_char[a_ch] > hash_char[b_ch]) ? (char)b_ch : (char)a_ch;
                else if (sa && !sb)
                    ptr[order_y * total_x + order_x] = (char)a_ch;
                else if (!sa && sb)
                    ptr[order_y * total_x + order_x] = (char)b_ch;
            }
            else {
                if (a_ch == 256 && b_ch < 256)
                    ptr[order_y * total_x + order_x] = (char)b_ch;
                else if (a_ch < 256)
                    ptr[order_y * total_x + order_x] = (char)a_ch;
            }
        }
    }
    return big_map;
}

//输出地图至控制台
void map_draw(map* map) {
    if (map == NULL || map->maps == NULL) return;
    int row = map->row, line = map->column;
    char* ptr = map->maps;
    for (int rows = 0; rows < row; rows++) {
        for (int lines = 0; lines < line; lines++) {
            printf("%c", ptr[lines + line * rows]);
            if (lines + 1 == line) printf("\n");
        }
    }
}
