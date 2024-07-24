#include "Actor.h"

#include "Actor/ActorManager.h"
#include "Actor/Component/component.h"

namespace Actor
{
    Object::Object() : Task()
    {
        if (this->_components.Init(32, 32) == FALSE)
        {
            HE_ASSERT(FALSE && "コンポーネントのタスク管理の初期化失敗");
        }
    }

    Object::~Object()
    {
        this->_components.End();
        this->_pDataAccess = NULL;
    }

    void Object::Setup(const Bool in_bAutoDelete)
    {
        Task::Setup(in_bAutoDelete);
        this->_Clear();
    }

    const Bool Object::Begin()
    {
        return TRUE;
    }

    const Bool Object::End()
    {
        // 設定しているコンポーネントを全て破棄
        this->RemoveAllComponent();

        // 設定している子アクターを全て外す
        this->_aChildObject.Clear();

        this->_pDataAccess = NULL;

        return TRUE;
    }

    /// <summary>
    /// Updates the specified in delta time.
    /// </summary>
    void Object::Update(const Float32 in_fDt, const Core::TaskData& in_rData)
    {
        if (this->_eState != EState_Active) return;

        switch (in_rData.uId)
        {
            // 入力処理
            case Object::ETaskUpdateId_Input:
            {
                HE_ASSERT(in_rData.pData != NULL);
                {
                    Platform::InputSystemInterface* pInput =
                        reinterpret_cast<Platform::InputSystemInterface*>(in_rData.pData);
                    HE_ASSERT(pInput != NULL);

                    // 入力送信する
                    this->_ProcessInput(in_fDt, pInput);
                }

                break;
            }
            // オブジェクト更新
            case Object::ETaskUpdateId_Object:
            {
                // 座標更新
                this->_ComputeWorldTransform();

                // コンポーネント更新
                this->_components.UpdateAll(in_fDt, in_rData);

                // コンポーネント内で更新した座標を含めて更新
                this->_ComputeWorldTransform();

                // コンポーネントがすべて更新してから実行
                // コンポーネントの結果が同フレーム取れる
                this->UpdateActor(in_fDt);

                // Actor内で更新した座標を含めて更新
                this->_ComputeWorldTransform();

                // 存在しない親アクターがあるかチェックして整理
                if (this->_parentActorHandle.Null() == FALSE)
                {
                    Object* pParentActor = this->_pDataAccess->Get(this->_parentActorHandle);
                    if (pParentActor == NULL) this->_parentActorHandle.Clear();
                }

                break;
            }
        }
    }

    const Bool Object::SetParentActor(const Core::Common::Handle& in_rHandle,
                                      const Uint32 in_uDepth)
    {
        HE_ASSERT(in_rHandle.Null() == FALSE);

        this->_parentActorHandle = in_rHandle;
        this->_uDepth            = in_uDepth + 1;

        return TRUE;
    }

    /// <summary>
    /// くっつている全てのコンポーネント削除.
    /// </summary>
    void Object::RemoveAllComponent()
    {
        this->_components.RemoveAll();
    }

    /// <summary>
    /// Removes the component.
    /// </summary>
    const Bool Object::RemoveComponent(Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT(in_rHandle.Null() == FALSE);
        this->_components.Remove(in_rHandle);
        return TRUE;
    }

    /// <summary>
    /// 子アクターが追加された時に呼ばれるイベント
    /// </summary>
    void Object::OnAddChildActor(Actor::Object* in_pChildActor)
    {
        HE_ASSERT(in_pChildActor != NULL);
        HE_ASSERT(this != in_pChildActor);

        this->_aChildObject.PushBack(in_pChildActor);
    }

    /// <summary>
    /// 子アクター外す時に呼ばれるイベント
    /// </summary>
    void Object::OnRemoveChildActor(Actor::Object* in_pChildActor)
    {
        HE_ASSERT(in_pChildActor != NULL);
        this->_aChildObject.Remove(in_pChildActor);
    }

