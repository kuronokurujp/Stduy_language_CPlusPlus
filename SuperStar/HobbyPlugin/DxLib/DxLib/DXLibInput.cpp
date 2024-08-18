#include "DXLibInput.h"

#include "DxLib.h"

// DxLib用の入力システムの実装

namespace DXLib
{
    static const Sint32 s_iaKeyMaps[Platform::EKeyboard::EKeyboard_MAX] = {
        KEY_INPUT_BACK,        KEY_INPUT_TAB,       KEY_INPUT_RETURN,   KEY_INPUT_LSHIFT,
        KEY_INPUT_RSHIFT,      KEY_INPUT_LCONTROL,  KEY_INPUT_RCONTROL, KEY_INPUT_ESCAPE,
        KEY_INPUT_SPACE,       KEY_INPUT_PGUP,      KEY_INPUT_PGDN,     KEY_INPUT_END,
        KEY_INPUT_HOME,        KEY_INPUT_LEFT,      KEY_INPUT_UP,       KEY_INPUT_RIGHT,
        KEY_INPUT_DOWN,        KEY_INPUT_INSERT,    KEY_INPUT_DELETE,   KEY_INPUT_MINUS,
        KEY_INPUT_YEN,         KEY_INPUT_PREVTRACK, KEY_INPUT_PERIOD,   KEY_INPUT_SLASH,
        KEY_INPUT_LALT,        KEY_INPUT_RALT,      KEY_INPUT_SCROLL,   KEY_INPUT_SEMICOLON,
        KEY_INPUT_COLON,       KEY_INPUT_LBRACKET,  KEY_INPUT_RBRACKET, KEY_INPUT_AT,
        KEY_INPUT_BACKSLASH,   KEY_INPUT_COMMA,     KEY_INPUT_KANJI,    KEY_INPUT_CONVERT,
        KEY_INPUT_NOCONVERT,   KEY_INPUT_KANA,      KEY_INPUT_APPS,     KEY_INPUT_CAPSLOCK,
        KEY_INPUT_SYSRQ,       KEY_INPUT_PAUSE,     KEY_INPUT_LWIN,     KEY_INPUT_RWIN,
        KEY_INPUT_NUMLOCK,     KEY_INPUT_NUMPAD0,   KEY_INPUT_NUMPAD1,  KEY_INPUT_NUMPAD2,
        KEY_INPUT_NUMPAD3,     KEY_INPUT_NUMPAD4,   KEY_INPUT_NUMPAD5,  KEY_INPUT_NUMPAD6,
        KEY_INPUT_NUMPAD7,     KEY_INPUT_NUMPAD8,   KEY_INPUT_NUMPAD9,  KEY_INPUT_MULTIPLY,
        KEY_INPUT_ADD,         KEY_INPUT_SUBTRACT,  KEY_INPUT_DECIMAL,  KEY_INPUT_DIVIDE,
        KEY_INPUT_NUMPADENTER, KEY_INPUT_F1,        KEY_INPUT_F2,       KEY_INPUT_F3,
        KEY_INPUT_F4,          KEY_INPUT_F5,        KEY_INPUT_F6,       KEY_INPUT_F7,
        KEY_INPUT_F8,          KEY_INPUT_F9,        KEY_INPUT_F10,      KEY_INPUT_F11,
        KEY_INPUT_F12,         KEY_INPUT_A,         KEY_INPUT_B,        KEY_INPUT_C,
        KEY_INPUT_D,           KEY_INPUT_E,         KEY_INPUT_F,        KEY_INPUT_G,
        KEY_INPUT_H,           KEY_INPUT_I,         KEY_INPUT_J,        KEY_INPUT_K,
        KEY_INPUT_L,           KEY_INPUT_M,         KEY_INPUT_N,        KEY_INPUT_O,
        KEY_INPUT_P,           KEY_INPUT_Q,         KEY_INPUT_R,        KEY_INPUT_S,
        KEY_INPUT_T,           KEY_INPUT_U,         KEY_INPUT_V,        KEY_INPUT_W,
        KEY_INPUT_X,           KEY_INPUT_Y,         KEY_INPUT_Z,        KEY_INPUT_0,
        KEY_INPUT_1,           KEY_INPUT_2,         KEY_INPUT_3,        KEY_INPUT_4,
        KEY_INPUT_5,           KEY_INPUT_6,         KEY_INPUT_7,        KEY_INPUT_8,
        KEY_INPUT_9,
    };

    void InputSystem::Init()
    {
        // キー入力初期化
        {
            for (Uint32 i = 0; i < Platform::EKeyboard::EKeyboard_MAX; ++i)
            {
                this->_state._keyboard._aCurrState[i] = CheckHitKey(s_iaKeyMaps[i]);
            }

            ::memset(this->_state._keyboard._aPrevState, Platform::EInputState::EInputState_NONE,
                     HE_ARRAY_SIZE(this->_state._keyboard._aPrevState));
        }

        // タッチ入力初期化
        {
            int x = 0;
            int y = 0;
            GetMousePoint(&x, &y);
            this->_state._touch._pos._fX = static_cast<Float32>(x);
            this->_state._touch._pos._fY = static_cast<Float32>(y);

            this->_uCurrButton = GetMouseInput();
            this->_uPrevButton = Platform::EInputState::EInputState_NONE;
        }
    }

    void InputSystem::Update(const Float32 in_fDeltaTime)
    {
        // キー入力の処理
        {
            for (Uint32 i = 0; i < Platform::EKeyboard::EKeyboard_MAX; ++i)
            {
                // キーの前情報を設定
                this->_state._keyboard._aPrevState[i] = this->_state._keyboard._aCurrState[i];
                // キーの新しい情報を設定
                this->_state._keyboard._aCurrState[i] = CheckHitKey(s_iaKeyMaps[i]);
            }
        }

        // タッチ入力
        {
            int x = 0;
            int y = 0;
            GetMousePoint(&x, &y);
            this->_state._touch._pos._fX = static_cast<Float32>(x);
            this->_state._touch._pos._fY = static_cast<Float32>(y);

            this->_uPrevButton = this->_state._touch._uCurrTouchState;
            this->_uCurrButton = GetMouseInput();

            this->_state._touch._uPrevTouchState = 0;
            this->_state._touch._uCurrTouchState = 0;
            if (this->_uPrevButton & MOUSE_INPUT_LEFT)
                this->_state._touch._uPrevTouchState |= Platform::EInputMouseType_Left;
            if (this->_uPrevButton & MOUSE_INPUT_RIGHT)
                this->_state._touch._uPrevTouchState |= Platform::EInputMouseType_Right;
            if (this->_uPrevButton & MOUSE_INPUT_MIDDLE)
                this->_state._touch._uPrevTouchState |= Platform::EInputMouseType_Middle;

            if (this->_uCurrButton & MOUSE_INPUT_LEFT)
                this->_state._touch._uCurrTouchState |= Platform::EInputMouseType_Left;
            if (this->_uCurrButton & MOUSE_INPUT_RIGHT)
                this->_state._touch._uCurrTouchState |= Platform::EInputMouseType_Right;
            if (this->_uCurrButton & MOUSE_INPUT_MIDDLE)
                this->_state._touch._uCurrTouchState |= Platform::EInputMouseType_Middle;
        }
    }
}  // namespace DXLib
