#include "baseheadfiles/basehead.h"
#include "tool/tool.h"
#include "memory/memory.h"
#include "game_logic/game_logic.h"
#include "interaction/interaction.h"
#include "game_statu/game_statu.h"

int main() {
#ifdef _WIN32
    if (init_windows_ANSI() == 1) {
        printf("ø™∆ÙANSI ß∞‹£°\n");
        return 0;
    }
#endif
    init_bg();
    if (game_start() != 0) {
        printf("≥Ã–Ú÷’÷π£°\n");
        return 0;
    }
    int max_blood = 100;
    int max_pro = 15;
    map_border(background, INIT);
    game_data_init(max_blood, max_pro);
    map_draw(background);
    int w, h;
    get_screen_size(&w, &h);

    while (1) {
        compute(info_get(2, h - 8), max_blood, max_pro);
        renew_card();
        renew_map();
        flush_output();
        renew_statu();
        map_draw(background);
        if (game_end() == 0) {
            char c;
            while (1) {
                printf("  ‰»ÎyÕÀ≥ˆ£∫");
                scanf_s(" %c", &c, 1);
                if (c == 'y' || c == 'Y') break;
                flush_output();
            }
            break;
        }
    }
    return 0;
}