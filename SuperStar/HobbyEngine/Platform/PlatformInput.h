#pragma once

// プラットフォームのインプットシステムインターフェイス
#include "Core/Core.h"
#include "Core/Math/Math.h"
#include "Core/Type.h"

namespace Platform
{
    // キーボードの各キーコード
    enum EKeyboard
    {
        // バックスペースキー
        EKeyboard_BACK,
        // タブキー
        EKeyboard_TAB,
        // エンターキー
        EKeyboard_RETURN,
        // 左シフトキー
        EKeyboard_LSHIFT,
        // 右シフトキー
        EKeyboard_RSHIFT,
        // 左コントロールキー
        EKeyboard_LCONTROL,
        // 右コントロールキー
        EKeyboard_RCONTROL,
        // エスケープキー
        EKeyboard_ESCAPE,
        // スペースキー
        EKeyboard_SPACE,
        // ＰａｇｅＵＰキー
        EKeyboard_PGUP,
        // ＰａｇｅＤｏｗｎキー
        EKeyboard_PGDN,
        // エンドキー
        EKeyboard_END,
        // ホームキー
        EKeyboard_HOME,
        // 左キー
        EKeyboard_LEFT,
        // 上キー
        EKeyboard_UP,
        // 右キー
        EKeyboard_RIGHT,
        // 下キー
        EKeyboard_DOWN,
        // インサートキー
        EKeyboard_INSERT,
        // デリートキー
        EKeyboard_DELETE,
        // －キー
        EKeyboard_MINUS,
        // ￥キー
        EKeyboard_YEN,
        // ＾キー
        EKeyboard_PREVTRACK,
        // ．キー
        EKeyboard_PERIOD,
        // ／キー
        EKeyboard_SLASH,
        // 左ＡＬＴキー
        EKeyboard_LALT,
        // 右ＡＬＴキー
        EKeyboard_RALT,
        // ScrollLockキー
        EKeyboard_SCROLL,
        // ；キー
        EKeyboard_SEMICOLON,
        // ：キー
        EKeyboard_COLON,
        // ［キー
        EKeyboard_LBRACKET,
        // ］キー
        EKeyboard_RBRACKET,
        // ＠キー
        EKeyboard_AT,
        // ＼キー
        EKeyboard_BACKSLASH,
        // ，キー
        EKeyboard_COMMA,
        EKeyboard_KANJI,
        EKeyboard_CONVERT,
        EKeyboard_NOCONVERT,
        EKeyboard_KANA,
        EKeyboard_APPS,
        // CaspLockキー
        EKeyboard_CAPSLOCK,
        EKeyboard_SYSRQ,
        // PauseBreakキー
        EKeyboard_PAUSE,
        EKeyboard_LWIN,
        EKeyboard_RWIN,
        EKeyboard_NUMLOCK,
        // テンキー０
        EKeyboard_NUMPAD0,
        // テンキー１
        EKeyboard_NUMPAD1,
        // テンキー２
        EKeyboard_NUMPAD2,
        // テンキー３
        EKeyboard_NUMPAD3,
        // テンキー４
        EKeyboard_NUMPAD4,
        // テンキー５
        EKeyboard_NUMPAD5,
        // テンキー６
        EKeyboard_NUMPAD6,
        // テンキー７
        EKeyboard_NUMPAD7,
        // テンキー８
        EKeyboard_NUMPAD8,
        // テンキー９
        EKeyboard_NUMPAD9,
        // テンキー＊キー
        EKeyboard_MULTIPLY,
        // テンキー＋キー
        EKeyboard_ADD,
        // テンキー－キー
        EKeyboard_SUBTRACT,
        // テンキー．キー
        EKeyboard_DECIMAL,
        // テンキー／キー
        EKeyboard_DIVIDE,
        // テンキーのエンターキー
        EKeyboard_NUMPADENTER,
        // Ｆ１キー
        EKeyboard_F1,
        // Ｆ２キー
        EKeyboard_F2,
        // Ｆ３キー
        EKeyboard_F3,
        // Ｆ４キー
        EKeyboard_F4,
        // Ｆ５キー
        EKeyboard_F5,
        // Ｆ６キー
        EKeyboard_F6,
        // Ｆ７キー
        EKeyboard_F7,
        // Ｆ８キー
        EKeyboard_F8,
        // Ｆ９キー
        EKeyboard_F9,
        // Ｆ１０キー
        EKeyboard_F10,
        // Ｆ１１キー
        EKeyboard_F11,
        // Ｆ１２キー
        EKeyboard_F12,
        // Ａキー
        EKeyboard_A,
        // Ｂキー
        EKeyboard_B,
        // Ｃキー
        EKeyboard_C,
        // Ｄキー
        EKeyboard_D,
        // Ｅキー
        EKeyboard_E,
        // Ｆキー
        EKeyboard_F,
        // Ｇキー
        EKeyboard_G,
        // Ｈキー
        EKeyboard_H,
        // Ｉキー
        EKeyboard_I,
        // Ｊキー
        EKeyboard_J,
        // Ｋキー
        EKeyboard_K,
        // Ｌキー
        EKeyboard_L,
        // Ｍキー
        EKeyboard_M,
        // Ｎキー
        EKeyboard_N,
        // Ｏキー
        EKeyboard_O,
        // Ｐキー
        EKeyboard_P,
        // Ｑキー
        EKeyboard_Q,
        // Ｒキー
        EKeyboard_R,
        // Ｓキー
        EKeyboard_S,
        // Ｔキー
        EKeyboard_T,
        // Ｕキー
        EKeyboard_U,
        // Ｖキー
        EKeyboard_V,
        // Ｗキー
        EKeyboard_W,
        // Ｘキー
        EKeyboard_X,
        // Ｙキー
        EKeyboard_Y,
        // Ｚキー
        EKeyboard_Z,
        // ０キー
        EKeyboard_0,
        // １キー
        EKeyboard_1,
        // ２キー
        EKeyboard_2,
        // ３キー
        EKeyboard_3,
        // ４キー
        EKeyboard_4,
        // ５キー
        EKeyboard_5,
        // ６キー
        EKeyboard_6,
        // ７キー
        EKeyboard_7,
        // ８キー
        EKeyboard_8,
        // ９キー
        EKeyboard_9,

