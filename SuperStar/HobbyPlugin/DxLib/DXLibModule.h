#pragma once

#include "Platform/PlatformModule.h"

#include "DXLibTime.h"
#include "DXLibInput.h"

namespace DXLib
{
    class DxLibModule final : public Platform::PlatformModule
    {
    public:
        /// <summary>
        /// TODO: モジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool Init() override final;

        /// <summary>
        /// TODO: モジュール終了
        /// </summary>
        /// <returns></returns>
        const Bool End() override final;

        const bool CreateMainWindow() override final;
        const bool ReleaseAllWindows() override final;

        const bool BeforUpdate(const Float32 in_deltaTime) override final;
        const bool Update(const Float32 in_deltaTime) override final;
        const bool AfterUpdate(const Float32 in_deltaTime) override final;

        // 時間システム
        Platform::TimeSystemInterface* Time() override final {
            return &this->_time;
        }

        // 入力システム
        Platform::InputSystemInterface* Input() override final {
          return &this->_input;
        }

        // TODO: 描画
        void BeginRender(void* in_pCmdBuff) override final;
        void Redner(void* in_pCmdBuff) override final;
        void EndRender(void* in_pCmdBuff) override final;

    private:
        TimeSystem _time;
        InputSystem _input;
    };
}
