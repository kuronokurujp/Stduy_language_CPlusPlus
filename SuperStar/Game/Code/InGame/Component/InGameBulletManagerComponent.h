#pragma once

#include <functional>

#include "Engine/Common/CustomMap.h"
#include "Engine/MiniEngine.h"
#include "InGame/Bullet/InGameBulletInterface.h"
#include "InGame/Component/InGameCollisionComponent.h"

namespace InGame
{
    /// <summary>
    /// 味方や敵の弾を管理するコンポーネント
    /// </summary>
    class InGameBulletManagerComponent final : public InGame::InGameCollisionComponent
    {
        HE_CLASS_COPY_NG(InGameBulletManagerComponent);
        HE_CLASS_MOVE_NG(InGameBulletManagerComponent);
        HE_GENERATED_CLASS_BODY_HEADER(InGameBulletManagerComponent,
                                       InGame::InGameCollisionComponent);

    private:
        class WorkFixVector final
            : public Core::Common::CustomFixVector<InGame::InGameBulletObject, 256>
        {
        public:
            void RemoveAt(const Uint32 in_uIndex) { this->_RemoveAt(in_uIndex); }
        };

    public:
        explicit InGameBulletManagerComponent() = default;

        /// <summary>
        /// 弾をレンダリングするハンドル
        /// </summary>
        void SetViewHandle(Core::Common::Handle& in_rHandle) { this->_viewHandle = in_rHandle; }

        /// <summary>
        /// 初期化
        /// </summary>
        void VSetup(const Bool in_bReleaseMem) override final;

        /// <summary>
        /// コンポーネント終了
        /// </summary>
        Bool VEnd() override final;

        /// <summary>
        /// コンポーネントの更新
        /// </summary>
        virtual void VUpdate(const Float32 in_fDt) override final;

        /// <summary>
        /// 弾の生成
        /// </summary>
        Bool MakeObject(Core::Memory::UniquePtr<InGameBulletFactoryInterface>);

        /// <summary>
        /// 弾のアルゴリズムを追加
        /// </summary>
        Bool AddStrategy(Core::Memory::UniquePtr<InGameBulletStrategyInterface>);

        // コリジョン処理
        Uint32 VColCount() const override final { return this->_vBullet.Size(); }
        Bool VOutputColData(CollisionData* out, const Uint32 in_uColIndex) override final;

    private:
        void _ForEachObject(std::function<Bool(InGame::InGameBulletObject*,
                                               InGame::InGameBulletStrategyInterface*)>);

    private:
        Core::Common::CustomFixMap<Core::Common::FixString128,
                                   Core::Memory::SharedPtr<InGame::InGameBulletStrategyInterface>,
                                   32>
            _mBulletStrategy;

        WorkFixVector _vBullet;
        Core::Common::Handle _viewHandle;
    };
}  // namespace InGame