    /// <summary>
    /// アクター自身, 子アクターに設定しているコンポーネントを全て出力
    /// </summary>
    void Object::OutputChildrenComponent(Core::Common::FastFixArrayBase<Component*>* out,
                                         const Core::Common::RTTI* in_pRtti)
    {
        auto c = this->GetComponentHandle(in_pRtti);
        if (c.Null() == FALSE)
        {
            out->PushBack(this->GetComponent<Component>(c));
        }

        const Uint32 uSize = this->_aChildObject.Size();
        if (uSize <= 0) return;

        Core::Common::FastFixArray<Object*, 32> stack;

        // 子アクターがあれば再帰処理する
        for (Uint32 i = 0; i < uSize; ++i)
        {
            stack.PushBack(this->_aChildObject[i]);
            while (stack.Empty())
            {
                auto obj = stack.PopBack();
                {
                    auto slot = obj->_components.GetUserDataList();
                    for (auto b = slot->begin(); b != slot->end(); ++b)
                    {
                        auto c = reinterpret_cast<Actor::Component*>(b->second);
                        if (c->GetRTTI().DerivesFrom(in_pRtti))
                        {
                            out->PushBack(c);
                        }
                    }
                }

                for (Uint32 j = 0; j < obj->_aChildObject.Size(); ++j)
                {
                    stack.PushBack(obj);
                }
            }
        }
    }

    Core::Common::Handle Object::GetComponentHandle(const Core::Common::RTTI* in_pRtti)
    {
        auto list = this->_components.GetUserDataList();
        auto end  = list->end();
        for (auto itr = list->begin(); itr != end; ++itr)
        {
            Component* target = reinterpret_cast<Component*>(itr->second);
            if (target->GetRTTI().DerivesFrom(in_pRtti)) return itr->first;
        }

        return InvalidHandle;
    }

    /// <summary>
    /// 親の位置を含めたワールド座標取得
    /// </summary>
    /// <returns></returns>
    const Core::Math::Vector3 Object::GetWorldPos()
    {
        Core::Math::Vector3 pos = this->_pos;
        // 親アクターがあれば座標を取得
        Core::Common::Handle handle = this->_parentActorHandle;
        while (handle.Null() == FALSE)
        {
            Object* pParentActor = this->_pDataAccess->Get(handle);
            if (pParentActor == NULL) break;

            pos += pParentActor->GetWorldPos();

            // 親アクターに更に親アクターがあれば処理を続ける
            handle = pParentActor->_parentActorHandle;
        }

        return pos;
    }

#if 0
    /// <summary>
    /// Gets the Renderer.
    /// </summary>
    /// <returns></returns>
    Renderer* Actor::GetRenderer()
    {
        return this->pRenderer;
    }

    /// <summary>
    /// Sets the rotation deg.
    /// 時計周りがマイナス値、反時計周りがプラス値
    /// </summary>
    /// <param name="in_rAngle">The in r angle.</param>
    /// <returns></returns>
    auto Actor::SetRotationAngleUnitDeg(const Math::Vector3 in_rAngle) -> void
    {
        // 角度からクォータニオンを作成
        auto angle = Math::Vector3(in_rAngle);
        angle.Mul(Math::RADIANS_UNIT);

        this->rotation.AngleUnitRadian(angle);

        this->recomputeWorldTransform = true;
    }

    /// <summary>
    /// Gets the forward.
    /// </summary>
    /// <returns></returns>
    auto Actor::GetForward() const -> Math::Vector3
    {
        return Math::Vector3Transform(Math::Vector3::Unit_X, this->rotation);
    }

    /// <summary>
    /// Processes the input.
    /// </summary>
    /// <param name="in_pKeyState">State of the in p key.</param>
    /// <returns></returns>
    auto Actor::ProcessInput(const uint8_t* in_pKeyState) -> void
    {
        if (this->state == EState_Active)
        {
            // コンポーネントの入力プロセスを実行する
            // 継承したコンポーネント側で実装している
            for (auto comp : this->_components)
            {
                comp->ProcessInput(in_pKeyState);
            }

            this->ActorInput(in_pKeyState);
        }
    }

#endif
    /// <summary>
    /// Computes the world transform.
    /// </summary>
    /// <returns></returns>
    void Object::_ComputeWorldTransform()
    {
        if (this->_bComputeTransform == false)
        {
            return;
        }

        this->_bComputeTransform = false;

        // 拡大・拡縮行列作成して初期行列として設定
        this->_worldTransform =
            Core::Math::Matrix4::CreateScale(this->_scale._fX, this->_scale._fY, this->_scale._fZ);

        // 回転行列作成して掛け算
        this->_worldTransform.Mul(Core::Math::Matrix4FormQuaternion(this->_rotation));

        // 平行移動行列作成して掛け算
        this->_worldTransform.Mul(Core::Math::Matrix4::CreateTranslation(this->_pos));
    }
}  // namespace Actor
