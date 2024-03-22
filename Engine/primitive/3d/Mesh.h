#pragma once
#include "../IPrimitive.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace LWP::Primitive {
	/// <summary>
	/// 3Dモデル
	/// </summary>
	class Mesh final
		: public IPrimitive {

	public: // ** 関数 ** //

		// 初期化を呼び出す
		Mesh() { Init(); }
		
		/// <summary>
		/// 頂点を生成する関数
		/// </summary>
		void CreateVertices() override { /* 3Dモデルでは削除する */ };
		/// <summary>
		/// インデックスを生成する関数
		/// </summary>
		void CreateIndexes() override { /* 3Dモデルでは削除する */ };
		
		/// <summary>
		/// モデルデータを読み込む
		/// </summary>
		/// <param name="filename">読み込むファイルの名前</param>
		void LoadFile(const std::string& filename);
		
		/// <summary>
		/// 頂点数を返す関数
		/// </summary>
		int GetVertexCount() const override { return static_cast<int>(vertices.size()); };
		/// <summary>
		/// インデックスの数を返す関数
		/// </summary>
		int GetIndexCount() const override { return static_cast<int>(indexes.size()); };

	private: // ** ファイルパス ** //
		const std::string directoryPath_ = "resources/obj/";
		// objがフォルダに入っている際の追加のディレクトリパス
		std::string path_ = "";

	private: // ** プライベートな関数 ** //

		/// <summary>
		/// ImGui
		/// </summary>
		//void DerivedDebugGUI(const std::string& label = "Derived") override;

		// 3Dモデルの形式別読み込み処理
		void LoadObj(const std::string& filename);


		// マテリアルの読み込み
		void LoadMtl(const std::string& filename);
	};
}