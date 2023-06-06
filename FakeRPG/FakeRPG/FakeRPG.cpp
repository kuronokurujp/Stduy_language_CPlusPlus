// FakeRPG.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <stdio.h>
#include <time.h>

#include "ConsoleFunction.h"

#include "Character.h"
#include "Command.h"

enum {
    GAME_STATE_ENCOUNT = 0,
    GAME_STATE_BATTLE_THINKING,
    GAME_STATE_BATTLE_ACTION,
    GAME_STATE_ENDING,
};

typedef struct {
    int state;
    int battle_char_turn;
} GAME_DATA;

static GAME_DATA s_game_data = {
    GAME_STATE_ENCOUNT,
};

static COMMAND_DATA s_commands[COMMAND_MAX] = {
    {
        "たたかう"
    },

    {
        "じゅもん"
    },

    {
        "にげる"
    },
};

static CHARACTER s_monsters[MONSTER_MAX] = {
    {
        15,
        15,
        15,
        15,
        "ゆうしゃ",
        "",
        COMMAND_FIGHT,
        0,
        3,
    },
    {
        3,
        3,
        0,
        0,
        "スライム",
        "（　＾ω＾）・・・",
        COMMAND_FIGHT,
        0,
        1,
    }
};

static CHARACTER s_characters[CHARACTER_MAX];

static void Init();
static void Wait();

static void Encount(const int monster);
static int BattleThiking(const int char_type);
static int BattleAction(const int char_type);
static void Ending();

static void DrawCharacter(const int character);
static void DrawPlayer();
static void DrawMonster();
static void DrawSpace();
static void DrawCommandList(const int select_command);

static void DrawMessageSelectCommand(const CHARACTER& character);
static void DrawMessageMonsterEncount(const char* monster_name);
static void DrawMessageFight(const char* char_name);
static void DrawMessageDamage(const CHARACTER& damage_character, const int damage);
static void DrawMessageCharacterKnockDown(const int char_type, const CHARACTER& char_data);
static void DrawMessageGameClear();

static void Init()
{
    srand((unsigned int)time(nullptr));

    s_characters[CHARACTER_PLAYER] = s_monsters[MONSTER_PLAYER];
    s_game_data.battle_char_turn = CHARACTER_PLAYER;
    s_game_data.state = GAME_STATE_ENCOUNT;
}

static void Wait()
{
    _getch();
}

static void Encount(const int monster)
{
    s_characters[CHARACTER_MONSTER] = s_monsters[monster];

    DrawPlayer();
    DrawSpace();
    DrawSpace();

    DrawMonster();
    DrawSpace();

    DrawMessageMonsterEncount(s_characters[CHARACTER_MONSTER].name);

    Wait();

    // 攻撃対象を設定
    s_characters[CHARACTER_PLAYER].target = CHARACTER_MONSTER;
    s_characters[CHARACTER_MONSTER].target = CHARACTER_PLAYER;
}

static int BattleThinking(const int char_type)
{
    DrawPlayer();
    DrawSpace();
    DrawSpace();

    DrawMonster();
    DrawSpace();
    DrawSpace();

    bool command_run = false;
    int next_state = GAME_STATE_BATTLE_THINKING;

    switch (char_type)
    {
    case CHARACTER_PLAYER:
    {
        DrawCommandList(s_characters[char_type].command);

        bool enter = false;
        unsigned int command = s_characters[char_type].command;
        switch (_getch())
        {
        case 'w': {
            --command;
            break;
        }
        case 's': {
            ++command;
            break;
        }
        default: {
            enter = true;
            break;
        }
        }

        s_characters[char_type].command = command % COMMAND_MAX;

        if (enter)
        {
            command_run = true;
        }

        break;
    }
    case CHARACTER_MONSTER: {
        command_run = true;
        break;
    }
    }

    if (command_run)
    {
        DrawMessageSelectCommand(s_characters[char_type]);
        next_state = GAME_STATE_BATTLE_ACTION;
    }

    return next_state;
}

static int BattleAction(const int char_type)
{
    DrawPlayer();
    DrawSpace();
    DrawSpace();

    DrawMonster();
    DrawSpace();
    DrawSpace();

    bool command_run = false;
    int next_state = GAME_STATE_BATTLE_ACTION;

    switch (s_characters[char_type].command)
    {
    case COMMAND_FIGHT:
    {
        int damage = 1 + rand() % s_characters[char_type].attack;
        CHARACTER* target_char = &s_characters[s_characters[char_type].target];

        target_char->hp -= damage;
        if (target_char->hp < 0)
            target_char->hp = 0;

        DrawMessageDamage(*target_char, damage);

        break;
    }
    case COMMAND_SPELL: {
        break;
    }
    case COMMAND_RUN: {
        break;
    }
    }

    CHARACTER* target_char = &s_characters[s_characters[char_type].target];
    switch (s_characters[char_type].target)
    {
    case CHARACTER_PLAYER:
    {
        if (target_char->hp <= 0) {
        }

        break;
    }
    case CHARACTER_MONSTER:
    {
        if (target_char->hp <= 0) {
            DrawMessageCharacterKnockDown(s_characters[char_type].target, *target_char);
        }

        break;
    }
    }

    if (target_char->hp <= 0) {
        next_state = GAME_STATE_ENDING;
    }
    else
    {
        next_state = GAME_STATE_BATTLE_THINKING;
    }

    return next_state;
}

