#include "MiniEngine.h"
#include "Engine.h"

// デバッグ用なのでリリース版には含めない
#ifdef _HOBBY_ENGINE_DEBUG

#include "Level/LevelLauncher.h"

#include "Core/Math/Math.h"
#include "Core/Common/FixMap.h"

// UIモジュール
#include "HobbyPlugin/UI/UIModule.h"
#include "HobbyPlugin/UI/Component/Widget/UIText.h"
#include "HobbyPlugin/UI/Component/Widget/UIButton.h"

// 描画モジュール
#include "HobbyPlugin/Render/RenderType.h"
// ローカライズモジュール
#include "HobbyPlugin/Localization/LocalizationModule.h"

// TODO: デバッグ画面を表示してレベルを切り替える

namespace Level
{
    // 偏移先
    typedef struct
    {
        const Char*	pName;
        Core::Math::Vector3 pos;
        Core::Math::Rect2 rect;

    } _DEBUG_LIST_ITEM_ST;

    typedef Core::Common::FixMap<Core::Common::FixString16, _DEBUG_LIST_ITEM_ST, 128> ST_SEQ_MAP;
    ST_SEQ_MAP s_seqMap;

    const Bool LevelLauncher::Begin()
    {
        const Bool bRet = Level::Node::Begin();
        E_ASSERT(bRet);

        {
            s_seqMap.Clear();
            s_seqMap.Add(
                E_STR_TEXT("C_TitleSeq"),
                {
                    E_STR_TEXT("TitleLevelName"),
                    Core::Math::Vector3(100.0f, 50.0f, 0.0f),
                    Core::Math::Rect2(0, 0, 300, 50)
                });
            s_seqMap.Add(
                E_STR_TEXT("C_PlaySeq"),
                {
                    E_STR_TEXT("MainGameLevelName"),
                    Core::Math::Vector3(100.0f, 100.0f + 10.0f, 0.0f),
                    Core::Math::Rect2(0, 0, 300, 50)
                });
            s_seqMap.Add(
                E_STR_TEXT("C_SpriteDebugSeq"),
                {
                    E_STR_TEXT("SpriteTestLevelName"),
                    Core::Math::Vector3(100.0f, 150.0f + 20.0f, 0.0f),
                    Core::Math::Rect2(0, 0, 300, 50)
                });
            s_seqMap.Add(
                E_STR_TEXT("C_BossDebugSeq"),
                {
                    E_STR_TEXT("BossTestLevelName"),
                    Core::Math::Vector3(100.0f, 200.0f + 30.0f, 0.0f),
                    Core::Math::Rect2(0, 0, 300, 50)
                });
        }

        // TODO: デバッグUI画面を作成
        Core::Common::Handle hUIWidget = UI::UIModule::I().NewLayer(E_STR_TEXT("DebugLayer"), 0);

        // TODO: ボタンとテキストを追加
        Sint32 sortNo = 0;
        for (ST_SEQ_MAP::Iterator it = s_seqMap.Begin(); it != s_seqMap.End(); ++it)
        {
            Core::Common::Handle hBtnAndTextWidget =
                UI::UIModule::I().NewButtonAndText(
                    it->_key.Str(),
                    ++sortNo,
                    HOBBY_LOCALIZATION_MODULE.Text(E_STR_TEXT("JP"), E_STR_TEXT("debug"), it->_data.pName),
                    //it->_data.pName,
                    it->_data.rect, Render::WhiteColor.c,
                    it->_data.rect.Pos(),
                    Render::BlackColor.c);

            auto pWidget = HOBBY_CURRENT_LEVEL.GetActor(hBtnAndTextWidget);

            // ボタンWidget座標設定
            {
                Core::Math::Vector2 pos(it->_data.pos.x, it->_data.pos.y);
                pWidget->SetPos(Core::Math::Vector3(pos.x, pos.y, 0.0f));
            }

            // TODO: ボタンを押した後のイベント
            {
                UI::UIButtonComponent* pBtnComp = reinterpret_cast<UI::UIButtonComponent*>(pWidget->GetComponent(UI::UIButtonComponent::CLASS_RTTI));
                E_ASSERT(pBtnComp != NULL);
                auto handler =
                    std::make_unique<UI::UIButtonMessageHandlerDefault>(it->_data.pName, [this](Core::Common::FixString128 in_msg)
                    {
                        // TODO: ボタンを押した後のイベント
                        this->_MoveLevel(in_msg.Str());
                    });
                pBtnComp->SetPushHandler(std::move(handler));
            }

            // 親Widgetに登録
            const Bool bRet = UI::UIModule::I().AddChildWidget(hUIWidget, hBtnAndTextWidget);
            E_ASSERT(bRet);
        }

        return TRUE;
    }

    const Bool LevelLauncher::End()
    {
        const Bool bRet = Level::Node::End();
        E_ASSERT(bRet);

        return TRUE;
    }

    void LevelLauncher::Update(const Float32 in_dt, const Core::TaskData* in_pData)
    {
        Level::Node::Update(in_dt, in_pData);
    }

    void LevelLauncher::_MoveLevel(const Char* in_pStateName)
    {
        auto it = s_seqMap.Find(in_pStateName);
        if (it == s_seqMap.End())
            return;
    }
}

#endif


