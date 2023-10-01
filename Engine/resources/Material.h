#pragma once
#include <memory>
#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include <wrl.h>

#include "../math/matrix/Matrix4x4.h"
#include "../Object/WorldTransform.h"

namespace LWP::Primitive {
	class Manager;
}

/// <summary>
/// マテリアル
/// </summary>
namespace LWP::Resource {
	// マテリアルの情報の構造体
	struct MaterialStruct {
		Math::Matrix4x4 uvMatrix;
		int32_t enableLighting = false;
	};

	class Material final {
	public:
		// マテリアルデータ
		MaterialStruct* data_;
		// uvTransform
		Object::WorldTransform uvTransform;

		Material(Primitive::Manager* primitiveManager);
		~Material() = default;

		// GPUアドレスを取得
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress();

		/// <summary>
		/// ImGui
		/// </summary>
		void DebugGUI(const std::string& label = "Material");

	private: // メンバ変数

		// マテリアル用のリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	};
}