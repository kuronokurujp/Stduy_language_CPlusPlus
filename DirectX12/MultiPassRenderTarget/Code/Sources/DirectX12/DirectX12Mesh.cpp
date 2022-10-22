#include "DirectX12/DirectX12Mesh.h"

namespace DirectX12
{
    Mesh::Mesh(std::string_view in_r_vs_key, std::string_view in_r_idx_key)
    {
        this->_Clear();

        this->_vs_buff_key = in_r_vs_key;
        this->_idx_buff_key = in_r_idx_key;
    }

    const bool Mesh::Create(
        std::shared_ptr<Context> in_r_context,
        std::vector<UINT8>& in_r_vertexs,
        size_t in_vertex_stride_byte_size,
        std::vector<UINT16>& in_r_indices)
    {
        if (this->_ready)
            return false;

        // インデックスバッファを利用しない場合はキー名をクリア
        if (in_r_indices.size() <= 0)
            this->_idx_buff_key.clear();

        // メッシュ作成
        // 頂点バッファビュー作成
        {
            this->_vb_size_in_bytes = static_cast<UINT>(in_r_vertexs.size() * sizeof(in_r_vertexs[0]));
            this->_vb_stride_in_bytes = static_cast<UINT>(in_vertex_stride_byte_size);

            // 頂点情報を書き込むバッファを作成
            auto p_vert_buff = DirectX12::CreateEmptyResourceByGPUTransition(in_r_context, this->_vs_buff_key, _vb_size_in_bytes);
            assert(p_vert_buff != nullptr);

            // 作った頂点バッファに情報をコピーする
            // この書き込み方法は共通化できないので個別で行う
            // コピーデータを一括して書き込む
            {
                UINT8* p_vert_map = nullptr;
                auto result = p_vert_buff->Map(0, nullptr, (void**)&p_vert_map);
                assert(SUCCEEDED(result));

                // 一括データ転送
                std::copy(in_r_vertexs.begin(), in_r_vertexs.end(), p_vert_map);

                p_vert_buff->Unmap(0, nullptr);
            }
        }

        // 頂点のインデックスバッファビューを作成
        if (!this->_idx_buff_key.empty())
        {
            // インデックスバッファを作成
            this->_id_size_in_bytes = static_cast<UINT>(in_r_indices.size() * sizeof(in_r_indices[0]));
            {
                auto p_idx_buff = DirectX12::CreateEmptyResourceByGPUTransition(in_r_context, this->_idx_buff_key, this->_id_size_in_bytes);
                assert(p_idx_buff != nullptr);

                // コピーするデータ先頭と末尾の型と転送する型を指定
                UINT16* p_map = nullptr;
                auto result = p_idx_buff->Map(0, nullptr, (void**)&p_map);
                assert(SUCCEEDED(result));

                std::copy(in_r_indices.begin(), in_r_indices.end(), p_map);

                p_idx_buff->Unmap(0, nullptr);
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

        // 空バッファにインデックスビュー定義
        this->_ib_view = {};
        if (!this->_idx_buff_key.empty())
        {
            // 内容を決めるバッファを取得
            auto buff = in_r_context->res_buff_map[this->_idx_buff_key.c_str()];
            this->_ib_view.BufferLocation = buff->GetGPUVirtualAddress();
            // インデックスの要素データ型を16byteにしている
            this->_ib_view.Format = DXGI_FORMAT_R16_UINT;
            this->_ib_view.SizeInBytes = this->_id_size_in_bytes;
        }

        this->_ready = true;

        return true;
    }

    const bool Mesh::Destory(std::shared_ptr<Context> in_r_context)
    {
        if (!this->_ready)
            return false;

        DirectX12::ReleaseResourceByGPUTransition(in_r_context, this->_vs_buff_key);

        if (!this->_idx_buff_key.empty())
            DirectX12::ReleaseResourceByGPUTransition(in_r_context, this->_idx_buff_key);

        this->_ready = false;

        return true;
    }

    void Mesh::InsertCmdToCmdPipeline(std::shared_ptr<Context> in_r_context, const D3D_PRIMITIVE_TOPOLOGY in_topology)
    {
        if (!this->_ready)
            return;

        // 描画するプリミティブ設定
        in_r_context->cmd_list->IASetPrimitiveTopology(in_topology);

        // 一度に設定できるインデックスバッファビューは一つのみ
        if (!this->_idx_buff_key.empty())
            in_r_context->cmd_list->IASetIndexBuffer(&this->_ib_view);

        // 頂点バッファビューを設定
        in_r_context->cmd_list->IASetVertexBuffers(0, 1, &this->_vb_view);
    }

    void Mesh::_Clear()
    {
        this->_idx_buff_key.clear();
        this->_vs_buff_key.clear();
    }
}
