#include "Component.h"

#include "Actor/Actor.h"
#include "Actor/ActorInterface.h"

namespace Actor
{
    Component::Component() : Task()
    {
        this->_Clear();
    }

    /// <summary>
    /// タスク利用した設定をした最初に実行
    /// 登録に必要な情報を設定
    /// </summary>
    /// <param name="bAutoDelete">TRUEだとタスク破棄と同時に削除
    void Component::VSetup(const Bool in_bAutoDelete)
    {
        HE_LOG_LINE(HE_STR_TEXT("初期化したコンポーネントは(%s)"),
                    this->VGetRTTI().GetName().Str());

        Core::Task::VSetup(in_bAutoDelete);

        this->_Clear();
    }

    void Component::VUpdate(const Float32 in_fDeltaTime, const Core::TaskData& in_rData)
    {
        this->VUpdate(in_fDeltaTime);
    }

}  // namespace Actor
