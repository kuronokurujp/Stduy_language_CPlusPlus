#pragma once

#include "ActorInterface.h"
#include "Component/component.h"
#include "Core/Common/FixArray.h"
#include "Core/Common/FixString.h"
#include "Core/Common/Handle.h"
#include "Core/Math/Vector3.h"
#include "Core/Task/Task.h"
#include "Core/Task/TaskManager.h"
#include "MiniEngine.h"

/*
// 配列管理「vector」クラスをインクルード
#include <vector>
// nullptr定義するためのインクルード
#include <stdio.h>
// #include <typeinfo.h>
#include "Math/Quaternion.h"
#include "Math/math.h"
#include "Math/matrix4.h"
#include "Math/vector2.h"
#include "common.h"

// 前方宣言
class Renderer;
class Renderer;
*/

// 前方宣言
namespace Platform
{
    class InputSystemInterface;
}

namespace Actor
{
    // 前方宣言
    // class Component;
    class ActorManagerPubliclnterface;

    /// <summary>
    /// ゲームアクター
    /// </summary>
    class Object : public Core::Task
    {
        E_CLASS_COPY_CONSTRUCT_NG(Object);

    public:
        /// <summary>
        /// Actorの状態.
        /// </summary>
        enum EState
        {
            EState_Active = 0,
            EState_Paused,
            EState_Dead,
        };

        enum ETaskUpdateId
        {
            // 入力更新
            ETaskUpdateId_Input = Task::NONE_ID + 1,
            // オブジェクト更新
            ETaskUpdateId_Object,
        };

        Object();
        virtual ~Object();

        /// <summary>
        /// 管理インスタンスを設定
        /// </summary>
        /// <param name="in_pDataAccess"></param>
        void SetManager(ActorManagerPubliclnterface* in_pDataAccess)
        {
            E_ASSERT(in_pDataAccess);
            this->_pDataAccess = in_pDataAccess;
        }

        /// <summary>
        /// 親アクターを設定
        /// </summary>
        /// <param name="in_handle"></param>
        const Bool SetParentActor(const Core::Common::Handle in_handle, const Uint32 in_depth);

        /// <summary>
        /// 開始
        /// 継承したクラスで必ず基本クラスのメソッドを呼ぶ
        /// </summary>
        virtual const Bool Begin() override;

        /// <summary>
        /// 終了
        /// 継承したクラスで必ず基本クラスのメソッドを呼ぶ
        /// </summary>
        virtual const Bool End() override;

        /// <summary>
        /// Updates the specified in delta time.
        /// </summary>
        void Update(const Float32 in_dt, const Core::TaskData&) override;

        /// <summary>
        /// 継承先でUpdateメソッドをoverride出来る.
        /// </summary>
        /// <param name="in_deltaTime">The in delta time.</param>
        virtual void UpdateActor(float in_deltaTime) {}

        /// <summary>
        /// コンポーネントを追加
        /// </summary>
        /// <returns></returns>
        template <class T>
        Core::Common::Handle AddComponent(const Sint32 in_updateOrder)
        {
            static_assert(std::is_base_of<Component, T>::value,
                          "TクラスはComponentクラスを継承していない");

            // TODO: アクターの準備が整う前に呼ばれるケースもある
            // その場合はペンディングリストに追加して準備が整った時にコンポーネントを追加する
            E_ASSERT(0 <= in_updateOrder);
            E_ASSERT(in_updateOrder < static_cast<Sint32>(this->_components.GetMaxGroup()));

            // TODO: 更新優先準備による追加処理を指定が必要
            // コンポーネントは確保したメモリを使いまわす
            Core::Common::Handle handle = this->_components.CreateAndAdd<T>(in_updateOrder, FALSE);
            Component* pComp            = this->_components.GetTask<Component>(handle);

            // コンポーネントを付けた自身を設定
            pComp->SetOwner(this);

            return handle;
        }

        const Uint32 GetDepth() const { return this->_depth; }

        /// <summary>
        /// くっつている全てのコンポーネント外す
        /// </summary>
        void RemoveAllComponent();

