#include "entity/entity.h"
#include "config/config.h"
#include "../tool/tool.h"
#include "../game_logic/game_logic.h"

static int times[2] = { 0,0 };

void get_card(basiccard* err, int types) {
    int posible = 0;
    err->statu = 1;
    switch (types) {
    case 0:
        posible = random(POSIBLILITY);
        if (posible + times[0] <= 1) {
            times[0] = 0;
            err->type = (basiccardtype)(random(3) - 1);
            switch (err->type) {
            case execute:
                err->value = (random(VALUE_MAX) == 1) ? 2 : 1;
                break;
            case heal:
                err->value = random(VALUE_MAX);
                break;
            case weaken:
                err->value = (VALUE_MAX > 10) ? random(VALUE_MAX % 5 + 2) : random(VALUE_MAX);
                break;
            }
            return;
        }
        times[0]--;
        err->type = (basiccardtype)(random(2) + 2);
        err->value = random(VALUE_MAX);
        break;
    case 1:
        posible = random(POSIBLILITY);
        if (posible + times[1] <= 1) {
            times[1] = 0;
            err->type = (basiccardtype)(random(3) - 1);
            switch (err->type) {
            case execute:
                err->value = (random(VALUE_MAX) == 1) ? 2 : 1;
                break;
            case heal:
                err->value = random(VALUE_MAX);
                break;
            case weaken:
                err->value = (VALUE_MAX > 10) ? random(VALUE_MAX % 5 + 2) : random(VALUE_MAX);
                break;
            }
            return;
        }
        times[1]--;
        err->type = (basiccardtype)(random(2) + 2);
        err->value = random(VALUE_MAX);
        break;
    }
}

void card_order_change(int types, int size, int index) {
    basiccard middle;
    switch (types) {
    case 0:
        middle = card[index];
        for (int t = 0; t < size; t++)
            card[t + index] = card[t + index + 1];
        card[size + index] = middle;
        break;
    case 1:
        middle = monster[index];
        for (int t = 0; t < size; t++)
            monster[t + index] = monster[t + index + 1];
        monster[size + index] = middle;
        break;
    }
}