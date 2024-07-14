#pragma once

#include <iostream>

#include "Core/Math/Rect2.h"
#include "Core/Task/Task.h"
#include "MiniEngine.h"

namespace Actor
{
    class Object;

    /// <summary>
    /// Actorクラスにくっつけるコンポーネント
    /// 継承前提のクラス
    /// このクラスのみのインスタンスを作れない
    /// </summary>
    class Component : public Core::Task
    {
        HE_CLASS_COPY_CONSTRUCT_NG(Component);

        GENERATED_CLASS_BASE_BODY_HEADER(Component);

    public:
        Component();
        virtual ~Component();

        /// <summary>
        /// タスク利用した設定をした最初に実行
        /// 登録に必要な情報を設定
        /// </summary>
        /// <param name="bAutoDelete">TRUEだとタスク破棄と同時に削除
        virtual void Setup(const Bool in_bReleaseMem) override;

        /// <summary>
        /// コンポーネントのオーターアクターを設定
        /// </summary>
        /// <param name="in_pOwner"></param>
        virtual void SetOwner(Object* in_pOwner) { this->_pOwner = in_pOwner; }

        /// <summary>
        /// タスク開始
        /// </summary>
        virtual const Bool Begin() override { return TRUE; }

        /// <summary>
        /// タスク終了
        /// </summary>
        virtual const Bool End() override { return TRUE; }

        /// <summary>
        /// コンポーネントの更新
        /// 必ず処理を記述
        /// </summary>
        /// <param name="in_deltaTime">The in delta time.</param>
        virtual void Update(const Float32 in_fDeltaTime) {}

        /// <summary>
        /// Gets the update order.
        /// </summary>
        /// <returns></returns>
        const int GetUpdateOrder() const { return this->_iUpdateOrder; }

        // 座標変換一覧
        void TransformLocalToWorldPos2D(Core::Math::Vector2* out,
                                        const Core::Math::Vector2& in_rOffsetPos);
        void TransformLocalToWorldRect2D(Core::Math::Rect2* out,
                                         const Core::Math::Rect2& in_rOffsetRect);

    protected:
        /// <summary>
        /// コンポーネントの更新
        /// 必ず処理を記述
        /// TaskDataは使わないので省略した更新メソッドを継承してもらう
        /// 呼び出してほしくないのでpublicとしない
        /// </summary>
        void Update(const Float32 in_fDeltaTime, const Core::TaskData& in_rData) override final
        {
            this->Update(in_fDeltaTime);
        }

    private:
        void _Clear()
        {
            this->_pOwner       = NULL;
            this->_iUpdateOrder = 0;
        }

    protected:
        Object* _pOwner      = NULL;
        Sint32 _iUpdateOrder = 0;
    };
}  // namespace Actor
