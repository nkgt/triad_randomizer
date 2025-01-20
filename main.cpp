#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#include <chrono>
#include <cstdio>
#include <iostream>
#include <random>
#include <thread>

#include "triads.hpp"

#define SIZE(x) (uint16_t)(sizeof(x) / sizeof(x[0]))

#ifdef _WIN32
static inline bool wait_for_space() {
    while (true) {
        if (_kbhit()) {
            int c = _getch();
            return c == ' ';
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
#else
static inline void enable_raw_mode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

static inline void disable_raw_mode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

static inline bool wait_for_space() {
    enable_raw_mode();
    char c;
    while (true) {
        if (read(STDIN_FILENO, &c, 1) > 0) {
            disable_raw_mode();
            return c == ' ';
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
#endif

int main() {
    uint16_t indices[SIZE(triads)];
    for (uint16_t i = 0; i < SIZE(triads); ++i) {
        indices[i] = i;
    }

    std::random_device triads_rd;
    std::mt19937 triads_gen(triads_rd());

    while (true) {
        std::printf("Startin run...\n\n");
        std::shuffle(indices, indices + SIZE(triads), triads_gen);
        uint16_t i = 0;

        do {
            const triad t = triads[indices[i]];
            std::printf("%d/%u\nString: %s\nInversion: %s\nKey: %s\n\n", i + 1, SIZE(triads), t.s, t.inversion, t.key);
            ++i;
        } while (wait_for_space() && i < SIZE(triads));
    }
}
