#include "LevelTitle.h"

// UIモジュール
#include "UIModule.h"

namespace Level
{
    //	変数
    static const char* s_pTitleSeqTitleNameText = "STG";
    static const char* s_pTitleSeqKeyGuideText  = " Space : GAME START a : GAME END";

    /*
            @brief
    */
    LevelTitle::LevelTitle() : Node()
    {
    }

    /*
            @brief
    */
    LevelTitle::~LevelTitle()
    {
    }

    const Bool LevelTitle::Begin()
    {
        const Bool bRet = Level::Node::Begin();
        HE_ASSERT(bRet);

        /*

                // UIイベントをキャッチするコンポーネントを追加
                {
                    auto h    = this->AddComponent<Level::LevelUserInputReceiveComponent>(0);
                    auto comp = this->GetComponent<Level::LevelUserInputReceiveComponent>(h);

                    auto handler = std::make_unique<Level::LevelUserInputMessage>(
                        [this](const Char* in_pMsg)
                        {
                            HE_LOG_LINE(in_pMsg);
                            // TODO: 次のレベルへ遷移
                        });
                    comp->SetReceiver(std::move(handler));
                }

                // UIのBuilderファイルからレイアウト作成
                this->_layoutAssetHandle = ModuleUI()->LoadAssetWithLayoutBuild(
                    Core::File::Path(HE_STR_TEXT("UI"), HE_STR_TEXT("Builder"), HE_STR_TEXT("Game"),
                                     HE_STR_TEXT("Title.xml")));
                // widgetを作成
                // レベルが切り替わると自動的にwidgetは破棄される
                this->_uIWidgetHandle = ModuleUI()->NewLayoutByLayotuAsset(this->_layoutAssetHandle,
           0);
                */

        return bRet;
    }

    const Bool LevelTitle::End()
    {
        // ロードしたアセットを破棄
        ModuleUI()->UnloadAssetWithLayoutBuild(this->_layoutAssetHandle);

        const Bool bRet = Level::Node::End();
        HE_ASSERT(bRet);

        return bRet;
    }

    void LevelTitle::Update(const Float32 in_fDt, const Core::TaskData&)
    {
        /*
            //	パッド制御
            {
                if (r.isPadTriggerOn(Input::PAD_KEY_BTN) || r.isButtonTriggerOn(Input::KEY_SPACE))
                {
                    //	次のステートへ
                    next = new C_PlaySeq();
                }
                else if (r.isButtonTriggerOn('a'))
                {
                    next = NULL;
                }
            }

            //	画面真ん中にタイトル名を表示
            r.drawDebugText(s_pTitleSeqTitleNameText, r.getScreenWidth() >> 1,
                            r.getScreenHeight() >> 1);

            std::string keyGuideText = s_pTitleSeqKeyGuideText;
            r.drawDebugText(s_pTitleSeqKeyGuideText,
                            (r.getScreenWidth() >> 1) - ((keyGuideText.length() / 2) * 8),
                            (r.getScreenHeight() >> 1) + 32);
                            */
    }

}  // namespace Level
