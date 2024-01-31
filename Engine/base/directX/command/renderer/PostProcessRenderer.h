#pragma once
#include "math/vector/Vector2.h"
#include "utility/Counter.h"
#include "resources/texture/RenderTexture.h"
#include "base/directX/command/RootSignature.h"
#include "base/directX/command/PSO.h"
#include "base/directX/descriptorHeap/HeapManager.h"
#include "Config.h"

namespace LWP::Base {
    /// <summary>
    /// ポストプロセスレンダリングコマンド
    /// </summary>
    class PostProcessRenderer {
    public: // ** メンバ関数 ** //
        // コンストラクタ
        PostProcessRenderer() = default;
        // デストラクタ
        ~PostProcessRenderer() = default;

        // 初期化処理
        void Init(ID3D12Device* device, DXC* dxc, HeapManager* heaps, RootSignature* root, std::string path);
        // レンダリング
        void Draw(ID3D12GraphicsCommandList* list, Resource::RenderTexture* target, bool isMain = false);

        // シェーダー作り直し
        void ReCreateShader(ID3D12Device* device, DXC* dxc, std::string path);


    private: // ** メンバ変数 ** //

        // レンダリングするためのデータ
        struct RenderData {
            int* time;	// 時間のデータ
            ID3D12Resource* resource;	// データのリソース
            D3D12_GPU_VIRTUAL_ADDRESS view;	// データのビュー
        };
        std::unique_ptr<RenderData> renderData_;

        // ルートシグネチャ（全ポストプロセスで共通なのでポインタ）
        RootSignature* rootPtr_;
        // PSO
        std::unique_ptr<PSO> pso_;
                
        // ディスクリプタヒープ管理クラスのポインタ
        HeapManager* heaps_ = nullptr;


    private: // ** メンバ変数 ** //

        void PreDraw(ID3D12GraphicsCommandList* list, Resource::RenderTexture* target);
        void PostDraw(ID3D12GraphicsCommandList* list, Resource::RenderTexture* target);
        
        void PreLastDraw(ID3D12GraphicsCommandList* list, Resource::RenderTexture* target);
        void PostLastDraw(ID3D12GraphicsCommandList* list);
    };
}