#include "LevelInGame_BG.h"

#include "Engine/Platform/PlatformModule.h"
#include "RenderModule.h"

namespace Level
{
    const Bool LevelInGame_BG::VBegin()
    {
        const Bool bRet = Node::VBegin();
        HE_ASSERT(bRet);

        // レンダリングビュー作成
        {
            // TODO: 一番奥にビュー追加
            auto pRenderModule = Module::ModuleManager::I().Get<Render::RenderModule>();
            this->_viewHandle  = pRenderModule->AddView();
        }

        //	点の位置を複数作成
        {
            // スクリーンサイズ取得
            auto pPlatform = Module::ModuleManager::I().Get<Platform::PlatformModule>();
            auto pScreen   = pPlatform->VScreen();
            auto pSystem   = pPlatform->VSystem();

            Uint32 uW = pScreen->VWidth();
            Uint32 uH = pScreen->VHeight();
            for (Uint32 i = 0; i < this->_aPointPos.Capacity(); ++i)
            {
                const Float32 fX = static_cast<Float32>(pSystem->VGetRand(uW));
                const Float32 fY = static_cast<Float32>(pSystem->VGetRand(uH));
                this->_aPointPos[i].Set(fX, fY);
            }
        }

        return TRUE;
    }

    const Bool LevelInGame_BG::VEnd()
    {
        auto pRenderModule = Module::ModuleManager::I().Get<Render::RenderModule>();
        if (pRenderModule != NULL) pRenderModule->RemoveView(this->_viewHandle);

        return Node::VEnd();
    }

    void LevelInGame_BG::VUpdate(const Float32 in_fDt)
    {
        Node::VUpdate(in_fDt);

        // スクリーン情報を取得
        auto pPlatform = Module::ModuleManager::I().Get<Platform::PlatformModule>();
        auto pScreen   = pPlatform->VScreen();
        auto pSystem   = pPlatform->VSystem();

        const Uint32 uW = pScreen->VWidth();
        const Uint32 uH = pScreen->VHeight();

        const Float32 fMaxXPos = static_cast<Float32>(uW);
        // 左から右へ動かす
        for (Uint32 i = 0; i < this->_aPointPos.Capacity(); ++i)
        {
            this->_aPointPos[i]._fX -= 0.1f;
            if (0 > this->_aPointPos[i]._fX)
            {
                this->_aPointPos[i]._fX = fMaxXPos;
                this->_aPointPos[i]._fY = pSystem->VGetRand(uH);
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
