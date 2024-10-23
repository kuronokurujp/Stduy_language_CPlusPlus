#include "InGameBulletManagerComponent.h"

#include "Engine/Math/Rect2.h"

// 弾制御一覧
#include "InGame/Bullet/InGameBulletNormal.h"
// 弾を打つイベント
#include "InGame/Event/InGameEventShot.h"

// 利用モジュール
#include "Engine/Platform/PlatformModule.h"

namespace InGame
{
    void InGameBulletManagerComponent::VSetup(const Bool in_bReleaseMem)
    {
        InGame::InGameCollisionComponent::VSetup(in_bReleaseMem);
        this->_mBulletStrategy.Clear();
    }

    Bool InGameBulletManagerComponent::VBegin()
    {
        // 利用する弾のアルゴリズムを登録
        {
            auto upStrategy = HE_MAKE_CUSTOM_UNIQUE_PTR(InGame::InGameBulletNormalStrategy);
            this->AddStrategy(std::move(upStrategy));
        }

        // 弾を打つイベント管理を追加
        {
            auto pEventModule = HE_ENGINE.ModuleManager().Get<Event::EventModule>();

            auto upStrategy = HE_MAKE_CUSTOM_UNIQUE_PTR(InGame::InGameShotEventManagerStrategy);
            this->_shotEventHandle = pEventModule->AddEventManager(std::move(upStrategy));
            HE_ASSERT(this->_shotEventHandle.Null() == FALSE);

            this->_shotEventListener =
                HE_MAKE_CUSTOM_SHARED_PTR(Event::EventListenerWithRegistEventFunc,
                                          HE_STR_TEXT("LevelInGameShotListener"),
                                          [this](Event::EventDataInterfacePtr const& in_spEventData)
                                          { return this->_HandleEvent(in_spEventData); });

            if (pEventModule->AddListener(this->_shotEventListener, INGAME_SHOT_EVENT_TYPE_NAME) ==
                FALSE)
            {
                HE_ASSERT(0 && "イベントリスナー設定に失敗");
            }
        }

        return InGame::InGameCollisionComponent::VBegin();
    }

    Bool InGameBulletManagerComponent::VEnd()
    {
        auto pEventModule = HE_ENGINE.ModuleManager().Get<Event::EventModule>();

        // 設定したイベントリスナーを解放
        pEventModule->RemoveAllListener(INGAME_SHOT_EVENT_TYPE_NAME);
        // 作成したイベント管理を解放
        pEventModule->RemoveEventManager(this->_shotEventHandle);

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

    /// <summary>
    /// リスナーがイベント受け取ったかどうか
    /// </summary>
    Bool InGameBulletManagerComponent::_HandleEvent(
        Event::EventDataInterfacePtr const& in_spEventData)
    {
        // ゲームのイベント処理を記述

        // 通常弾を打つ
        if (in_spEventData->VDataTypeHash() == EventShotNormalBullet::EventTypeHash())
        {
            // 通常弾の発射処理

            EventShotNormalBullet* pEvent =
                reinterpret_cast<EventShotNormalBullet*>(in_spEventData.get());
            HE_ASSERT(pEvent != NULL);

            auto upFactor = HE_MAKE_CUSTOM_UNIQUE_PTR(InGameBulletNormalFactory, pEvent->_pos,
                                                      pEvent->_dir, pEvent->_uCollisionHashCode);

            this->MakeObject(std::move(upFactor));
        }

        return TRUE;
    }
}  // namespace InGame
