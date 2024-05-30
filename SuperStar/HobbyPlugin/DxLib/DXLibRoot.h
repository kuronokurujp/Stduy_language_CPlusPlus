#pragma once

#include "Platform/Module.h"
#include "DXLibTime.h"

namespace DXLib
{
    /// <summary>
    /// DxLibを利用するプラットフォーム
    /// </summary>
    class Root : public Platform::RootInterface
    {
    public:
        const Bool Init() override final;
        void Shutdown() override final;

        const Bool CreateMainWindow() override final;
        const Bool ReleaseAllWindows() override final;

        const Bool BeforUpdate(const Float32 in_deltaTime) override final;
        const Bool Update(const Float32 in_deltaTime) override final;
        const Bool AfterUpdate(const Float32 in_deltaTime) override final;

        void GenerateOutput() override final {}

        Platform::TimeInterface* Time() override final
        {
            return &this->_time;
        }

    private:
        DXLibTime _time;
    };
}
