#pragma once

#include "Actor.h"
#include "ActorInterface.h"
#include "Common/CustomMap.h"
#include "Common/Handle.h"
#include "MiniEngine.h"
#include "Task/TaskManager.h"

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
    class ActorManager final : public ActorManagerPubliclnterface
    {
    private:
        struct PendingData
        {
            Core::Common::Handle handle;
            Sint32 moveGroupId;
        };

    public:
        /// <summary>
        /// 起動する
        /// 必ず最初に呼び出す
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
        /// <param name="in_pActor">The in p actor.</param>
        /// <returns></returns>
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
            pObject->SetManager(this);

            return handle;
        }

        /// <summary>
        /// Removes the actor.
        /// </summary>
        /// <param name="in_pActor">The in p actor.</param>
        /// <returns></returns>
        // void RemoveActor(Actor *in_pActor);
        void Remove(const Core::Common::Handle&) override final;

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
        /// アクターに入力状態を送信
        /// </summary>
        void ProcessInput(const Float32 in_fDt, Platform::InputSystemInterface* in_pInput);

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

        /*
        /// <summary>
        /// Gets the actors.
        /// </summary>
        /// <returns></returns>
        const std::vector<Actor*> GetActors();

        /// <summary>
        /// Deletes the objects.
        /// </summary>
        /// <param name="in_rActors">The in r actors.</param>
        /// <param name="pValidate">The p validate.</param>
        /// <returns></returns>
        void DeleteAllActorsAndMemFree(std::vector<Actor*> &in_rActors, bool(*pValidate)(Actor*));

        /// <summary>
        /// Deletes all actors and memory free.
        /// </summary>
        void DeleteAllActorsAndMemFree();
        */

    protected:
        inline const Sint32 _GetPendingGroupId() const
        {
            return this->_taskManager.GetMaxGroup() - 1;
        }
        inline const Uint32 _GetGroupMax() const { return this->_taskManager.GetMaxGroup() - 1; }

    protected:
        /*
        // Actor登録リスト
        std::vector<Actor*> actors;

        // Actor::Update()で新しいActorを登録時のキャッシュ用
        // なぜこうしたか?
        // actorsのループ処理中に登録・解除すると個数が変化してループバグが起こるから
        std::vector<Actor*> pendingActors;
        */

        Bool _bUpdatingActors = FALSE;

    private:
        /// <summary>
        /// アクターを登録する管理
        /// </summary>
        Core::TaskManager _taskManager;

        /// <summary>
        /// 更新保留アクターのデータ
        /// </summary>
        Core::Common::FixMap<Uint64, PendingData, 256> _pendingDataMap;
    };
}  // namespace Actor
