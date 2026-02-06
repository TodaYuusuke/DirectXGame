#pragma once
#include "../utility/RootSignature.h"
#include "../utility/PSO.h"
#include "IPostProcess.h"

#include "base/directX/resource/data/ConstantBuffer.h"
#include <fstream>
#include <vector>
#include <memory>
#include <string>

namespace LWP::Base {
	/// <summary>
	/// ポストプロセスの共通パラメータ構造体
	/// </summary>
	struct PostProcessParameter {
		float time;
		int rWidth;
		int rHeight;
		int padding;
	};

	/// <summary>
	/// ポストプロセス管理クラス
	/// <para>HLSLコードをバイナリ形式のGPUシェーダーに変換する</para>
	/// </summary>
	class PostProcessor final {
	public: // ** パブリックなメンバ変数 ** //

		// そもそも行うかフラグ
		bool isActive = false;


	public: // ** メンバ関数 ** //

		// 初期化
		void Init();
		// 更新
		void Update();

		// 描画する前に行わなければならない処理
		void PreCommands(ID3D12GraphicsCommandList* list, Base::RenderResource* target);
		// コマンドリストにView当をセットする関数
		void SetCommands(ID3D12GraphicsCommandList* list);

		/// <summary>
		/// Debug用のImGui
		/// </summary>
		void DebugGUI();

		// 全てのパスを取得
		const std::vector<std::shared_ptr<PostProcess::IPostProcess>>& GetPasses() const { return passes_; }

		/// <summary>
		/// パスを追加する関数
		/// </summary>
		template<class T>
		T* AddPass() {
			std::shared_ptr<T> pass = std::make_shared<T>();
			pass->Init();
			passes_.push_back(pass);
			CreateShaderFile();	// 追加されたのでシェーダー更新
			return pass.get();
		}
		// 全てのパスをクリア
		void ClearPasses() { passes_.clear(); }

		/// <summary>
		/// 指定した型のパスを取得する（先頭のものしか取得できないので注意！）
		/// </summary>
		template<class T>
		T* GetPass() {
			for (auto& pass : passes_) {
				if (T* ptr = dynamic_cast<T*>(pass.get())) {
					return ptr;
				}
			}
			return nullptr;
		}


	private: // ** メンバ変数 ** //

		// RootSignature
		RootSignature root_;
		// PSO
		PSO pso_;

		// 共通パラメータ
		Base::ConstantBuffer<PostProcessParameter> commonBuffer_;

		// 適用するポストプロセス達
		std::vector<std::shared_ptr<PostProcess::IPostProcess>> passes_;


	private: // ** プライベートなメンバ関数 ** //

		// パラメーターからシェーダーファイルを生成しPSOを生成する関数
		void CreateShaderFile();
	};
}
