#pragma once

#include "DirectX12/DirectX12MiniHeader.h"

#include "GUI/Interface/WindowModelInterface.h"

namespace GUI
{
    /// <summary>
    /// DirectX12用のウィンドウモデル
    /// </summary>
    class DirectX12WindowModel : public WindowModelInterface
    {
    public:
        // 画面クリア色データ構造
        // TODO: もしかしたら探せば既に定義されているかもしれないので後で差し替えるかも
        typedef union
        {
            float _color[4];
            struct
            {
                float r, g, b, a;
            } rgba;
        } ClearColorRGB;

        /// <summary>
        /// 作成したDirectX12のコンテキストを渡して初期化
        /// </summary>
        /// <param name="in_context"></param>
        /// <param name="in_width"></param>
        /// <param name="in_height"></param>
        DirectX12WindowModel(
            std::shared_ptr<DirectX12::Context> in_context,
            const UINT32 in_width,
            const UINT32 in_height);

        virtual ~DirectX12WindowModel();

        // 縦と横のウィンドウサイズ
        inline UINT32 Width() override final { return static_cast<UINT32>(this->_viewport.Width); }
        inline UINT32 Height() override final { return static_cast<UINT32>(this->_viewport.Height); }

        // ウィンドウサイズのアスペクト比
        // 横を基準にする
        inline float AspectRatio() override final { return (this->_viewport.Width / this->_viewport.Height); }

        // 更新可能かどうか
        const bool IsUpdate() override final { return this->_update_flag; }
        // 更新するかどうかのフラグ
        void EnableUpdate(const bool in_flag) override final;

        // DirectX12のコンテキスト取得
        std::shared_ptr<DirectX12::Context> Context() { return this->_context; }

        // 画面のビューポート領域とシザー領域の取得
        const CD3DX12_VIEWPORT& GetViewPort() const { return this->_viewport; }
        const D3D12_RECT& GetScissorRect() const { return this->_scissor_rect; }

        // 画面全体をクリアする色
        const ClearColorRGB& GetClearClear() const { return this->_clear_color; }

    private:
        std::shared_ptr<DirectX12::Context> _context;

        CD3DX12_VIEWPORT _viewport;
        D3D12_RECT _scissor_rect;

        ClearColorRGB _clear_color;
        bool _update_flag;
    };
}
