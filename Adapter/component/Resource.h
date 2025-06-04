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
		/// オーディオファイルの読み込み（resources/audio/を短縮ver）
		/// </summary>
		/// <param name="filePath">読み込むファイルパス</param>
		/// <returns>オーディオのインスタンス</returns>
		Primitive::OldMeshData* LoadMesh(const std::string& filePath);
		/// <summary>
		/// オーディオファイルの読み込み（exeからのパスで指定）
		/// </summary>
		/// <param name="filePath">読み込むファイルパス</param>
		/// <returns>オーディオのインスタンス</returns>
		Primitive::OldMeshData* LoadMeshLongPath(const std::string& filePath);

		/// <summary>
		/// 3Dモデルの読み込み（exeからのフルパス指定）（ユーザー呼び出し禁止）
		/// </summary>
		/// <param name="filePath">読み込むファイルのフルパス</param>
		void LoadModel(const std::string& filePath);
		/// <summary>
		/// 読み込んだ3Dモデルのポインタを受け取る（exeからのフルパス指定）（ユーザー呼び出し禁止）
		/// </summary>
		/// <param name="filePath">読み込むファイルのフルパス</param>
		ModelData* GetModel(const std::string& filePath);

	};
};