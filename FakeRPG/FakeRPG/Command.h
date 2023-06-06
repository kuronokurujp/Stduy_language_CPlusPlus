#pragma once

typedef struct {
    char name[4 * 2 + 1];
} COMMAND_DATA;

/// <summary>
/// コマンド一覧
/// </summary>
enum {
    COMMAND_FIGHT = 0,
    COMMAND_SPELL,
    COMMAND_RUN,
    COMMAND_MAX,
};
