#include "tool/tool.h"


void initialize(void) {
    memset(hash_char, -1, 257 * sizeof(int));
}

void set_priority(char arr[], int change_numbers) {
    initialize();
    for (int times = 0; times < change_numbers; times++) {
        if ((int)arr[times] >= 0 && (int)arr[times] <= 255) {
            hash_char[(int)arr[times]] = times;
        }
    }
    hash_char[256] = change_numbers;
}

int random(int random_number_area) {
    static int times = 0;
    times += 1;
    if (times % TIME_AROUND == 0)
        srand((unsigned char)time(NULL));
    if (times == 1)
        srand((unsigned char)time(NULL));
    return rand() % random_number_area + 1;
}

void flush_input(void) {
#ifdef _WIN32
    HANDLE hinput = GetStdHandle(STD_INPUT_HANDLE);
    if (hinput != INVALID_HANDLE_VALUE)
        FlushConsoleInputBuffer(hinput);
#else
    tcflush(STDIN_FILENO, TCIFLUSH);
#endif
}

void flush_output(void) {
#ifdef _WIN32
    HANDLE hconsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hconsole == INVALID_HANDLE_VALUE) return;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hconsole, &csbi)) return;
    COORD dwhome = { 0,0 };
    DWORD dwwritten;
    FillConsoleOutputCharacter(hconsole, ' ', csbi.dwSize.X * csbi.dwSize.Y, dwhome, &dwwritten);
    FillConsoleOutputAttribute(hconsole, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, dwhome, &dwwritten);
    SetConsoleCursorPosition(hconsole, dwhome);
#else
    printf("\033[2J\033[H");
    fflush(stdout);
#endif
}

int init_windows_ANSI(void) {
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hout == INVALID_HANDLE_VALUE) return 1;
    DWORD mode = 0;
    GetConsoleMode(hout, &mode);
    mode |= 0x0004;
    SetConsoleMode(hout, mode);
    return 0;
}

void get_screen_size(int* width, int* height) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hconsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hconsole, &csbi);
    *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void cursor_to_xy(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1);
    fflush(stdout);
}

void reset_cursor(int height_screen) {
    printf("\033[%d;1H", height_screen);
    fflush(stdout);
}