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
        HE_GENERATED_CLASS_BODY_HEADER(Component, Core::Task);

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
        /// </summary>
        virtual void VUpdate(const Float32 in_fDt) override {}

        /// <summary>
        /// コンポーネントのオーターアクターを設定
        /// </summary>
        void SetOwner(Object* in_pOwner) { this->_pOwner = in_pOwner; }

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
