#include "LevelInGame_BG.h"

#include "Engine/Platform/PlatformModule.h"
#include "RenderModule.h"

namespace Level
{
    const Bool LevelInGame_BG::Begin()
    {
        const Bool bRet = Node::Begin();
        HE_ASSERT(bRet);

        // レンダリングビュー作成
        {
            auto pRenderModule = Module::ModuleManager::I().Get<Render::RenderModule>();
            this->_viewHandle  = pRenderModule->AddView();
        }

        //	点の位置を複数作成
        {
            // スクリーンサイズ取得
            auto pPlatform = Module::ModuleManager::I().Get<Platform::PlatformModule>();
            auto pScreen   = pPlatform->Screen();
            auto pSystem   = pPlatform->System();

            Uint32 uW = pScreen->Width();
            Uint32 uH = pScreen->Height();
            for (Uint32 i = 0; i < this->_aPointPos.Capacity(); ++i)
            {
                const Float32 fX = static_cast<Float32>(pSystem->GetRand(uW));
                const Float32 fY = static_cast<Float32>(pSystem->GetRand(uH));
                this->_aPointPos[i].Set(fX, fY);
            }
        }

        return TRUE;
    }

    const Bool LevelInGame_BG::End()
    {
        auto pRenderModule = Module::ModuleManager::I().Get<Render::RenderModule>();
        if (pRenderModule != NULL) pRenderModule->RemoveView(this->_viewHandle);

        const Bool bRet = Node::End();
        HE_ASSERT(bRet);

        return TRUE;
    }

    void LevelInGame_BG::Update(const Float32 in_fDt, const Core::TaskData& in_rTaskData)
    {
        Node::Update(in_fDt, in_rTaskData);

        // スクリーン情報を取得
        auto pPlatform = Module::ModuleManager::I().Get<Platform::PlatformModule>();
        auto pScreen   = pPlatform->Screen();
        auto pSystem   = pPlatform->System();

        const Uint32 uW = pScreen->Width();
        const Uint32 uH = pScreen->Height();

        const Float32 fMaxXPos = static_cast<Float32>(uW);
        // 左から右へ動かす
        for (Uint32 i = 0; i < this->_aPointPos.Capacity(); ++i)
        {
            this->_aPointPos[i]._fX -= 0.1f;
            if (0 > this->_aPointPos[i]._fX)
            {
                this->_aPointPos[i]._fX = fMaxXPos;
                this->_aPointPos[i]._fY = pSystem->GetRand(uH);
            }
        }

        // TODO: 背景を黒くする
        // inst.setScreenColor(0x000000);

        // 点を描画
        for (Uint32 i = 0; i < this->_aPointPos.Capacity(); ++i)
        {
            Render::CreateCmd2DPointDraw(this->_viewHandle, this->_aPointPos[i],
                                         Render::RGB::White);
        }
    }
}  // namespace Level
