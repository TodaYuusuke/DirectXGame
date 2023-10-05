#pragma once
#include "System.h"

/// <summary>
/// LightWeightParticleEngine
/// </summary>
namespace LWP {
	/// <summary>
	/// リソース関連
	/// </summary>
	namespace Resource {
		/// <summary>
		/// テクスチャファイルの読み込み
		/// </summary>
		/// <param name="filePath">読み込むファイルパス</param>
		/// <returns>テクスチャのインスタンス</returns>
		static Texture* LoadTexture(const std::string& filePath);

		/// <summary>
		/// オーディオファイルの読み込み
		/// </summary>
		/// <param name="filePath">読み込むファイルパス</param>
		/// <returns>オーディオのインスタンス</returns>
		//static Audio* LoadAudio(const std::string& filePath);

		/// <summary>
		/// 3Dモデルの読み込み
		/// </summary>
		/// <typeparam name="TPrimitive">形の種類</typeparam>
		/// <returns>形のインスタンス</returns>
		static LWP::Primitive::Mesh* LoadModel(const std::string& filename);
	};
};