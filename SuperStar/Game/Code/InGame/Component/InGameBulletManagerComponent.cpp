#include "InGameBulletManagerComponent.h"

#include "Engine/Math/Rect2.h"

// 利用モジュール
#include "Engine/Platform/PlatformModule.h"

namespace InGame
{
    void InGameBulletManagerComponent::VSetup(const Bool in_bReleaseMem)
    {
        InGame::InGameCollisionComponent::VSetup(in_bReleaseMem);
        this->_mBulletStrategy.Clear();
    }

    Bool InGameBulletManagerComponent::VEnd()
    {
        this->_mBulletStrategy.Clear();
        return InGame::InGameCollisionComponent::VEnd();
    }

    void InGameBulletManagerComponent::VUpdate(const Float32 in_fDt)
    {
        InGame::InGameCollisionComponent::VUpdate(in_fDt);
        // 弾の生存範囲内のデータを作る
        Core::Math::Rect2 activeScreenInSide;
        {
            auto pPlatform = HE_ENGINE.ModuleManager().Get<Platform::PlatformModule>();
            auto pScreen   = pPlatform->VScreen();

            Float32 fW = static_cast<Float32>(pScreen->VWidth());
            Float32 fH = static_cast<Float32>(pScreen->VHeight());

            const Float32 fSideLength = 100.0f;
            activeScreenInSide.Set(-fSideLength, -fSideLength, 2 * fSideLength + fW,
                                   2 * fSideLength + fH, Core::Math::Rect2::EAnchor_Left);
        }

        this->_ForEachObject(
            [this, activeScreenInSide](InGame::InGameBulletObject* in_pObject,
                                       InGame::InGameBulletStrategyInterface* in_pStrategy)
            {
                // 更新失敗したらワークから外す
                if (in_pStrategy->VUpdate(&in_pObject->work, this->_viewHandle) == FALSE)
                {
                    return FALSE;
                }
                // 画面外かチェックして画面外ならワークから外す
                else if (in_pStrategy->VIsScreenInSide(&in_pObject->work, activeScreenInSide) ==
                         FALSE)
                {
                    return FALSE;
                }

                return TRUE;
            });
    }

    Bool InGameBulletManagerComponent::MakeObject(
        Core::Memory::UniquePtr<InGameBulletFactoryInterface> in_upFactory)
    {
        InGameBulletObject obj;

        HE_STR_CPY_S(obj.aName, HE_ARRAY_NUM(obj.aName), in_upFactory->VName(),
                     HE_STR_LEN(in_upFactory->VName()));

        ::memset(&obj.work, 0, HE_ARRAY_SIZE(obj.work));
        in_upFactory->VConfiguration(&obj.work);

        this->_vBullet.PushBack(obj);

        return TRUE;
    }

    Bool InGameBulletManagerComponent::AddStrategy(
        Core::Memory::UniquePtr<InGameBulletStrategyInterface> in_upStrategy)
    {
        if (this->_mBulletStrategy.Contains(in_upStrategy->VName()))
        {
            HE_ASSERT(0);
            return FALSE;
        }

        auto szName = in_upStrategy->VName();
        this->_mBulletStrategy.Add(szName, std::move(in_upStrategy));

        return TRUE;
    }

    Bool InGameBulletManagerComponent::VOutputColData(CollisionData* out, const Uint32 in_uColIndex)
    {
        auto pBullet = this->_vBullet.GetPtr(in_uColIndex);

        auto itr = this->_mBulletStrategy.FindKey(pBullet->aName);
        if (this->_mBulletStrategy.End() == itr) return FALSE;

        // ストラテジー毎にコリジョンデータを作成
        return itr->data->VOutputCollisionData(out, &pBullet->work);
    }

    void InGameBulletManagerComponent::_ForEachObject(
        std::function<Bool(InGame::InGameBulletObject*, InGame::InGameBulletStrategyInterface*)>
            in_func)
    {
        Sint32 iWorkSize = static_cast<Sint32>(this->_vBullet.Size());
        if (iWorkSize <= 0) return;

        for (Sint32 i = iWorkSize - 1; 0 <= i; --i)
        {
            auto pObject = &this->_vBullet[i];

            if (this->_mBulletStrategy.Contains(pObject->aName) == FALSE) continue;

            auto itr = this->_mBulletStrategy.FindKey(pObject->aName);
            HE_ASSERT(itr->data->VWorkSize() <= sizeof(pObject->work));

            if (in_func(pObject, itr->data.get()) == FALSE)
            {
                this->_vBullet.RemoveAt(i);
            }
        }
    }
}  // namespace InGame
