﻿#pragma once

#include "Actor/Component/Component.h"
#include "Engine/Common/CustomList.h"
#include "Engine/Common/CustomStack.h"
#include "Engine/Math/Math.h"
#include "Engine/MiniEngine.h"
#include "Engine/Task/Task.h"
#include "Engine/Task/TaskManager.h"

// 前方宣言
namespace Platform
{
    class InputInterface;
}

namespace Actor
{
    // 前方宣言
    // class ActorManagerPubliclnterface;
    class ActorManager;

    /// <summary>
    /// ゲームアクター
    /// </summary>
    class Object : public Core::Task
    {
        HE_CLASS_COPY_NG(Object);
        HE_CLASS_MOVE_NG(Object);

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

        Object();
        virtual ~Object();

        /// <summary>
        /// 管理インスタンスを設定
        /// </summary>
        void SetOwner(ActorManager*);

        /// <summary>
        /// ハンドルからオーナーに所属しているアクターを取得
        /// </summary>
        Object* GetActorByOwner(const Core::Common::Handle&);

        /// <summary>
        /// 生成直後の設定処理
        /// データ初期化はここで行う
        /// </summary>
        /// <param name="in_bAutoDelete"></param>
        virtual void VSetup(const Bool in_bAutoDelete) override;

        /// <summary>
        /// 開始
        /// 継承したクラスで必ず基本クラスのメソッドを呼ぶ
        /// </summary>
        virtual const Bool VBegin() override;

        /// <summary>
        /// 終了
        /// 継承したクラスで必ず基本クラスのメソッドを呼ぶ
        /// </summary>
        virtual const Bool VEnd() override;

        /// <summary>
        /// Updates the specified in delta time.
        /// </summary>
        void VUpdate(const Float32 in_fDt, const Core::TaskData&) override;

        /// <summary>
        /// 継承先でUpdateメソッドをoverride出来る.
        /// </summary>
        virtual void VUpdateActor(Float32 in_fDeltaTime) {}

        /// <summary>
        /// コンポーネントを追加
        /// </summary>
        template <class T>
        Core::Common::Handle AddComponent(const Sint32 in_uUpdateOrder)
        {
            static_assert(std::is_base_of<Component, T>::value,
                          "TクラスはComponentクラスを継承していない");

            // TODO: アクターの準備が整う前に呼ばれるケースもある
            // その場合はペンディングリストに追加して準備が整った時にコンポーネントを追加する
            HE_ASSERT(0 <= in_uUpdateOrder);
            HE_ASSERT(in_uUpdateOrder < static_cast<Sint32>(this->_components.GetMaxGroup()));

            // TODO: 更新優先準備による追加処理を指定が必要
            // コンポーネントは確保したメモリを使いまわす
            Core::Common::Handle handle = this->_components.CreateAndAdd<T>(in_uUpdateOrder, FALSE);
            Component* pComp            = this->_components.GetTask<Component>(handle);

            // コンポーネントを付けた自身を設定
            pComp->SetOwner(this);
            this->_pOwner->VOnActorRegistComponent(pComp);

            return handle;
        }

        /// <summary>
        /// くっつている全てのコンポーネント外す
        /// </summary>
        void RemoveAllComponent();

        /// <summary>
        /// Removes the component.
        /// </summary>
        const Bool RemoveComponent(Core::Common::Handle*);

        /// <summary>
        /// 親アクターがあればその親アクターのコンポーネントを取得
        /// </summary>
        template <class T>
        T* GetParentComponent()
        {
            static_assert(std::is_base_of<Component, T>::value,
                          "TクラスはComponentクラスを継承していない");

            // 親アクターがないなら即終了
            if (this->_parentActorHandle.Null()) return NULL;

            // 親アクターに目的のコンポーネント取得
            Object* pParentObj = this->_pDataAccess->Get(this->_parentActorHandle);
            HE_ASSERT(pParentObj != NULL);

            T* pFindComp = pParentObj->GetComponent<T>();
            if (pFindComp != NULL) return pFindComp;

            // なければ親アクターの更に親アクターに目的コンポーネントがあるかチェック
            // 再帰処理をしている
            return pParentObj->GetParentComponent<T>();
        }

        /// <summary>
        /// Gets the state.
        /// </summary>
        const EState GetState() const { return this->_eState; }

        /// <summary>
        /// Sets the state.
        /// </summary>
        void SetState(const EState in_eState) { this->_eState = in_eState; }

        /// <summary>
        /// オブジェクト名を設定 / 取得
        /// </summary>
        void SetName(const Core::Common::StringBase& in_szrName)
        {
            this->_szName = in_szrName.Str();
        }
        const Core::Common::StringBase& GetName() const { return this->_szName; }

        /// <summary>
        /// 取得コンポーネント(ハンドル)
        /// </summary>
        template <class T>
        T* GetComponent(Core::Common::Handle& in_rHandle)
        {
            static_assert(std::is_base_of<Component, T>::value,
                          "TクラスはComponentクラスを継承していない");

            T* p = reinterpret_cast<T*>(this->_components.GetTask(in_rHandle));
            HE_ASSERT(p);

            return p;
        }

        /// <summary>
        /// RTTIから目的のコンポーネントを取得
        /// </summary>
        Core::Common::Handle GetComponentHandle(const Core::Common::RTTI*);

        inline const std::unordered_map<Core::Common::Handle, Core::Task*>* GetComponents() const
        {
            return this->_components.GetUserDataList();
        }

        /// <summary>
        /// 親の位置を含めたワールド座標取得
        /// </summary>
        /// <returns></returns>
        const Core::Math::Vector3 GetWorldPos();

        /// <summary>
        /// Sets the position.
        /// </summary>
        inline void SetPos(const Core::Math::Vector3& in_rPosition) { this->_pos = in_rPosition; }

        inline const Bool ValidComponent(const Core::Common::Handle& in_h)
        {
            if (in_h.Null()) return FALSE;

            return this->_components.Valid(in_h);
        }

        /// <summary>
        /// アクター自身, 子アクターに設定しているコンポーネントを全て出力
        /// アクターの階層構造が深くなると再帰処理が多くなるので注意
        /// </summary>
        void OutputChildrenComponent(Core::Common::StackBase<Component*>* out,
                                     const Core::Common::RTTI*);

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

#endif
    protected:
        /// <summary>
        /// Computes the world transform.
        /// </summary>
        void _ComputeWorldTransform();

    private:
        void _Clear()
        {
            // Actorの状態
            this->_eState = EState_Active;
            this->_pos.Clear();
            this->_scale.Clear();
            this->_bComputeTransform = FALSE;

            this->_pOwner = NULL;
            this->_components.RemoveAll();
        }

    protected:
        // ActorManagerPubliclnterface* _pDataAccess = NULL;
        ActorManager* _pOwner = NULL;

    private:
        // Actorの状態
        EState _eState = EState_Active;

        Core::Math::Vector3 _pos;
        Core::Math::Vector3 _scale;
        Core::Math::Quaternion _rotation;

        Core::Math::Matrix4 _worldTransform;
        Bool _bComputeTransform = FALSE;

        Core::TaskManager _components;

        // オブジェクト名
        Core::Common::FixString128 _szName;
    };
}  // namespace Actor
