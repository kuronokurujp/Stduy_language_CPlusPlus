#pragma once

#include "ActorModule.h"
#include "Engine/Common/CustomArray.h"
#include "Engine/MiniEngine.h"
#include "Engine/Task/Task.h"
#include "Engine/Task/TaskManager.h"
#include "EnhancedInputModule.h"

/// <summary>
/// レベルの管理
/// ゲーム中のアクターなどのオブジェクトを配置するレベルを管理
/// </summary>
namespace Level
{
    // 前方宣言
    class Node;

    /// <summary>
    /// レベルを管理するクラス
    /// 個々のレベルはタスク形式で管理する
    /// </summary>
    class Manager
    {
        HE_CLASS_COPY_NG(Manager);
        HE_CLASS_MOVE_NG(Manager);

    public:
        /// <summary>
        /// レベルノードを管理する専用アクタークラス
        /// </summary>
        class NodeManager final : public Actor::ActorManager
        {
        public:
            NodeManager(Manager* in_pManager) : Actor::ActorManager(), _pLevelManager(in_pManager)
            {
            }

            inline Manager* GetLevelManager() const { return this->_pLevelManager; }

        private:
            Manager* _pLevelManager = NULL;
        };

    public:
        Manager() : _nodeManager(this) {}

        /// <summary>
        /// 必ず最初に呼び出す
        /// </summary>
        const Bool Init();

        /// <summary>
        /// 使い終わったら実行
        /// そのあとに利用したらエラーになる
        /// </summary>
        const Bool End();

        /// <summary>
        /// ユーザー入力処理
        /// </summary>
        void ProcessInput(const Float32 in_fDt, const EnhancedInput::InputMap&);

        /// <summary>
        /// 前更新
        /// </summary>
        void BeforeUpdate(const Float32);

        /// <summary>
        /// 更新
        /// </summary>
        void Update(const Float32);

        /// <summary>
        /// 後更新
        /// </summary>
        void LateUpdate(const Float32);

        /// <summary>
        /// 起動するレベルを設定
        /// </summary>
        template <class T>
        const Bool StartLevel()
        {
            static_assert(std::is_base_of<Node, T>::value,
                          "Tクラスはレベルのノードクラスを継承していない");

            // レベルのノードは使いまわさない
            Core::Common::Handle handle = this->_nodeManager.Add<T>();
            if (handle.Null()) return FALSE;

            return this->_StartLevel(handle);
        }

        /// <summary>
        /// レベルを追加
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <returns></returns>
        template <class T>
        const Bool AddLevel()
        {
            static_assert(std::is_base_of<Node, T>::value,
                          "Tクラスはレベルのノードクラスを継承していない");

            // レベルのノードは使いまわさない
            Core::Common::Handle handle = this->_nodeManager.Add<T>();
            if (handle.Null()) return FALSE;

            return TRUE;
        }

        /// <summary>
        /// カレントレベルを取得
        /// </summary>
        /// <returns></returns>
        Node* CurrentLevel();

    private:
        const Bool _StartLevel(const Core::Common::Handle&);

    private:
        // レベルのノードをアクターとして管理
        NodeManager _nodeManager;

        /// <summary>
        /// カレントレベルのハンドル
        /// </summary>
        Core::Common::Handle _currentLevelHandle;

        /// <summary>
        ///  切り替え先のレベルのハンドル
        /// </summary>
        Core::Common::Handle _nextLevelHandle;
    };

    /// <summary>
    /// レベルのノード
    /// アクターなどのオブジェクトを配置
    /// </summary>
    class Node : public Actor::Object
    {
        HE_CLASS_COPY_NG(Node);
        HE_CLASS_MOVE_NG(Node);

    private:
        /// <summary>
        /// レベルノードにつけるアクターの専用管理クラス
        /// </summary>
        class CustomActorManager final : public Actor::ActorManager
        {
        public:
            /// <summary>
            /// 管理下にあるアクターに入力状態を送信
            /// </summary>
            void ProcessInput(const Float32 in_fDt, const EnhancedInput::InputMap*);

            /// <summary>
            /// 入力コンポーネントの登録・解除
            /// </summary>
            void RegistInputComponent(Actor::InputComponent&) override final;
            void UnRegistInputComponent(Actor::InputComponent&) override final;

        private:
            Core::Common::CustomList<Actor::InputComponent> _lstInputComponent;
        };

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
        /// タスク開始
        /// 継承先は必ず基底クラスのメソッドを最初に呼び出す
        /// 呼ばないとエラーになるので注意
        /// </summary>
        const Bool Begin() override;

        /// <summary>
        /// タスク終了
        /// 継承先は必ず基底クラスのメソッドを最初に呼び出す
        /// 呼ばないとエラーになるので注意
        /// </summary>
        const Bool End() override;

        /// <summary>
        /// 更新
        /// 継承先は必ず基底クラスのメソッドを最初に呼び出す
        /// 呼ばないとエラーになるので注意
        /// </summary>
        void Update(const Float32 in_fDt, const Core::TaskData&) override;

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

        // レベルに追加されたアクターを削除
        void RemoveActor(Core::Common::Handle*);

        /// <summary>
        /// レベルのアクターを取得
        /// </summary>
        Actor::Object* GetActor(const Core::Common::Handle&);

        /// <summary>
        /// アクター同士の関連付け設定
        /// </summary>
        const Bool ChainActor(const Core::Common::Handle& in_rActor,
                              const Core::Common::Handle& in_rParentActor);

        /// <summary>
        /// レベルを追加
        /// </summary>
        template <class T>
        const Bool AddLevel()
        {
            static_assert(std::is_base_of<Node, T>::value,
                          "Tクラスはレベルのノードクラスを継承していない");

            // ノード管理にレベルを追加
            auto pNodeManager = reinterpret_cast<Manager::NodeManager*>(this->GetManagerAcceesor());
            HE_ASSERT(pNodeManager);
            if (pNodeManager == NULL) return FALSE;

            auto pLevelManager = pNodeManager->GetLevelManager();
            HE_ASSERT(pLevelManager);
            if (pLevelManager == NULL) return FALSE;

            // ノード追加
            return pLevelManager->AddLevel<T>();
        }

    protected:
        virtual void _ProcessInput(const Float32 in_fDt,
                                   const EnhancedInput::InputMap* in_pInputMap)
        {
        }

    private:
        /// <summary>
        /// レベルに紐づけるアクター管理
        /// </summary>
        std::shared_ptr<CustomActorManager> _pActorManager;
    };
}  // namespace Level