        /// <summary>
        /// Removes the component.
        /// </summary>
        /// <param name="in_pComponent">The in p component.</param>
        /// <returns></returns>
        const Bool RemoveComponent(Core::Common::Handle in_hComponent);

        /// <summary>
        /// 親アクターがあればその親アクターのコンポーネントを取得
        /// </summary>
        template <class T>
        T* GetParentComponent()
        {
            // 親アクターがないなら即終了
            if (this->_parentActorHandle.Null()) return NULL;

            // 親アクターに目的のコンポーネント取得
            Object* pParentObj = this->_pDataAccess->Get(this->_parentActorHandle);
            E_ASSERT(pParentObj != NULL);

            T* pFindComp = pParentObj->GetComponent<T>();
            if (pFindComp != NULL) return pFindComp;

            // なければ親アクターの更に親アクターに目的コンポーネントがあるかチェック
            // 再帰処理をしている
            return pParentObj->GetParentComponent<T>();
        }

        /// <summary>
        /// Gets the state.
        /// </summary>
        /// <returns></returns>
        const EState GetState() const { return this->state; }

        /// <summary>
        /// Sets the state.
        /// </summary>
        /// <param name="in_state">State of the in.</param>
        /// <returns></returns>
        void SetState(const EState in_state) { this->state = in_state; }

        /// <summary>
        /// オブジェクト名を設定 / 取得
        /// </summary>
        void SetName(const Core::Common::FixString128 in_name) { this->_name = in_name; }
        const Core::Common::FixString128& GetName() const { return this->_name; }

        /// <summary>
        /// 取得コンポーネント(ハンドル)
        /// </summary>
        /// <returns></returns>
        template <class T>
        T* GetComponent(Core::Common::Handle& in_hComponent)
        {
            static_assert(std::is_base_of<Component, T>::value,
                          "TクラスはComponentクラスを継承していない");

            T* p = reinterpret_cast<T*>(this->_components.GetTask(in_hComponent));
            E_ASSERT(p);

            return p;
        }

        /*
                /// <summary>
                /// TODO: アクター自身, 子アクターに設定しているコンポーネントを全て出力
                /// アクターの階層構造が深くなると再帰処理が多くなるので注意
                /// </summary>
                template<class T>
                void OutputChildComponents(Core::Common::FastFixArray<T*, 128>& out_comArray)
                {
                    T* c = this->GetComponent<T>();
                    if (c != NULL)
                        out_comArray.PushBack(c);

                    // TODO: 子アクターがあれば再帰処理する
                    for (Uint32 i = 0; i < this->_childObjects.Size(); ++i)
                    {
                        this->_childObjects[i]->OutputChildComponents(out_comArray);
                    }
                }

                /// <summary>
                /// TODO: 型指定したコンポーネントを取得.
                /// コンポーネントは重複設定しない前提.
                /// </summary>
                /// <returns></returns>
                template <class T>
                T* GetComponent()
                {
                    auto& list = this->_components.GetUserDataList();
                    for (auto itr = list.begin(); itr != list.end(); ++itr)
                    {
                        T* target = reinterpret_cast<T*>(*itr);
                        if (target != NULL)
                            return target;
                    }
                    return NULL;
                }
        */

        /// <summary>
        /// TODO: アクター自身, 子アクターに設定しているコンポーネントを全て出力
        /// アクターの階層構造が深くなると再帰処理が多くなるので注意
        /// </summary>
        void OutputChildComponents(Core::Common::FastFixArray<Component*, 128>& out_comArray,
                                   const Core::Common::RTTI& in_rtti);

        /// <summary>
        /// RTTIから目的のコンポーネントを取得
        /// </summary>
        Component* GetComponent(const Core::Common::RTTI& in_rtti);

        /// <summary>
        /// 親の位置を含めたワールド座標取得
        /// </summary>
        /// <returns></returns>
        const Core::Math::Vector3 GetWorldPos();

