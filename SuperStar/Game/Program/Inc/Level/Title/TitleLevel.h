#pragma once

#include "MiniEngine.h"

// 外部モジュール
#include "HobbyPlugin/Level/LevelManager.h"

namespace Level
{
    /// <summary>
    /// タイトルレベル
    /// </summary>
    class TitleLevel : public Level::Node
    {
    public:
        TitleLevel();
        ~TitleLevel();

        const Bool Begin() override final;
        const Bool End() override final;

        void Update(const Float32 in_dt, const Core::TaskData* in_pData) override final;
    };
}

