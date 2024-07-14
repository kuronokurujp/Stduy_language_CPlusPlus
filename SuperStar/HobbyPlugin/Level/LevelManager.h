#pragma once

#include <memory>

#include "Core/Common/FixArray.h"
#include "Core/Task/Task.h"
#include "Core/Task/TaskManager.h"
#include "HobbyPlugin/Actor/ActorManager.h"
#include "MiniEngine.h"

// 前方宣言
namespace Platform
{
    class InputSystemInterface;
}

/// <summary>
/// レベルの管理
/// ゲーム中のアクターなどのオブジェクトを配置するレベルを管理
/// </summary>
namespace Level
{
    /// <summary>
    /// レベルのノード
    /// アクターなどのオブジェクトを配置
    /// </summary>
    class Node : public Actor::Object
    {
        HE_CLASS_COPY_CONSTRUCT_NG(Node);

    public:
        enum ETaskUpdateId
        {
            // 入力更新
            ETaskUpdateId_Input = Task::uNoneId + 1,
            // Actor更新
            ETaskUpdateId_Actor,
        };

        Node() : Actor::Object() {}

        /// <summary>
        /// タスク利用した設定をした最初に実行
        /// 登録に必要な情報を設定
        /// </summary>
        /// <param name="bAutoDelete">TRUEだとタスク破棄と同時にタ
        virtual void Setup(const Bool in_bReleaseMem) override;

        /// <summary>
        /// タスク開始
        /// 継承先は必ず基底クラスのメソッドを最初に呼び出す
        /// 呼ばないとエラーになるので注意
        /// </summary>
        virtual const Bool Begin() override;

        /// <summary>
        /// タスク終了
        /// 継承先は必ず基底クラスのメソッドを最初に呼び出す
        /// 呼ばないとエラーになるので注意
        /// </summary>
        virtual const Bool End() override;

        /// <summary>
        /// 更新用で継承先が実装しないとだめ
        /// 継承先は必ず基底クラスのメソッドを最初に呼び出す
        /// 呼ばないとエラーになるので注意
        /// </summary>
        virtual void Update(const Float32 in_fDt, const Core::TaskData&) override;

        /// <summary>
        /// レベルにアクターを追加
        /// </summary>
        template <class T>
        Core::Common::Handle AddActor()
        {
            static_assert(std::is_base_of<Actor::Object, T>::value,
                          "TクラスはアクターのObjectクラスを継承していない");

            Core::Common::Handle handle = this->_pActorManager->Add<T>();
            HE_ASSERT(handle.Null() == FALSE);

            return handle;
        }

        // TODO: レベルに追加されたアクターを削除
        void RemoveActor(const Core::Common::Handle&);

        /// <summary>
        /// レベルのアクターを取得
        /// </summary>
        /// <param name="in_hActor"></param>
        /// <returns></returns>
        Actor::Object* GetActor(const Core::Common::Handle&);

        /// <summary>
        /// アクターに親アクターを追加
        /// </summary>
        const Bool AddParentActor(const Core::Common::Handle& in_rActor,
                                  const Core::Common::Handle& in_rParentActor);

    private:
        /// <summary>
        /// レベルに紐づけるアクター管理
        /// </summary>
        std::shared_ptr<Actor::ActorManager> _pActorManager;

        /// <summary>
        /// UIオブジェクトのコリジョン配列
        /// </summary>
        Core::Common::FastFixArray<Core::Common::Handle, 256> _aCollision;
    };

    /// <summary>
    /// レベルを管理するクラス
    /// 個々のレベルはタスク形式で管理する
    /// </summary>
    class Manager
    {
        HE_CLASS_COPY_CONSTRUCT_NG(Manager);

    public:
        Manager() {}

        /// <summary>
        /// 必ず最初に呼び出す
        /// </summary>
        /// <returns></returns>
        const Bool Init();

        /// <summary>
        /// 使い終わったら実行
        /// そのあとに利用したらエラーになる
        /// </summary>
        /// <returns></returns>
        const Bool End();

        /// <summary>
        /// 更新
        /// </summary>
        void Update(const Float32 in_fDt, Platform::InputSystemInterface* in_pInput);

        /// <summary>
        /// 起動するレベルを設定
        /// </summary>
        /// <param name="in_pNode"></param>
        template <class T>
        const Bool StartLevel()
        {
            static_assert(std::is_base_of<Node, T>::value,
                          "Tクラスはレベルのノードクラスを継承していない");

            // レベルのノードは使いまわさない
            Core::Common::Handle handle = this->_nodeManager.Add<T>();
            if (handle.Null()) return FALSE;

            this->_currentLevel = handle;

            return TRUE;
        }

        /// <summary>
        /// カレントレベルを取得
        /// </summary>
        /// <returns></returns>
        Level::Node* CurrentLevel()
        {
            Level::Node* pNode =
                reinterpret_cast<Level::Node*>(this->_nodeManager.Get(this->_currentLevel));
            return pNode;
        }

        // TODO: レベルを切り替える

    private:
        // レベルのノードをアクターとして管理
        Actor::ActorManager _nodeManager;

        /// <summary>
        /// カレントレベルのハンドル
        /// </summary>
        Core::Common::Handle _currentLevel;
    };

}  // namespace Level
