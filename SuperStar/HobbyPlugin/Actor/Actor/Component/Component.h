#pragma once

#include "Engine/Math/Rect2.h"
#include "Engine/MiniEngine.h"
#include "Engine/Task/Task.h"

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
        HE_CLASS_COPY_NG(Component);
        HE_CLASS_MOVE_NG(Component);
        GENERATED_CLASS_BASE_BODY_HEADER(Component);

    public:
        Component();
        virtual ~Component() = default;

        /// <summary>
        /// タスク利用した設定をした最初に実行
        /// 登録に必要な情報を設定
        /// </summary>
        /// <param name="bAutoDelete">TRUEだとタスク破棄と同時に削除
        virtual void VSetup(const Bool in_bReleaseMem) override;

        /// <summary>
        /// コンポーネントの更新
        /// 必ず処理を記述
        /// </summary>
        virtual void VUpdate(const Float32 in_fDeltaTime) {}

        /// <summary>
        /// Gets the update order.
        /// </summary>
        const int GetUpdateOrder() const { return this->_iUpdateOrder; }

        /// <summary>
        /// コンポーネントのオーターアクターを設定
        /// </summary>
        void SetOwner(Object* in_pOwner) { this->_pOwner = in_pOwner; }

    protected:
        /// <summary>
        /// コンポーネントの更新
        /// </summary>
        void VUpdate(const Float32 in_fDeltaTime, const Core::TaskData& in_rData) override final;

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
