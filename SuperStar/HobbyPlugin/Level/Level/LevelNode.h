#pragma once

#include "Actor/Actor.h"
#include "Actor/ActorInterface.h"
#include "Actor/ActorManager.h"
#include "EnhancedInputModule.h"
// エンジン最小インクルード
#include "Engine/MiniEngine.h"

namespace Level
{
    /// <summary>
    /// レベルのノード
    /// アクターなどのオブジェクトを配置
    /// </summary>
    class Node : public Actor::Object
    {
        HE_CLASS_COPY_NG(Node);
        HE_CLASS_MOVE_NG(Node);

    public:
        enum ETaskUpdateId
        {
            // 入力更新
            ETaskUpdateId_Input = Task::uNoneId + 1,
            // Actor更新
            ETaskUpdateId_Actor,
        };

        Node();  // : Actor::Object(), _actorManager(&this->_actorManagerDecorater) {}
        virtual ~Node() = default;

        /// <summary>
        /// タスク開始
        /// </summary>
        Bool VBegin() override;

        /// <summary>
        /// タスク終了
        /// </summary>
        Bool VEnd() override;

        /// <summary>
        /// 更新
        /// </summary>
        void VUpdate(const Float32 in_fDt) override;

        /// <summary>
        /// イベント
        /// </summary>
        void VEvent(const Core::TaskData&) override final;

        /// <summary>
        /// レベルにアクターを追加
        /// </summary>
        template <class T>
        Core::Common::Handle AddActor()
        {
            static_assert(std::is_base_of<Actor::Object, T>::value,
                          "TクラスはアクターのObjectクラスを継承していない");

            Core::Common::Handle handle = this->_actorManager.Add<T>();
            HE_ASSERT(handle.Null() == FALSE);

            return handle;
        }

        // レベルに追加されたアクターを削除
        void RemoveActor(Core::Common::Handle*);

        /// <summary>
        /// レベルを追加
        /// </summary>
        template <class T>
        const Core::Common::Handle AddLevel()
        {
            static_assert(std::is_base_of<Node, T>::value,
                          "Tクラスはレベルのノードクラスを継承していない");

            // ノード管理にレベルを追加
            auto pNodeManager = this->_pOwner;
            HE_ASSERT(pNodeManager);
            if (pNodeManager == NULL) return NullHandle;

            Core::Common::Handle handle = pNodeManager->Add<T>();
            if (handle.Null()) return NullHandle;

            return handle;
        }

        /// <summary>
        /// レベルのアクターを取得
        /// </summary>
        template <class T>
        T* GetActor(const Core::Common::Handle& in_rHandle)
        {
            static_assert(std::is_base_of<Actor::Object, T>::value,
                          "Tクラスはアクターのクラスを継承していない");

            HE_ASSERT(in_rHandle.Null() == FALSE);
            return reinterpret_cast<T*>(this->_actorManager.Get(in_rHandle));
        }

        /// <summary>
        /// アクター同士の関連付け設定
        /// </summary>
        Bool ChainActor(const Core::Common::Handle& in_rActor,
                        const Core::Common::Handle& in_rParentActor);

    protected:
        virtual void _VProcessInput(const EnhancedInput::InputMap* in_pInputMap);

        /// <summary>
        /// 追加したコンポーネントのセットアップ
        /// </summary>
        Bool _VSetupComponent(const Core::Common::Handle&) final override;

    private:
        /// <summary>
        /// レベルに紐づけるアクター管理
        /// </summary>
        Actor::ActorManager _actorManager;
        // ActorMaanagerDecorater _actorManagerDecorater;
    };
}  // namespace Level
