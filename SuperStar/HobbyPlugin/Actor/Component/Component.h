#pragma once

#include "MiniEngine.h"
#include "Core/Task/Task.h"
#include "Core/Math/Rect2.h"

#include <iostream>

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
        E_CLASS_COPY_CONSTRUCT_NG(Component);
        GENERATED_CLASS_BASE_BODY_HEADER();

    public:
        Component();
        virtual ~Component();

        /// <summary>
        /// タスク利用した設定をした最初に実行
        /// 登録に必要な情報を設定
        /// </summary>
        /// <param name="bAutoDelete">TRUEだとタスク破棄と同時に削除
        virtual void Init(const Bool in_bReleaseMem) override;

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
        virtual void Update(const Float32 in_deltaTime) {}

        /// <summary>
        /// コンポーネントの更新
        /// 必ず処理を記述
        /// TaskDataは使わないので省略した更新メソッドを継承してもらう
        /// </summary>
        /// <param name="in_deltaTime">The in delta time.</param>
        void Update(
            const Float32 in_deltaTime,
            const Core::TaskData* in_pData) override final
        {
            this->Update(in_deltaTime);
        }

        /// <summary>
        /// Gets the update order.
        /// </summary>
        /// <returns></returns>
        const int GetUpdateOrder() const { return this->_updateOrder; }

        // 座標変換一覧
        void TransformLocalToWorldPos2D(Core::Math::Vector2* out_pPos, const Core::Math::Vector2& in_offsetPos);
        void TransformLocalToWorldRect2D(Core::Math::Rect2* out_pRect, const Core::Math::Rect2& in_offsetRect);

    private:
        void _Clear()
        {
            this->_pOwner = NULL;
            this->_updateOrder = 0;
        }

    protected:
        Object* _pOwner = NULL;
        Sint32 _updateOrder = 0;
    };
}
