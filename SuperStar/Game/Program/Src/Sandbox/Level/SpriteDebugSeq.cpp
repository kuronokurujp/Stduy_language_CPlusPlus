/**
 *	@file 	SpriteDebugSeq.cpp
 *	@brief	シーケンス
 *	@author	yuto uchida
 *	@data   2011-02-17
 *	@note
 *
 */
//	インクルード
#include "SpriteDebugSeq.h"

#include <string>

#include "common/Common.h"
#include "common/SelectedList.h"
#include "game/GameSystem.h"
#include "game/graphics/sprite/SpriteAnim.h"
#include "game/utl/File.h"

namespace MainSeq
{
    class Viewr2D
    {
    public:
        //	コンストラクタ
        Viewr2D() { _Clear(); }
        ~Viewr2D() {}

        /*
                @brief	初期化
        */
        void Init()
        {
            m_pFile = new GameLib::C_File();
            ASSERT(m_pFile);

            m_pSpriteSelectedList = new SelectedListInput();
            ASSERT(m_pSpriteSelectedList);

            m_pSpriteSelectedList->Create(1, 4, 0);
            m_pSpriteSelectedList->Add("test");

            _SetNext(_LIST_STATE);
        }

        /*
                @brief	終了
        */
        void Relese()
        {
            if (m_pSpriteSelectedList != NULL)
            {
                m_pSpriteSelectedList->Destroy();
            }

            SAFE_DELETE(m_pFile);
            SAFE_DELETE(m_pSprite);
            SAFE_DELETE(m_pSpriteSelectedList);
        }

        /*
                @brief	更新
        */
        void Update() { (this->*m_pUpdateFuncList[m_State])(); }

        /*
                @brief	描画
        */
        void Draw() { (this->*m_pDrawFuncList[m_State])(); }

    private:
        //	定義
        typedef enum
        {
            _LIST_STATE = 0,
            _VIEW_STATE,
        } _STATE_ENUM;

        //	関数
        void _Clear()
        {
            m_pSpriteSelectedList = NULL;
            m_pSprite             = NULL;
            m_State               = _LIST_STATE;
            m_pFile               = NULL;
        }

        /*
                @brief	次のステート
        */
        void _SetNext(const _STATE_ENUM in_State)
        {
            //	後処理
            (this->*m_pReleaseFuncList[m_State])();

            //	初期処理
            (this->*m_pInitFuncList[in_State])();

            m_State = in_State;
        }

        /*
                @brief	リスト初期化
        */
        void _InitList() {}

        /*
                @brief	リスト終了
        */
        void _ReleaseList() {}

        /*
                @brief	リスト更新
        */
        void _UpdateList()
        {
            GameLib::C_GameSystem& r = GameLib::C_GameSystem::inst();

            //	パッド制御
            if (r.isPadTriggerOn(Input::PAD_KEY_BTN) || r.isButtonTriggerOn('a'))
            {
                //	スプライト描画へ
                _SetNext(_VIEW_STATE);
            }
        }

        /*
                @brief	描画
        */
        void _DrawList() { m_pSpriteSelectedList->DebugDraw(); }

        /*
                @brief	スプライト表示初期化
        */
        void _InitView()
        {
            // m_pSpriteSelectedList->GetCursorByName();
            m_pFile->Load("2D/SuraimuAnim.gpc", ".gpc");
            m_pSprite = new GameLib::SpriteAnim(m_pFile);
        }

        /*
                @brief	スプライト表示終了
        */
        void _ReleaseView()
        {
            m_pFile->UnLoad();
            SAFE_DELETE(m_pSprite);
        }

