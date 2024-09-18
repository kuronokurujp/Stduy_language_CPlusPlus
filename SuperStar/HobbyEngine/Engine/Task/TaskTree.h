#pragma once

#include <functional>

#include "Task.h"

// エンジン最小インクルード
#include "Engine/MiniEngine.h"

namespace Core
{
    /// <summary>
    /// 子タスクを持たせるタスクツリーのタスク
    /// </summary>
    class TaskTree : public Task
    {
        HE_CLASS_COPY_NG(TaskTree);
        HE_CLASS_MOVE_NG(TaskTree);

        HE_GENERATED_CLASS_BODY_HEADER(TaskTree, Task);

    public:
        // 子タスクのリストノード
        class ChildTaskNode final : public Core::Common::LinkedListNode<ChildTaskNode>
        {
        public:
            ChildTaskNode() { this->Clear(); }
            ChildTaskNode(const Core::Common::Handle& in_rParentHandle,
                          const Core::Common::Handle& in_rHandle)
                : _parentHandle(in_rParentHandle), _handle(in_rHandle)
            {
            }

            inline const Core::Common::Handle& GetParentHandle() const
            {
                return this->_parentHandle;
            }
            inline Core::Common::Handle& GetHandle() { return this->_handle; }

            void Clear()
            {
                this->_handle.Clear();
                this->_parentHandle.Clear();
            }

            inline Bool Empty() const
            {
                return (this->_handle.Null()) && (this->_parentHandle.Null());
            }

        private:
            Core::Common::Handle _parentHandle;
            Core::Common::Handle _handle;
        };

        using ChildTaskNodeIterator = Core::Common::CustomList<TaskTree::ChildTaskNode>::Iterator;

    public:
        TaskTree() : Task() { this->_Clear(); }

        virtual void VSetup(const Bool in_bAutoDelete);

        virtual void VUpdate(const Float32) override;
        virtual void VEvent(const TaskData& in_rEvent) override;

        inline const ChildTaskNode GetChildTaskNode() const { return this->_chainNode; }
        inline Bool IsChild() const { return this->_bChild; }

        /// <summary>
        /// 子タスクの追加
        /// </summary>
        Bool AddChildTask(const Core::Common::Handle&);

        /// <summary>
        /// 子タスクを外す
        /// </summary>
        const ChildTaskNodeIterator RemoveChildTask(Core::Common::Handle*);

    protected:
        /// <summary>
        /// タスク破棄
        /// </summary>
        virtual void _VDestory() override;

    private:
        TaskTree* _GetTaskTree(const Core::Common::Handle& in_rHandle);

        /// <summary>
        /// 子タスクのループ
        /// </summary>
        void _EachbyChildTask(std::function<void(Task*)>);

        void _Clear()
        {
            this->_lstChildTask.Clear();
            this->_chainNode.Clear();
            this->_bChild = FALSE;
        }

    protected:
        Core::Common::CustomList<ChildTaskNode> _lstChildTask;
        ChildTaskNode _chainNode;

    private:
        Bool _bChild = FALSE;
    };
}  // namespace Core
