#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"
#include "Engine/Platform/PlatformInput.h"

namespace EnhancedInput
{
    /// <summary>
    /// 入力タイプ
    /// </summary>
    enum eInputType
    {
        eInputType_Keyboard = 0,
        eInputType_Touch,
    };

    /// <summary>
    /// 入力アクションデータ
    /// </summary>
    struct ActionData
    {
        using ActionKeyMap   = Core::Common::CustomFixVector<Platform::EKeyboard, 4>;
        using ActionTouchMap = Core::Common::CustomFixVector<Platform::EInputMouseType, 4>;

        // CustomFixMapクラスでバッファデータを作成時に使う
        ActionData() {}
        ActionData(const ActionKeyMap& in_arKeys) { this->aKeyboardKeys = in_arKeys; }
        ActionData(const ActionKeyMap& in_arKeys, const ActionTouchMap& in_arTouchs)
        {
            this->aKeyboardKeys = in_arKeys;
            this->aTouchs       = in_arTouchs;
        }

        // CustomFixMapクラスでデータをコピーする時に使う
        void operator=(const ActionData& in_rSrc)
        {
            this->aKeyboardKeys = in_rSrc.aKeyboardKeys;
            this->aTouchs       = in_rSrc.aTouchs;
        }

        /// <summary>
        /// キーボードキーデータ
        /// </summary>
        Core::Common::CustomFixVector<Platform::EKeyboard, 4> aKeyboardKeys;

        /// <summary>
        /// マウスやタッチ入力データ
        /// </summary>
        Core::Common::CustomFixVector<Platform::EInputMouseType, 4> aTouchs;
        // Core::Common::CustomArray<Uint32, 4> aButtons;
        // Core::Common::CustomArray<Uint32, 4> aSticks;
    };

    /// <summary>
    /// アクションを入力したデータ
    /// </summary>
    struct InputData
    {
        eInputType eType;

        union Item
        {
            struct Key
            {
                Platform::EKeyboard eKey;
            } keyboard;

            struct Touch
            {
                Platform::EInputMouseType eType;
                Float32 fX, fY;
            } touch;
        } item;
    };

    /// <summary>
    /// アクションデータのマップ型
    /// </summary>
    using ActionMap = Core::Common::CustomFixMap<Core::Common::FixString64, ActionData, 32>;

    /// <summary>
    /// 入力データのマップ型
    /// </summary>
    using InputMap = Core::Common::CustomFixMap<Core::Common::FixString64,
                                                Core::Common::CustomFixVector<InputData, 32>, 32>;

    /// <summary>
    /// インプット用の追加モジュール
    /// </summary>
    class EnhancedInputModule final : public Module::ModuleBase
    {
        HE_MODULE_GENRATE_DECLARATION(EnhancedInputModule);

    public:
        EnhancedInputModule();

        /// <summary>
        /// 共通利用のアクションをマッピングデータを設定
        /// </summary>
        /// <param name="in_mrTable"></param>
        void SetCommonMappingAction(const ActionMap&);

        /// <summary>
        /// 共通利用のアクションマッピングデータを追加
        /// </summary>
        /// <param name=""></param>
        void AddCommonMappingAction(const ActionMap&);
        /// <summary>
        /// 共通利用のアクションマッピングデータを外す
        /// </summary>
        /// <param name="in_pActonName"></param>
        void RemoveCommonMappingAction(const ActionMap&);
        void RemoveCommonMappingAction(const Char*);

        const InputMap& GetInputMap() const { return this->_mInputAction; }

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool _Start() override final;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        const Bool _Release() override final;

        const Bool _BeforeUpdate(const Float32 in_fDeltaTime) override final;
        const Bool _Update(const Float32 in_fDeltaTime) override final;

    private:
        /// <summary>
        /// マッピングしたアクションデータ
        /// </summary>
        ActionMap _mMappingAction;

        /// <summary>
        /// マッピングしたアクションの入力リスト
        /// </summary>
        InputMap _mInputAction;
    };

}  // namespace EnhancedInput
