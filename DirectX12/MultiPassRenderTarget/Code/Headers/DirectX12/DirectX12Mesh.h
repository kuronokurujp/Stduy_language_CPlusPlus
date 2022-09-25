#pragma once

#include "DirectX12Core.h"
#include <memory>
#include <string>

namespace DirectX12
{
    /// <summary>
    /// DirectX12用のメッシュ作成
    /// </summary>
    class Mesh
    {
    public:
        Mesh(std::string_view in_r_vs_key, std::string_view in_r_idx_key);

        /// <summary>
        /// メッシュの頂点とインデックスを作成
        /// </summary>
        /// <param name="in_r_context"></param>
        /// <param name="in_r_vertexs"></param>
        /// <param name="in_vertex_stride_byte_size"></param>
        /// <param name="in_r_indices"></param>
        /// <returns></returns>
        const bool Create(std::shared_ptr<Context> in_r_context, std::vector<UINT8>& in_r_vertexs, size_t in_vertex_stride_byte_size, std::vector<UINT16>& in_r_indices);
        /// <summary>
        /// 作成した頂点とインデックスを消す
        /// </summary>
        /// <param name="in_r_context"></param>
        /// <returns></returns>
        const bool Destory(std::shared_ptr<Context> in_r_context);

        /// <summary>
        /// レンダリングコマンドをコマンドパイプラインに挿入
        /// </summary>
        /// <param name="in_r_context"></param>
        /// <param name="in_topology"></param>
        void InsertCmdToCmdPipeline(std::shared_ptr<Context> in_r_context, const D3D_PRIMITIVE_TOPOLOGY in_topology);

    private:
        void _Clear();

    private:
        std::string _vs_buff_key;
        std::string _idx_buff_key;

        UINT _vb_size_in_bytes = 0;
        UINT _vb_stride_in_bytes = 0;
        UINT _id_size_in_bytes = 0;

        D3D12_VERTEX_BUFFER_VIEW _vb_view = {};
        D3D12_INDEX_BUFFER_VIEW _ib_view = {};

        bool _ready = false;
    };
}
