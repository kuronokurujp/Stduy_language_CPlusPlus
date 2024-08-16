#pragma once

#include "Actor.h"
#include "ActorInterface.h"
#include "Component/InputComponent.h"
#include "Engine/Common/CustomMap.h"
#include "Engine/Common/Handle.h"
#include "Engine/MiniEngine.h"
#include "Engine/Task/TaskManager.h"

// 前方宣言
namespace Platform
{
    class InputSystemInterface;
}

namespace Actor
{
    /// <summary>
    /// アクターの管理
    /// アクター登録 / 削除 / 更新などアクター個々の挙動を制御
    /// </summary>
    class ActorManager : public ActorManagerPubliclnterface
    {
    private:
        /// <summary>
        /// 更新中に作成した保留アクター情報
        /// </summary>
        struct PendingData
        {
            Core::Common::Handle handle;
            Sint32 sMoveGroupId;
        };

        class InputCompontContent : public Core::Common::LinkedListNode<InputCompontContent>
        {
        public:
            InputCompontContent(InputComponent* in_pCompoent) { this->_pComponent = in_pCompoent; }

        private:
            InputComponent* _pComponent = NULL;
        };

    public:
        /// <summary>
        /// 起動する
        /// 必ず最初に呼び出す
        /// グループ最大数は2以上にする
        /// 保留のアクターを管理する専用グループを作るため
        /// </summary>
        /// <returns></returns>
        const Bool Start(const Uint32 in_uActorCapacity, const Uint32 in_uActorGroupMax);

        /// <summary>
        /// 終了
        /// これを呼び出した後は使えない
        /// </summary>
        /// <returns></returns>
        const Bool End();

        /// <summary>
        /// Adds the actor.
        /// </summary>
        template <class T>
        Core::Common::Handle Add()
        {
            static_assert(std::is_base_of<Object, T>::value,
                          "TクラスはアクターのObjectクラスを継承していない");

            Core::Common::Handle handle;
            // Actorが更新中の場合は保留グループIDに一旦登録
            // Actorは確保したメモリを使いまわさない
            if (this->_bUpdatingActors)
            {
                handle = this->_taskManager.CreateAndAdd<T>(this->_GetPendingGroupId(), TRUE);
                Uint32 dataIdx = this->_pendingDataMap.Size();
                this->_pendingDataMap.Add(handle, PendingData{handle, 0});
            }
            else
            {
                handle = this->_taskManager.CreateAndAdd<T>(0, TRUE);
            }

            Object* pObject = this->_taskManager.GetTask<Object>(handle);
            HE_ASSERT(pObject != NULL);
            pObject->SetManagerAccessor(this);

            return handle;
        }

        /// <summary>
        /// Removes the actor.
        /// </summary>
        /// <param name="in_pActor">The in p actor.</param>
        /// <returns></returns>
        // void RemoveActor(Actor *in_pActor);
        void Remove(Core::Common::Handle*) override final;

        /// <summary>
        /// アクター取得
        /// </summary>
        /// <param name=""></param>
        /// <returns></returns>
        Object* Get(const Core::Common::Handle&) override final;

        /// <summary>
        /// Determines whether the specified in p actor is actor.
        /// </summary>
        /// <param name="in_pActor">The in p actor.</param>
        /// <returns></returns>
        const Bool IsActor(const Core::Common::Handle&) override final;

        /// <summary>
        /// 入力コンポーネントの登録・解除
        /// </summary>
        void RegistInputComponent(InputComponent&) override { HE_ASSERT(FALSE); }
        void UnRegistInputComponent(InputComponent&) override { HE_ASSERT(FALSE); }

        /// <summary>
        /// アクター更新
        /// </summary>
        /// <param name="in_dt"></param>
        void Update(const Float32 in_fDt, const Core::TaskData&);

        /// <summary>
        /// 保留アクター更新
        /// </summary>
        void UpdatePending();

        /// <summary>
        /// アクターの階層移動
        /// </summary>
        /// <param name="in_hHandle"></param>
        /// <param name="in_depth"></param>
        void MoveDepth(const Core::Common::Handle& in_rHandle, const Uint32 in_uDepth);

    protected:
        inline const Sint32 _GetPendingGroupId() const
        {
            return this->_taskManager.GetMaxGroup() - 1;
        }

        /// <summary>
        /// 最後のグループが保留グループなので更新グループ数は最大グループ数-1になる
        /// </summary>
        /// <returns></returns>
        inline const Uint32 _GetUpdateGroupMax() const
        {
            return this->_taskManager.GetMaxGroup() - 1;
        }

    protected:
        Bool _bUpdatingActors = FALSE;

    private:
        /// <summary>
        /// アクターを登録する管理
        /// </summary>
        Core::TaskManager _taskManager;

        /// <summary>
        /// 更新保留アクターのデータ
        /// </summary>
        Core::Common::CustomFixMap<Uint64, PendingData, 256> _pendingDataMap;
    };
}  // namespace Actor