        /*
                @brief	スプライト表示更新
        */
        void _UpdateView()
        {
            m_pSprite->update();

            GameLib::C_GameSystem& r = GameLib::C_GameSystem::inst();

            //	座標制御
            {
                Vec3& pos = m_pSprite->getPos();
                if (r.isButtonOn(Input::KEY_UP))
                {
                    pos.y += -1.f;
                }
                else if (r.isButtonOn(Input::KEY_DOWN))
                {
                    pos.y += 1.f;
                }

                if (r.isButtonOn(Input::KEY_LEFT))
                {
                    pos.x += -1.f;
                }
                else if (r.isButtonOn(Input::KEY_RIGHT))
                {
                    pos.x += 1.f;
                }
            }

            //	アニメ制御
            {
                int animIdx    = (int)m_pSprite->getIdxAnim();
                int nowAnimIdx = animIdx;

                int animIdxMax = (int)m_pSprite->getMaxAnim();
                if (r.isButtonTriggerOn('z'))
                {
                    --animIdx;
                }

                if (r.isButtonTriggerOn(('x')))
                {
                    ++animIdx;
                }

                animIdx = CLAMP(animIdx, 0, animIdxMax - 1);
                if (animIdx != nowAnimIdx)
                {
                    //	アニメ変更
                    m_pSprite->chgIdxAnim((unsigned)animIdx);
                }

                if (m_pSprite->isPlay() == false)
                {
                    //	アニメ再生
                    if (r.isButtonTriggerOn('s'))
                    {
                        m_pSprite->play(true);
                    }
                }
                else
                {
                    //	アニメ停止
                    if (r.isButtonTriggerOn('s'))
                    {
                        m_pSprite->stop();
                    }
                }
            }

            m_pSprite->update();

            //	パッド制御
            if (r.isButtonTriggerOn('a'))
            {
                //	スプライト選択描画へ
                _SetNext(_LIST_STATE);
            }
        }

        /*
                @brief	スプライト表示
        */
        void _DrawView()
        {
            if (m_pSprite == NULL)
            {
                return;
            }

            m_pSprite->draw();

            //	デバッグ情報
            GameLib::C_GameSystem& r = GameLib::C_GameSystem::inst();
            {
                Vec3& pos = m_pSprite->getPos();

                char pText[256] = {NULL};
                sprintf_s(pText, "XPos[%lf]", pos.x);
                r.drawDebugText(pText, 0, 0);

                sprintf_s(pText, "YPos[%lf]", pos.y);
                r.drawDebugText(pText, 0, 32);

                sprintf_s(pText, "AnimIdx[%d] / AnimIdxMax[%d]", m_pSprite->getIdxAnim(),
                          m_pSprite->getMaxAnim());
                r.drawDebugText(pText, 0, 64);

                sprintf_s(pText, "Frame[%d] / FrameMax[%d]", m_pSprite->getAnimFrame(),
                          m_pSprite->getAnimFrameMax());
                r.drawDebugText(pText, 0, 96);
            }
        }

        //	変数
        SelectedListInput* m_pSpriteSelectedList;
        GameLib::SpriteAnim* m_pSprite;
        GameLib::C_File* m_pFile;

        static void (Viewr2D::*m_pInitFuncList[])();
        static void (Viewr2D::*m_pUpdateFuncList[])();
        static void (Viewr2D::*m_pDrawFuncList[])();
        static void (Viewr2D::*m_pReleaseFuncList[])();

        _STATE_ENUM m_State;
    };

    void (Viewr2D::*Viewr2D::m_pInitFuncList[])() = {
        &Viewr2D::_InitList,
        &Viewr2D::_InitView,
    };

    void (Viewr2D::*Viewr2D::m_pReleaseFuncList[])() = {
        &Viewr2D::_ReleaseList,
        &Viewr2D::_ReleaseView,
    };

    void (Viewr2D::*Viewr2D::m_pUpdateFuncList[])() = {
        &Viewr2D::_UpdateList,
        &Viewr2D::_UpdateView,
    };

    void (Viewr2D::*Viewr2D::m_pDrawFuncList[])() = {
        &Viewr2D::_DrawList,
        &Viewr2D::_DrawView,
    };

    static Viewr2D* s_pView2D = NULL;

    /*
            @brief
    */
    C_SpriteDebugSeq::C_SpriteDebugSeq()
    {
        s_pView2D = new Viewr2D();
        s_pView2D->Init();
    }

    /*
            @brief
    */
    C_SpriteDebugSeq::~C_SpriteDebugSeq()
    {
        s_pView2D->Relese();
        SAFE_DELETE(s_pView2D);
    }

    /*
            @brief	更新
    */
    I_MainSeq* C_SpriteDebugSeq::update(C_Parent* ip_parent)
    {
        I_MainSeq* next = this;

        s_pView2D->Update();
        s_pView2D->Draw();

        return next;
    }

}  // namespace MainSeq
