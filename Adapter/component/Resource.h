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
		/// テクスチャファイルの読み込み（resources/texture/を短縮ver）
		/// </summary>
		/// <param name="filePath">読み込むファイルパス</param>
		/// <returns>テクスチャのインスタンス</returns>
		Texture LoadTexture(const std::string& filePath);
		/// <summary>
		/// テクスチャファイルの読み込み（exeからのパスで指定）
		/// </summary>
		/// <param name="filePath">読み込むファイルパス</param>
		/// <returns>テクスチャのインスタンス</returns>
		Texture LoadTextureLongPath(const std::string& filePath);

		/// <summary>
		/// オーディオファイルの読み込み（resources/audio/を短縮ver）
		/// </summary>
		/// <param name="filePath">読み込むファイルパス</param>
		/// <returns>オーディオのインスタンス</returns>
		AudioData* LoadAudio(const std::string& filePath);
		/// <summary>
		/// オーディオファイルの読み込み（exeからのパスで指定）
		/// </summary>
		/// <param name="filePath">読み込むファイルパス</param>
		/// <returns>オーディオのインスタンス</returns>
		AudioData* LoadAudioLongPath(const std::string& filePath);

		/// <summary>
		/// 3Dモデルの読み込み（resources/audio/を短縮ver）（ユーザー呼び出し不要）
		/// </summary>
		/// <param name="filePath">読み込むファイルパス</param>
		/// <returns>オーディオのインスタンス</returns>
		const Primitive::MeshData& LoadMesh(const std::string& filePath);
		/// <summary>
		/// 3Dモデルの読み込み（exeからのパスで指定）（ユーザー呼び出し不要）
		/// </summary>
		/// <param name="filePath">読み込むファイルパス</param>
		/// <returns>オーディオのインスタンス</returns>
		const Primitive::MeshData& LoadMeshLongPath(const std::string& filePath);

		// クラスのインスタンスをエンジンにセットする関数群
		void SetInstance(Animation* ptr);
		void SetInstance(Motion* ptr);
		// クラスのインスタンスをエンジンから解放する関数群
		void DeleteInstance(Animation* ptr);
		void DeleteInstance(Motion* ptr);
	};
};