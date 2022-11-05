#pragma once

#include <memory>
#include <string>

#include "DirectX12/DirectX12Math.h"

namespace DirectX12
{
    class Mesh;
    struct Context;
}

namespace Geometry
{
    /// <summary>
    /// ペラポリゴン
    /// </summary>
    class PeraPoly
    {
    public:
        PeraPoly(
            const std::string_view& in_r_vs_filepath,
            const std::string_view& in_r_ps_filepath);

        bool Create(std::shared_ptr<DirectX12::Context> in_context);

        void Render(std::shared_ptr<DirectX12::Context> in_context);

    private:
        struct Vertex
        {
            DirectX::XMFLOAT3 pos;
            DirectX::XMFLOAT2 uv;
        };

        std::shared_ptr<DirectX12::Mesh> _mesh;

        std::string _vs_filepath;
        std::string _ps_filepath;

        std::string _gps_key;
        std::string _sig_key;

        static Vertex s_pv[4];
    };
}
