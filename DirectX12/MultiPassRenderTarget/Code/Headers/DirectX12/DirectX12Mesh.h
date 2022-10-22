#pragma once

#include "DirectX12Core.h"
#include <memory>
#include <string>

namespace DirectX12
{
    /// <summary>
    /// DirectX12用のメッシュ作成
    /// TODO: マテリアルと関連付けできるようにするべき
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
        /// メッシュの頂点
        /// インデックスバッファなし版
        /// 頂点型を指定する版
        /// </summary>
        /// <param name="in_r_context"></param>
        /// <param name="in_r_vertexs"></param>
        /// <returns></returns>
        template<typename T>
        const bool Create(std::shared_ptr<Context> in_r_context, std::vector<T>& in_r_vertexs)
        {
            if (this->_ready)
                return false;

            // メッシュ作成
            // 頂点バッファビュー作成
            {
                this->_vb_size_in_bytes = static_cast<UINT>(in_r_vertexs.size() * sizeof(in_r_vertexs[0]));
                this->_vb_stride_in_bytes = sizeof(in_r_vertexs[0]);

                // 頂点情報を書き込むバッファを作成
                auto p_vert_buff = DirectX12::CreateEmptyResourceByGPUTransition(in_r_context, this->_vs_buff_key, this->_vb_size_in_bytes);
                assert(p_vert_buff != nullptr);

                // 作った頂点バッファに情報をコピーする
                // この書き込み方法は共通化できないので個別で行う
                // コピーデータを一括して書き込む
                {
                    T* p_vert_map = nullptr;
                    auto result = p_vert_buff->Map(0, nullptr, (void**)&p_vert_map);
                    assert(SUCCEEDED(result));

                    // 一括データ転送
                    std::copy(in_r_vertexs.begin(), in_r_vertexs.end(), p_vert_map);

                    p_vert_buff->Unmap(0, nullptr);
                }
            }

            // 頂点バッファ構成を出力
            // 毎フレームやっても問題ない？
            // 頂点バッファが変わった時は当然ビューも変わるので更新箇所でやるのがいい気が
            this->_vb_view = {};
            {
                // 内容を決めるバッファを取得
                auto buff = in_r_context->res_buff_map[this->_vs_buff_key.c_str()];
                this->_vb_view.BufferLocation = buff->GetGPUVirtualAddress();
                // バッファ全サイズ
                this->_vb_view.SizeInBytes = this->_vb_size_in_bytes;
                // バッファ内の一塊となるデータサイズ
                this->_vb_view.StrideInBytes = this->_vb_stride_in_bytes;
            }

            this->_ready = true;

            return true;
        }

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
