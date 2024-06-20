#include "Actor.h"

#include "ActorManager.h"
#include "Component/component.h"

/*
#include "SDL/SDL_assert.h"
#include "renderer.h"
*/

namespace Actor
{
    /*
    /// <summary>
    /// Initializes a new instance of the <see cref="Actor"/> class.
    /// </summary>
    Actor::Actor(Renderer* in_pRenderer, ActorManager* in_pActorManager)
    {
        this->_Clear();

        this->pActorManager = in_pActorManager;
        this->pActorManager->AddActorMemData(this);

        this->pRenderer = in_pRenderer;
    }
    */
    Object::Object() : Task()
    {
        if (this->_components.Init(32, 32) == FALSE)
        {
            E_ASSERT(FALSE && "コンポーネントのタスク管理の初期化失敗");
        }
    }

    /// <summary>
    /// Finalizes an instance of the <see cref="Actor"/> class.
    /// </summary>
    Object::~Object()
    {
        this->_components.End();
        this->_pDataAccess = NULL;
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
        this->_childObjects.Empty();

        this->_pDataAccess = NULL;

        return TRUE;
    }

    /// <summary>
    /// Updates the specified in delta time.
    /// </summary>
    void Object::Update(const Float32 in_dt, const Core::TaskData* in_pData)
    {
        if (this->state != EState_Active) return;

        switch (in_pData->id)
        {
            // TODO: 入力処理
            case Object::ETaskUpdateId_Input:
            {
                E_ASSERT(in_pData->pData != NULL);
                {
                    Platform::InputSystemInterface* pInput =
                        reinterpret_cast<Platform::InputSystemInterface*>(in_pData->pData);
                    E_ASSERT(pInput != NULL);

                    // TODO: 入力送信する
                    this->_ProcessInput(in_dt, pInput);
                }

                break;
            }
            // TODO: オブジェクト更新
            case Object::ETaskUpdateId_Object:
            {
                // 座標更新
                //        this->ComputeWorldTransform();

                // コンポーネント更新
                this->_components.UpdateAll(in_dt, in_pData);

                // コンポーネント内で更新した座標を含めて更新
                //        this->ComputeWorldTransform();

                // コンポーネントがすべて更新してから実行
                // コンポーネントの結果が同フレーム取れる
                this->UpdateActor(in_dt);

                // Actor内で更新した座標を含めて更新
                //        this->ComputeWorldTransform();

                // TODO: 存在しない親アクターがあるかチェックして整理
                if (this->_parentActorHandle.Null() == FALSE)
                {
                    Object* pParentActor = this->_pDataAccess->Get(this->_parentActorHandle);
                    if (pParentActor == NULL) this->_parentActorHandle.Clear();
                }

                break;
            }
        }
    }

    /// <summary>
    /// 子アクターを追加
    /// </summary>
    /// <param name="in_handle"></param>
    const Bool Object::SetParentActor(const Core::Common::Handle in_handle)
    {
        E_ASSERT(in_handle.Null() == FALSE);

        this->_parentActorHandle = in_handle;

        return TRUE;
    }

    /// <summary>
    /// くっつている全てのコンポーネント削除.
    /// </summary>
    /// <returns></returns>
    void Object::RemoveAllComponent()
    {
        this->_components.RemoveAll();
        /*
        for (auto component : this->_components)
        {
            SAFETY_MEM_RELEASE(component);
        }

        this->_components.clear();
        */
    }

    /// <summary>
    /// Removes the component.
    /// </summary>
    /// <param name="in_pComponent">The in p component.</param>
    /// <returns></returns>
    const Bool Object::RemoveComponent(Core::Common::Handle in_hComponent)
    {
        E_ASSERT(in_hComponent.Null() == FALSE);
        this->_components.Remove(in_hComponent);
        /*

        // 同じコンポーネントを多重設定出来ない
        auto iter = std::find(
            this->_components.begin(),
            this->_components.end(),
            in_pComponent);
        if (iter == this->_components.end())
        {
            return false;
        }

        this->_components.erase(iter);
        */

        return TRUE;
    }

    /// <summary>
    /// 子アクターが追加された時に呼ばれるイベント
    /// </summary>
    /// <param name="in_pChildActor"></param>
    void Object::OnAddChildActor(Actor::Object* in_pChildActor)
    {
        E_ASSERT(in_pChildActor != NULL);
        E_ASSERT(this != in_pChildActor);

        this->_childObjects.PushBack(in_pChildActor);
    }

    /// <summary>
    /// 子アクター外す時に呼ばれるイベント
    /// </summary>
    /// <param name="in_pChildActor"></param>
    void Object::OnRemoveChildActor(Actor::Object* in_pChildActor)
    {
        E_ASSERT(in_pChildActor != NULL);
        this->_childObjects.Remove(in_pChildActor);
    }

    /// <summary>
    /// TODO: アクター自身, 子アクターに設定しているコンポーネントを全て出力
    /// アクターの階層構造が深くなると再帰処理が多くなるので注意
    /// </summary>
    void Object::OutputChildComponents(Core::Common::FastFixArray<Component*, 128>& out_comArray,
                                       const Core::Common::RTTI& in_rtti)
    {
        Component* c = this->GetComponent(in_rtti);
        if (c != NULL) out_comArray.PushBack(c);

        // TODO: 子アクターがあれば再帰処理する
        for (Uint32 i = 0; i < this->_childObjects.Size(); ++i)
        {
            this->_childObjects[i]->OutputChildComponents(out_comArray, in_rtti);
        }
    }

    Component* Object::GetComponent(const Core::Common::RTTI& in_rtti)
    {
        auto& list = this->_components.GetUserDataList();
        for (auto itr = list.begin(); itr != list.end(); ++itr)
        {
            Component* target = reinterpret_cast<Component*>(itr->_pItem);
            if (target->GetRTTI().DerivesFrom(in_rtti)) return target;
        }

        return NULL;
    }

    /// <summary>
    /// 親の位置を含めたワールド座標取得
    /// </summary>
    /// <returns></returns>
    const Core::Math::Vector3 Object::GetWorldPos()
    {
        Core::Math::Vector3 pos = this->pos;
        // TODO: 親アクターがあれば座標を取得
        Core::Common::Handle handle = this->_parentActorHandle;
        while (handle.Null() == FALSE)
        {
            Object* pParentActor = this->_pDataAccess->Get(handle);
            if (pParentActor == NULL) break;

            pos += pParentActor->GetWorldPos();

            // TODO: 親アクターに更に親アクターがあれば処理を続ける
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

    /// <summary>
    /// Computes the world transform.
    /// </summary>
    /// <returns></returns>
    auto Actor::ComputeWorldTransform() -> void
    {
        if (this->recomputeWorldTransform == false)
        {
            return;
        }

        this->recomputeWorldTransform = false;

        // 拡大・拡縮行列作成して初期行列として設定
        this->worldTransform = Math::Matrix4::CreateScale(
            this->scale.x,
            this->scale.y,
            this->scale.z);

        // 回転行列作成して掛け算
        this->worldTransform.Mul(Math::Matrix4FormQuaternion(this->rotation));

        // 平行移動行列作成して掛け算
        this->worldTransform.Mul(Math::Matrix4::CreateTranslation(this->position));
    }
#endif
}  // namespace Actor
