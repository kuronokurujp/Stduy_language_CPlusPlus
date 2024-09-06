#pragma once

#include "Component.h"
#include "Engine/Math/Math.h"

// エンジン最小インクルード
#include "Engine/MiniEngine.h"

namespace Actor
{
    /// <summary>
    /// 座標や回転,スケールなどを扱うコンポーネント
    /// </summary>
    class TransformComponent final : public Component
    {
        HE_CLASS_COPY_NG(TransformComponent);
        HE_GENERATED_CLASS_BODY_HEADER(TransformComponent, Component);

    public:
        TransformComponent() : Component() { this->_Clear(); }

        /// <summary>
        /// 親の位置を含めたワールド座標取得
        /// </summary>
        const Core::Math::Vector3 GetWorldPos();

        /// <summary>
        /// Sets the position.
        /// </summary>
        inline void SetPos(const Core::Math::Vector3& in_rPosition) { this->_pos = in_rPosition; }

        // 座標変換一覧
        void TransformLocalToWorldPos2D(Core::Math::Vector2* out,
                                        const Core::Math::Vector2& in_rOffsetPos);
        void TransformLocalToWorldRect2D(Core::Math::Rect2* out,
                                         const Core::Math::Rect2& in_rOffsetRect);

        /// <summary>
        /// 座標や回転,スケールを加えた最新のワールド行列を作る
        /// 最新のワールド行列がほしい場合は呼び出す必要がある
        /// </summary>
        const Core::Math::Matrix4& ComputeWorldTransform();

    private:
        void _Clear()
        {
            this->_pos.Clear();
            this->_scale.Clear();
        }

    private:
        Core::Math::Vector3 _pos;
        Core::Math::Vector3 _scale;
        Core::Math::Quaternion _rotation;

        Core::Math::Matrix4 _worldTransform;
    };
}  // namespace Actor