        // キーコード最大
        EKeyboard_MAX,
    };

    /// <summary>
    /// 入力ボタン状態
    /// </summary>
    enum EInputState
    {
        EInputState_NONE = 0,
        EInputState_PRESSED,
        EInputState_RELEASED,
        EInputState_HOLD,
    };

    /// <summary>
    /// マウスタイプ
    /// </summary>
    enum EInputMouseType
    {
        EInputMouseType_Left   = 0x01,
        EInputMouseType_Right  = 0x02,
        EInputMouseType_Middle = 0x04,
        EInputMouseType_All    = 0xff,
    };

    /// <summary>
    /// キーボード状態
    /// </summary>
    class KeyboardInput
    {
    public:
        /// <summary>
        /// 指定キーを押しているか
        /// </summary>
        const Bool GetKeyValue(const EKeyboard in_keyCode) const
        {
            return (this->_currState[in_keyCode] != EInputState_NONE);
        }

        /// <summary>
        /// 指定キーの入力状態
        /// </summary>
        const EInputState GetKeyState(const EKeyboard in_keyCode) const;

    public:
        Uint8 _currState[EKeyboard::EKeyboard_MAX];
        Uint8 _prevState[EKeyboard::EKeyboard_MAX];
    };

    /// <summary>
    /// タッチ状態
    /// </summary>
    class TouchInput
    {
    public:
        const Core::Math::Vector2& GetWorldPos() const { return this->_pos; }

        const Bool GetTouchValue(const EInputMouseType) const;
        const EInputState GetTouchState(const EInputMouseType) const;

        // TODO: 指定した矩形がタッチ範囲かどうか
        const Bool IsTouchInRect(const Core::Math::Rect2 in_rect) const;

    public:
        Core::Math::Vector2 _pos;
        Uint32 _currTouchState;
        Uint32 _prevTouchState;
    };

    /// <summary>
    /// キーやタッチなどの入力状態
    /// </summary>
    struct InputState
    {
        KeyboardInput _keyboard;
        TouchInput _touch;
    };

    /// <summary>
    /// TODO: プラットフォームのインプットシステム
    /// マウス, キーボード, コントローラーを対応
    /// </summary>
    class InputSystemInterface
    {
    public:
        virtual void Init()                                   = 0;
        virtual void BeforeUpdate(const Float32 in_deltaTime) = 0;
        virtual void Update(const Float32 in_deltaTime)       = 0;
        virtual void AfterUpdate(const Float32 in_deltaTime)  = 0;

        const InputState& GetState() const { return this->_state; }

    protected:
        InputState _state;
    };
}  // namespace Platform
