#pragma once

#include "Actor/Component/Component.h"
#include "Engine/Common/CustomVector.h"
#include "Engine/MiniEngine.h"

namespace InGame
{
    class InGameCircleCollision2DComponent;

    class CollisionInterface
    {
    public:
        virtual ~CollisionInterface() = default;
        // 2D円のコリジョン
        virtual const Bool VCollision(InGameCircleCollision2DComponent&) = 0;
    };

    /// <summary>
    /// インゲームのアクター衝突コンポーネント
    /// </summary>
    class InGameCollisionComponent : public Actor::Component, CollisionInterface
    {
        HE_CLASS_COPY_NG(InGameCollisionComponent);
        HE_CLASS_MOVE_NG(InGameCollisionComponent);
        HE_GENERATED_CLASS_BODY_HEADER(InGameCollisionComponent, Actor::Component);

    public:
        InGameCollisionComponent();
        virtual ~InGameCollisionComponent() = default;

        /// <summary>
        /// タスク開始
        /// </summary>
        virtual const Bool VBegin() override;

        /// <summary>
        /// タスク終了
        /// </summary>
        virtual const Bool VEnd() override;

        // 全ての衝突コンポーネント同士の衝突処理
        static void CollisionAll();

    protected:
        virtual const Bool VCollision(InGameCircleCollision2DComponent&) = 0;

    private:
        static const Bool CollisionWithComponent(CollisionInterface*, InGameCollisionComponent*);

        // コリジョンリスト
        static Core::Common::CustomFixVector<InGameCollisionComponent*, 1024> s_vCollision;
    };

    class InGameCircleCollision2DComponent final : public InGameCollisionComponent
    {
        HE_CLASS_COPY_NG(InGameCircleCollision2DComponent);
        HE_CLASS_MOVE_NG(InGameCircleCollision2DComponent);
        HE_GENERATED_CLASS_BODY_HEADER(InGameCircleCollision2DComponent, InGameCollisionComponent);

    public:
        InGameCircleCollision2DComponent() : InGameCollisionComponent() {}

        // 半径値
        void SetRadius(const Float32 in_fRadius) { this->_fRadius = in_fRadius; }

    protected:
        const Bool VCollision(InGameCircleCollision2DComponent&) override final;

    private:
        Float32 _fRadius = 0.0f;
    };

}  // namespace InGame
