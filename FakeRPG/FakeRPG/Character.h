#pragma once

/// <summary>
/// キャラクター構造体
/// </summary>
typedef struct {
    int hp;
    int max_hp;
    int mp;
    int max_mp;
    char name[4 * 2 + 1];
    char aa[256];

    unsigned int command;

    int target;
    int attack;
} CHARACTER;

/// <summary>
/// モンスターの定義
/// </summary>
enum {
    MONSTER_PLAYER = 0,
    MONSTER_SLIME,
    MONSTER_MAX,
};

/// <summary>
/// キャラの種類
/// </summary>
enum {
    CHARACTER_PLAYER = 0,
    CHARACTER_MONSTER,
    CHARACTER_MAX,
};
