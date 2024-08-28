#include "Actor.h"

#include "Actor/ActorManager.h"
#include "Actor/Component/Component.h"

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
        this->_pOwner = NULL;
    }

    void Object::SetOwner(ActorManager* in_pOwner)
    {
        HE_ASSERT(in_pOwner);
        this->_pOwner = in_pOwner;
    }

    Object* Object::GetActorByOwner(const Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT(this->_pOwner);
        return this->_pOwner->Get(in_rHandle);
    }

    void Object::VSetup(const Bool in_bAutoDelete)
    {
        Task::VSetup(in_bAutoDelete);

        this->_Clear();
    }

    const Bool Object::VBegin()
    {
        if (Task::VBegin() == FALSE) return FALSE;

        return TRUE;
    }

    const Bool Object::VEnd()
    {
        if (Task::VEnd() == FALSE) return FALSE;

        // 設定しているコンポーネントを全て破棄
        this->RemoveAllComponent();

        this->_Clear();

        return TRUE;
    }

    /// <summary>
    /// Updates the specified in delta time.
    /// </summary>
    void Object::VUpdate(const Float32 in_fDt, const Core::TaskData& in_rData)
    {
        if (this->_eState != EState_Active) return;

        // 座標更新
        this->_ComputeWorldTransform();

        // コンポーネント更新
        this->_components.UpdateAll(in_fDt, in_rData);

        // コンポーネント内で更新した座標を含めて更新
        this->_ComputeWorldTransform();

        // コンポーネントがすべて更新してから実行
        // コンポーネントの結果が同フレーム取れる
        this->VUpdateActor(in_fDt);

        // Actor内で更新した座標を含めて更新
        this->_ComputeWorldTransform();
    }

    /// <summary>
    /// くっつている全てのコンポーネント削除.
    /// </summary>
    void Object::RemoveAllComponent()
    {
        // コンポーネント解除をオーナーに通知
        auto components = this->_components.GetUserDataList();
        for (auto itr = components->begin(); itr != components->end(); ++itr)
        {
            auto pComponent = reinterpret_cast<Component*>(itr->second);
            this->_pOwner->VOnActorUnRegistComponent(pComponent);
        }

        this->_components.RemoveAll();
    }

    /// <summary>
    /// Removes the component.
    /// </summary>
    const Bool Object::RemoveComponent(Core::Common::Handle* in_pHandle)
    {
        HE_ASSERT(in_pHandle);
        HE_ASSERT(in_pHandle->Null() == FALSE);

        // コンポーネント解除をオーナーに通知
        auto pComponent = reinterpret_cast<Component*>(this->_components.GetTask(*in_pHandle));
        this->_pOwner->VOnActorUnRegistComponent(pComponent);

        this->_components.RemoveTask(in_pHandle);

        return TRUE;
    }

    /// <summary>
    /// アクター自身, 子アクターに設定しているコンポーネントを全て出力
    /// </summary>
    void Object::OutputChildrenComponent(Core::Common::StackBase<Component*>* out,
                                         const Core::Common::RTTI* in_pRtti)
    {
        auto c = this->GetComponentHandle(in_pRtti);
        if (c.Null() == FALSE)
        {
            out->PushBack(this->GetComponent<Component>(c));
        }

        if (this->_lstChildTask.Empty()) return;
        auto itr    = this->_lstChildTask.BeginItr();
        auto endItr = this->_lstChildTask.EndItr();

        Core::Common::CustomFixStack<Core::Common::CustomList<ChildTaskNode>::Iterator, 32>
            listItrStack;

        while (itr != endItr)
        {
            auto pChildActor = this->_pTaskManager->GetTask<Object>(itr->GetHandle());
            HE_ASSERT(pChildActor);

            auto slot = pChildActor->_components.GetUserDataList();
            for (auto b = slot->begin(); b != slot->end(); ++b)
            {
                auto c = reinterpret_cast<Actor::Component*>(b->second);
                if (c->VGetRTTI().DerivesFrom(in_pRtti))
                {
                    out->PushBack(c);
                }
            }

            ++itr;

            // タスクに子タスクがあれば,処理するタスクリストを子タスクのに変える
            // リスト精査が終わったら,タスクリストを親タスクのリストに戻すので更新前のイテレータをスタックで保存しておく
            if (pChildActor->_lstChildTask.Empty() == FALSE)
            {
                listItrStack.PushBack(itr);
                listItrStack.PushBack(endItr);

                itr    = pChildActor->_lstChildTask.BeginItr();
                endItr = pChildActor->_lstChildTask.EndItr();
            }
            else
            {
                // リストの精査が終わった
                if (itr == endItr)
                {
                    // スタックに保存したイテレータがある場合は
                    // タスクリストを親タスクのリストに戻す
                    if (listItrStack.Empty() == FALSE)
                    {
                        endItr = listItrStack.PopBack();
                        itr    = listItrStack.PopBack();
                    }
                }
            }
        }
    }

    Core::Common::Handle Object::GetComponentHandle(const Core::Common::RTTI* in_pRtti)
    {
        auto list = this->GetComponents();
        auto end  = list->end();
        for (auto itr = list->begin(); itr != end; ++itr)
        {
            Component* target = reinterpret_cast<Component*>(itr->second);
            if (target->VGetRTTI().DerivesFrom(in_pRtti)) return itr->first;
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
        if (this->_bChild)
        {
            Core::Common::Handle handle = this->_chainNode.GetParentHandle();
            while (handle.Null() == FALSE)
            {
                Object* pParentActor = this->GetActorByOwner(handle);
                if (pParentActor == NULL) break;

                pos += pParentActor->GetWorldPos();

                // 親アクターに更に親アクターがあれば処理を続ける
                if (pParentActor->_bChild)
                    handle = pParentActor->_chainNode.GetParentHandle();
                else
                    handle.Clear();
            }
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
