#include "LevelManager.h"

#include "ActorModule.h"
#include "Engine/Memory/Memory.h"
#include "LevelNode.h"

namespace Level
{
    const Bool Manager::Init()
    {
        // タスク管理を初期化
        // 利用するタスク設定は内部で固定する
        if (this->_nodeManager.Start(32, 2) == FALSE) return FALSE;

        return TRUE;
    }

    const Bool Manager::Release()
    {
        if (this->_nodeManager.End() == FALSE) return FALSE;

        return TRUE;
    }

    void Manager::ProcessInput(const Float32 in_fDt, const EnhancedInput::InputMap& in_rInputMap)
    {
        // 入力更新
        {
            EnhancedInput::InputMap& rInputMap = const_cast<EnhancedInput::InputMap&>(in_rInputMap);
            Core::TaskData taskData{Node::ETaskUpdateId_Input, reinterpret_cast<void*>(&rInputMap)};

            this->_nodeManager.Event(taskData);
        }
    }

    void Manager::BeforeUpdate(const Float32 in_fDt)
    {
        // レベルの切り替え先があればカレントと交換
        // 切り替えた後は古いカレントは破棄
        if ((this->_nextLevelHandle.Null() == FALSE))
        {
            auto pNode = reinterpret_cast<Node*>(this->_nodeManager.Get(this->_nextLevelHandle));
            HE_ASSERT(pNode);
            pNode->SetState(Actor::Object::EState::EState_Active);

            if (this->_currentLevelHandle.Null() == FALSE)
                this->_nodeManager.Remove(&this->_currentLevelHandle);

            this->_currentLevelHandle = this->_nextLevelHandle;
            this->_nextLevelHandle.Clear();
        }

        this->_nodeManager.BeginUpdate(in_fDt);
    }

    void Manager::Update(const Float32 in_fDt)
    {
        // アクター更新
        this->_nodeManager.Update(in_fDt);
    }

    void Manager::LateUpdate(const Float32 in_fDt)
    {
        this->_nodeManager.LateUpdate(in_fDt);
    }

    Node* Manager::GetLevel(const Core::Common::Handle& in_rHandle)
    {
        Level::Node* pNode = reinterpret_cast<Level::Node*>(this->_nodeManager.Get(in_rHandle));
        return pNode;
    }

    const Bool Manager::_StartLevel(const Core::Common::Handle& in_rHandle)
    {
        this->_nextLevelHandle = in_rHandle;

        auto pNode = reinterpret_cast<Node*>(this->_nodeManager.Get(this->_nextLevelHandle));
        HE_ASSERT(pNode);
        if (pNode == NULL) return FALSE;

        // 切り替えた後にアクティブにする
        pNode->SetState(Actor::Object::EState::EState_Paused);

        return TRUE;
    }

}  // namespace Level