static void Ending()
{
    DrawPlayer();
    DrawSpace();
    DrawSpace();

    DrawMessageGameClear();
}

static void DrawSpace()
{
    printf("\n");
}

static void DrawPlayer()
{
    printf("%s", s_characters[CHARACTER_PLAYER].name);

    DrawSpace();

    printf("HP : %d/%d MP : %d/%d",
        s_characters[CHARACTER_PLAYER].hp,
        s_characters[CHARACTER_PLAYER].max_hp,
        s_characters[CHARACTER_PLAYER].mp,
        s_characters[CHARACTER_PLAYER].max_mp);
}

static void DrawMonster()
{
    if (0 < s_characters[CHARACTER_MONSTER].hp)
        printf("%s ", s_characters[CHARACTER_MONSTER].aa);
    else
        DrawSpace();

    printf("(HP : %d/%d)",
        s_characters[CHARACTER_MONSTER].hp,
        s_characters[CHARACTER_MONSTER].max_hp);
}

static void DrawCommandList(const int select_command)
{
    for (int i = 0; i < _countof(s_commands); ++i)
    {
        if (i == select_command)
        {
            printf(">");
        }
        else
        {
            printf(" ");
        }

        printf("%s", s_commands[i].name);
        DrawSpace();
    }
}

static void DrawMessageSelectCommand(const CHARACTER& character)
{
    switch (character.command)
    {
    case COMMAND_FIGHT: {
        DrawMessageFight(character.name);

        break;
    }
    case COMMAND_SPELL: {
        break;
    }
    case COMMAND_RUN: {
        break;
    }
    }
}

static void DrawMessageFight(const char* char_name)
{
    printf("%sのこうげき!", char_name);
    DrawSpace();
}

static void DrawMessageDamage(const CHARACTER& damage_character, const int damage)
{
    printf("%sに %dのダメージ!", damage_character.name, damage);
    DrawSpace();
}

static void DrawMessageMonsterEncount(const char* monster_name)
{
    printf("%sが　あらわれた", monster_name);
    DrawSpace();
}

static void DrawMessageCharacterKnockDown(const int char_type, const CHARACTER& char_data)
{
    switch (char_type)
    {
    case CHARACTER_PLAYER: {
        break;
    }
    case CHARACTER_MONSTER: {
        printf("%sをたおした!", char_data.name);
        DrawSpace();
        break;
    }
    }
}

static void DrawMessageGameClear()
{
    printf("ゲームクリアー");
}

int main()
{
    // 文字コード設定
    //setlocale(LC_ALL, "en_US.utf8");

    // コンソールのカーソルを非表示
    Console::SetDisplayCursor(false);

    // コンソールのフォント設定
    Console::SetFontSize(72);

    // コンソールサイスを変える
    Console::SetScreenSize(32, 10);

    // コンソール画面をフルスクリーンにする
    Console::SwitchFullScreenMode(true);

    Init();
    while (true) {
        // コンソールのカーソルを非表示
        Console::SetDisplayCursor(false);

        Console::Cls();

        switch (s_game_data.state)
        {
        case GAME_STATE_ENCOUNT:
        {
            Encount(MONSTER_SLIME);

            s_game_data.state = GAME_STATE_BATTLE_THINKING;
            s_game_data.battle_char_turn = CHARACTER_PLAYER;

            break;
        }
        case GAME_STATE_BATTLE_THINKING:
        {
            int next_state = BattleThinking(s_game_data.battle_char_turn);
            if (s_game_data.state != next_state)
                s_game_data.state = next_state;

            Wait();

            break;
        }
        case GAME_STATE_BATTLE_ACTION:
        {
            int next_state = BattleAction(s_game_data.battle_char_turn);
            if (s_game_data.state != next_state)
            {
                s_game_data.state = next_state;

                s_game_data.battle_char_turn += 1;
                s_game_data.battle_char_turn = s_game_data.battle_char_turn % _countof(s_characters);
            }

            Wait();

            break;
        }
        case GAME_STATE_ENDING: {
            Ending();
            break;
        }
        }
    }
}
