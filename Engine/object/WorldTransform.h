#pragma once
#include "../math/Math.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include <wrl.h>
#include <string>

namespace LWP::Primitive {
	class Manager;
}

namespace LWP::Object {
	class WorldTransform {
	public:
		// ** 変数 ** //
 		
		// ローカルスケール
		Math::Vector3 scale;
		// X,Y,Z軸回りのローカル回転角
		Math::Vector3 rotation;
		// ローカル座標
		Math::Vector3 translation;


		// ** プロパティ変数 ** //

	private: // 親となるワールド変換へのポインタ（読み取り専用）
		WorldTransform* parent_ = nullptr;
	public: // アクセッサ
		void Parent(WorldTransform* parent);	// 親関係を登録
		//WorldTransform* Parent();	// 親関係のポインタを受け取る（実装禁止）

	private: // ワールド変換行列
		Math::Matrix4x4* matWorld_ = nullptr;
	public: // アクセッサ
		//void MatWorld(Math::Matrix4x4);	// ワールド行列をセットする（実装禁止）
		Math::Matrix4x4 MatWorld();	// ワールド行列を返す


	public: // ** パブリックなメンバ関数 ** //
		// コンストラクタ
		WorldTransform();
		WorldTransform(Math::Vector3 s, Math::Vector3 r, Math::Vector3 t);
		WorldTransform(Math::Vector3 r, Math::Vector3 t);
		WorldTransform(Math::Vector3 t);
		~WorldTransform() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// リソースを作成する
		/// </summary>
		void CreateResource(Primitive::Manager* manager);

		/// <summary>
		/// ワールド座標を返す
		/// </summary>
		Math::Vector3 GetWorldPosition();
		
		/// <summary>
		// GPUアドレスを取得
		/// </summary>
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() { return resource_.Get()->GetGPUVirtualAddress(); }

		/// <summary>
		/// ImGui
		/// </summary>
		void DebugGUI(const std::string& label = "WorldTransform");

	private: // ** プライベートな変数 ** //

		Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;	// リソース


	private: // ** プライベートな関数 ** //
	};
}