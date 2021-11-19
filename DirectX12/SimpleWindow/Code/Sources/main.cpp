#include <Windows.h>
#include <tchar.h>
#include <assert.h>
#include <vector>

#ifdef _DEBUG
#include <iostream>
#endif

// DXをインクルード
// これらのファイル群はどの環境にも入っているわけじゃないよね？
// ue4はエンジンプロジェクトの中にDXのファイル群が含まれていた
#include <d3d12.h>
#include <dxgi1_6.h>

// DXのライブラリを含める
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace std;

void DebugOutputFormatString(const char* in_p_format, ...)
{
#ifdef _DEBUG
	va_list valist;
	// 変数名をva_listとするとva_list型と混在してコンパイルエラーになるので注意
	va_start(valist, in_p_format);
	printf(in_p_format, valist);
	va_end(valist);
#endif
}

void EnableDebugLayer()
{
	// デバッグレイヤーのインターフェイス取得
	ID3D12Debug* p_debug_layer = nullptr;
	auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&p_debug_layer));
	assert(result == S_OK);

	// レイヤーを有効
	p_debug_layer->EnableDebugLayer();
	// インターフェイスはもう不要なので解放
	p_debug_layer->Release();
}

LRESULT WindowProcedure(HWND in_hwnd, UINT in_msg, WPARAM in_wparam, LPARAM in_lparam)
{
	if (in_msg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(in_hwnd, in_msg, in_wparam, in_lparam);
}

// デバッグと非デバッグとでエントリーポイントを分けている
// これはデバッグの場合はコンソールウィンドウを出してデバッグ情報を出力するため
#ifdef _DEBUG
int main()
{
	EnableDebugLayer();
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#endif
	DebugOutputFormatString("DirectX12 Sample.");

	WNDCLASSEX w = {};

	w.cbSize = sizeof(w);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;
	w.lpszClassName = _T("DX12Sample");
	w.hInstance = GetModuleHandle(nullptr);

	RegisterClassEx(&w);

	LONG window_width = 640;
	LONG window_height = 480;
	RECT wrc = { 0, 0, window_width, window_height };

	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	HWND hwnd = CreateWindow(
		w.lpszClassName,
		_T(""),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		w.hInstance,
		nullptr
	);

	// DXGIはハードウェアやドライブを直接制御ができる
	// 主な用途としては
	// DXの描画インフラストラクチャーを生成して画面フリップや描画デバイスを制御
	IDXGIFactory6* p_dxgi_factory = nullptr;
	{
		// 末尾の数字は何？
#ifdef _DEBUG
		auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&p_dxgi_factory));
		assert(result == S_OK);
#else
		auto result = CreateDXGIFactory1(IID_PPV_ARGS(&p_dxgi_factory));
		assert(result == S_OK);
#endif
	}

	// DXのデバイス作成
	ID3D12Device* p_dev = nullptr;
	{
		D3D_FEATURE_LEVEL feture_level;
		// 失敗は考えない
		{
			D3D_FEATURE_LEVEL levels[] = {
				D3D_FEATURE_LEVEL_12_1,
				D3D_FEATURE_LEVEL_12_0,
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0,
				D3D_FEATURE_LEVEL_9_3,
				D3D_FEATURE_LEVEL_9_2,
				D3D_FEATURE_LEVEL_9_1,
			};
			for (auto level : levels)
			{
				// デバイス指定はnullptrで自動指定にしている
				if (D3D12CreateDevice(nullptr, level, IID_PPV_ARGS(&p_dev)) == S_OK)
				{
					feture_level = level;
					break;
				}
			}

			assert(p_dev != nullptr);
		}
	}

	// DXのコマンドリストを生成
	ID3D12CommandAllocator* p_cmd_allocator = nullptr;
	ID3D12GraphicsCommandList* p_cmd_list = nullptr;
	{
		// GPUに命令するためのコマンドリストのデータ
		// メモリサイズは動的に変わるのだろうか？
		auto result_command_allocator = p_dev->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&p_cmd_allocator));
		assert(result_command_allocator == S_OK);

		// GPUに命令するためのメソッドインターフェイスを作成
		auto result_command_list = p_dev->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			p_cmd_allocator,
			nullptr,
			IID_PPV_ARGS(&p_cmd_list));
		assert(result_command_list == S_OK);
	}

	// DXのコマンドリストを実行するコマンドを生成してコマンドキューを生成
	ID3D12CommandQueue* p_cmd_queue = nullptr;
	{
		D3D12_COMMAND_QUEUE_DESC cmd_queue_desc = {};

		// タイムアウトなし
		cmd_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		// アダプターを１つのみ使うなら0
		cmd_queue_desc.NodeMask = 0;
		// プライオリティーの設定を通常
		cmd_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		// コマンドリストを合わせる
		cmd_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		auto result_command_queue = p_dev->CreateCommandQueue(
			&cmd_queue_desc,
			IID_PPV_ARGS(&p_cmd_queue));
		assert(result_command_queue == S_OK);
	}

	// DXのスワップチェーンの生成
	IDXGISwapChain4* p_swapchain = nullptr;

	// ダブルバッファリングをするために必要
	// ウィンドウモード前提
	{
		// まずスワップチェイン用の設定データを作る
		DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
		swap_chain_desc.Width = window_width;
		swap_chain_desc.Height = window_height;
		swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swap_chain_desc.Stereo = false;
		// マルチサンプルの指定
		swap_chain_desc.SampleDesc.Count = 1;
		swap_chain_desc.SampleDesc.Quality = 0;

		swap_chain_desc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
		// ダブルバッファリングするのにで2
		swap_chain_desc.BufferCount = 2;

		// バックバッファは伸び縮みできる
		swap_chain_desc.Scaling = DXGI_SCALING_STRETCH;

		// フリップ後は破棄
		swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		// これなに？
		swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

		// ウィンドウとフルスクリーン切り替え自由に
		swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// エラーとしても何が原因でエラーになったのかを出力してくれないのだろうか？
		// スワップチェーン生成にはDXGIが必要なのだろうか？
		auto result_swap_chain = p_dxgi_factory->CreateSwapChainForHwnd(
			p_cmd_queue,
			hwnd,
			&swap_chain_desc,
			nullptr,
			nullptr,
			(IDXGISwapChain1**)&p_swapchain);
		// 成功するとBufferCount数分、バッファが作成される
		assert(result_swap_chain == S_OK);
	}

	// ディスクリプタヒープを作成
	// 今回はレンダーターゲットビュー用に作成
	ID3D12DescriptorHeap* p_rtv_heaps = nullptr;
	{
		D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
		// 作成するビューを指定
		heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		// GPUが複数ある場合に利用、一つなら０でいい
		heap_desc.NodeMask = 0;
		// 作成するディスクリプタの数、ダブルバッファリングなので２にする
		heap_desc.NumDescriptors = 2;
		heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		auto result = p_dev->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&p_rtv_heaps));
		assert(result == S_OK);
	}

	// スワップチェーンとメモリを紐づけ
	std::vector<ID3D12Resource*> back_buffers;
	{
		// スワップチェーンの説明データを取得してバッファを作成
		DXGI_SWAP_CHAIN_DESC swap_desc = {};
		auto result = p_swapchain->GetDesc(&swap_desc);
		assert(result == S_OK);

		// スワップチェーンのバッファに応じたバックバッファを作成
		back_buffers.resize(swap_desc.BufferCount);

		// ディスクリプタの先頭ハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE handle = p_rtv_heaps->GetCPUDescriptorHandleForHeapStart();
		for (UINT i = 0; i < swap_desc.BufferCount; ++i)
		{
			result = p_swapchain->GetBuffer(i, IID_PPV_ARGS(&back_buffers[i]));
			assert(result == S_OK);

			// レンダーをディスクリプタを指定して作成
			p_dev->CreateRenderTargetView(
				back_buffers[i],
				nullptr,
				handle
			);

			// ハンドルが参照するアドレスを更新して、次に参照するディスクリプタのアドレス値を作る
			handle.ptr += p_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}
	}

	// フェンスオブジェクトを作成
	ID3D12Fence* p_fence = nullptr;
	UINT64 fence_val = 0;
	{
		auto result = p_dev->CreateFence(fence_val, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&p_fence));
		assert(result == S_OK);
	}

	// ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);

	MSG msg = {};
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;

		// バックバッファのレンダーターゲットインデックスを取得
		auto bb_idx = p_swapchain->GetCurrentBackBufferIndex();

		// バックバッファをレンダーターゲットとして使用通知のバリアコマンド追加
		D3D12_RESOURCE_BARRIER barrier_desc = {};
		{
			barrier_desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier_desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier_desc.Transition.pResource = back_buffers[bb_idx];
			barrier_desc.Transition.Subresource = 0;

			// 利用するリソースをレンダーターゲットとして指定
			barrier_desc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier_desc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

			p_cmd_list->ResourceBarrier(1, &barrier_desc);
		}

		// レンダーターゲットビューを設定コマンド追加
		// 参照するディスクリプタのポインターを利用してレンダーターゲットビューを設定
		auto rtvH = p_rtv_heaps->GetCPUDescriptorHandleForHeapStart();
		{
			rtvH.ptr += bb_idx * p_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			p_cmd_list->OMSetRenderTargets(
				1,
				&rtvH,
				true,
				nullptr);
		}

		// 設定したレンダーターゲットをクリア
		float clear_color[] = { 1.0f, 1.0f, 0.0f, 1.0f };
		p_cmd_list->ClearRenderTargetView(rtvH, clear_color, 0, nullptr);

		// バックバッファをフロントバッファにする
		{
			// リソース指定をPresentにする
			barrier_desc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier_desc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			p_cmd_list->ResourceBarrier(1, &barrier_desc);
		}

		// これ以上コマンドをためれないようにする
		p_cmd_list->Close();

		// コマンドリストの実行
		ID3D12CommandList* p_cmd_lists[] = { p_cmd_list };
		p_cmd_queue->ExecuteCommandLists(1, p_cmd_lists);

		// コマンド実行のフェンスを通知
		p_cmd_queue->Signal(p_fence, ++fence_val);
		{
			if (p_fence->GetCompletedValue() != fence_val)
			{
				// コマンドリストの命令が全て終了の待ち状態を作る
				auto event = CreateEvent(nullptr, false, false, nullptr);
				p_fence->SetEventOnCompletion(fence_val, event);

				// イベントが実行されるまで待ち
				WaitForSingleObject(event, INFINITE);

				CloseHandle(event);
			}
		}

		// コマンドリストのリセット
		p_cmd_allocator->Reset();
		// コマンドのクローズ状態を解除
		p_cmd_list->Reset(p_cmd_allocator, nullptr);

		// バッファを入れ替え
		p_swapchain->Present(1, 0);
	}

	UnregisterClass(w.lpszClassName, w.hInstance);

	return 0;
}