#pragma once

#include "MiniEngine.h"

// 外部モジュール
#include "HobbyPlugin/Level/LevelManager.h"

namespace Level
{
    /// <summary>
    /// デバッグメインレベル
    /// </summary>
    class DebugMain : public Level::Node
    {
        E_CLASS_COPY_CONSTRUCT_NG(DebugMain)

    public:
        DebugMain() : Level::Node() {}
        const Bool Begin() override final;
        const Bool End() override final;

        void Update(const Float32 in_dt, const Core::TaskData* in_pData) override final;

    private:
        /// <summary>
        /// TODO: レベル名を指定してレベル移動
        /// </summary>
        void _MoveLevel(const Char* in_pLevelName);
    };
} 

