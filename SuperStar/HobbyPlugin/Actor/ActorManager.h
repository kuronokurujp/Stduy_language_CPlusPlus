#pragma once

#include "Actor.h"
#include "ActorInterface.h"
#include "Core/Common/Handle.h"
#include "Core/Task/TaskManager.h"
#include "MiniEngine.h"

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
    class ActorManager final : public ActorManagerlnterface
    {
    public:
        /// <summary>
        /// 初期化
        /// 必ず最初に呼び出す
        /// </summary>
        /// <returns></returns>
        const Bool Init();

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
            Core::Common::Handle handle;
            // Actorが更新中の場合はキャッシュリストに一旦登録
            // Actorは確保したメモリを使いまわさない
            if (this->_bUpdatingActors)
            {
                handle = this->_taskManager.CreateAndAdd<T>(0, TRUE);
            }
            else
            {
                handle = this->_taskManager.CreateAndAdd<T>(1, TRUE);
            }

            Object* pObject = this->_taskManager.GetTask<Object>(handle);
            E_ASSERT(pObject != NULL);
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
        void ProcessInput(const Float32 in_dt, Platform::InputSystemInterface* in_pInput);

        /// <summary>
        /// アクター更新
        /// </summary>
        /// <param name="in_dt"></param>
        void Update(const Float32 in_dt);

        /// <summary>
        /// 保留アクター更新
        /// </summary>
        void UpdatePending();

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

        /*
        void _Clear()
        {
            this->actors.clear();
            this->pendingActors.clear();
        }
        */
    };
}  // namespace Actor