        /// <summary>
        /// Sets the position.
        /// </summary>
        /// <param name="in_rPosition">The in r position.</param>
        /// <returns></returns>
        inline void SetPos(const Core::Math::Vector3& in_rPosition) { this->pos = in_rPosition; }

        /// <summary>
        /// 子アクターが追加された時に呼ばれるイベント
        /// </summary>
        /// <param name="in_pChildActor"></param>
        virtual void OnAddChildActor(Actor::Object* in_pChildActor);

        /// <summary>
        /// 子アクター外す時に呼ばれるイベント
        /// </summary>
        /// <param name="in_pChildActor"></param>
        virtual void OnRemoveChildActor(Actor::Object* in_pChildActor);

        // TODO: コリジョンイベント

#if 0
        /// <summary>
        /// Gets the scale.
        /// </summary>
        /// <returns></returns>
        inline const Math::Vector3& GetScale() { return this->scale; }

        /// <summary>
        /// Sets the scale.
        /// </summary>
        /// <param name="in_rScale">The in r scale.</param>
        /// <returns></returns>
        inline void SetScale(const Math::Vector3& in_rScale)
        {
            this->scale = in_rScale;
            this->recomputeWorldTransform = true;
        }

        /// <summary>
        /// Sets the scale.
        /// </summary>
        /// <param name="in_scale">The in scale.</param>
        /// <returns></returns>
        inline void SetScale(const float in_scale)
        {
            this->SetScale(Math::Vector3(in_scale, in_scale, in_scale));
        }

        /// <summary>
        /// Sets the rotation deg.
        /// </summary>
        /// <param name="in_rAngle">The in r angle.</param>
        /// <returns></returns>
        void SetRotationAngleUnitDeg(const Math::Vector3 in_rAngle);

        /// <summary>
        /// Sets the rotation.
        /// </summary>
        /// <param name="in_rRot">The in r rot.</param>
        /// <returns></returns>
        inline void SetRotation(const Math::Quaternion& in_rRot)
        {
            this->rotation = in_rRot;
            this->recomputeWorldTransform = true;
        }

        /// <summary>
        /// Gets the rotation.
        /// </summary>
        /// <returns></returns>
        inline const Math::Quaternion& GetRotation() const { return this->rotation; }

        /// <summary>
        /// Gets the forward.
        /// </summary>
        /// <returns></returns>
        Math::Vector3 GetForward() const;

        inline const Math::Matrix4& GetWorldTransform() const { return this->worldTransform; }

        /// <summary>
        /// Processes the input.
        /// </summary>
        /// <param name="in_pKeyState">State of the in p key.</param>
        /// <returns></returns>
        void ProcessInput(const uint8_t* in_pKeyState);

        /// <summary>
        /// Actors the input.
        /// </summary>
        /// <param name="in_pKeyState">State of the in p key.</param>
        /// <returns></returns>
        virtual void ActorInput(const uint8_t* /* in_pKeyState */) {}

        /// <summary>
        /// Computes the world transform.
        /// </summary>
        /// <returns></returns>
        void ComputeWorldTransform();
#endif
    protected:
        virtual void _ProcessInput(const Float32, Platform::InputSystemInterface*) {}

    private:
        // Actorの状態
        EState state = EState_Active;

        Core::Math::Vector3 pos;

        /*
        Math::Vector3 scale;
        Math::Quaternion rotation;

        Math::Matrix4 worldTransform;
        bool recomputeWorldTransform;

        ActorManager* pActorManager;
        Renderer* pRenderer;
        */

        // Actorにつけるコンポーネントリスト
        // ポインターにしているのはポインターからリストの要素を削除するため
        // std::vector<Component*> _components;
        Core::Common::FastFixArray<Actor::Object*, 512> _childObjects;

        Core::TaskManager _components;
        Core::Common::Handle _parentActorHandle;

        ActorManagerPubliclnterface* _pDataAccess = NULL;

        Bool _bStart  = FALSE;
        Uint32 _depth = 0;

        // オブジェクト名
        Core::Common::FixString128 _name;
    };
}  // namespace Actor
