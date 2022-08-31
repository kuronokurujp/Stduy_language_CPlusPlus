#pragma once

#include "DirectX12Core.h"
#include <memory>
#include <string>

namespace DirectX12
{
    class Material
    {
    public:
        Material(const std::string_view in_buff_key);

        const bool Create();
        const bool Release();

        const bool AddTextureView(const D3D12_SHADER_RESOURCE_VIEW_DESC& in_r_desc);
    };
}
