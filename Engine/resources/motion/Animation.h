#pragma once
#include "math/vector/Vector3.h"
#include "math/Quaternion.h"
#include <string>
#include <vector>
#include <map>

// 前方宣言
namespace LWP::Resource {
	class SkinningModel;
}

namespace LWP::Resource {
	// キーフレーム構造体
	template <typename tValue>
	struct Keyframe {
		float time;
		tValue value;
	};
	// アニメーションカーブ
	template <typename tValue>
	struct AnimationCurve {
		std::vector<Keyframe<tValue>> keyframes;
	};

	// ノードアニメーション
	struct NodeAnimation {
		AnimationCurve<Math::Vector3> translate;
		AnimationCurve<Math::Quaternion> rotate;
		AnimationCurve<Math::Vector3> scale;
	};

	/// <summary>
	/// アニメーションクラス
	/// </summary>
	class Animation final {
	public: // **　パブリックなメンバ変数 ** //
		
		// 固有名詞
		std::string name = "Animation";
		// deltaTimeの係数影響OnOff
		bool useDeltaTimeMultiply = true;

		// アクティブ切り替え
		bool isActive = true;

	public: // **　メンバ関数 ** //

		// デフォルトコンストラクタ
		Animation();
		// デストラクタ
		~Animation();

		/// <summary>
		/// モーション経過時間の初期化
		/// </summary>
		void Init();

		/// <summary>
		/// アニメーション開始
		/// </summary>
		/// <param name="name">再生するアニメーション名</param>
		void Play(const std::string animName);
		/// <summary>
		/// アニメーション開始
		/// </summary>
		/// <param name="name">再生するアニメーション名</param>
		/// <param name="loop">ループするかのフラグ</param>
		void Play(const std::string animName, bool loop);
		/// <summary>
		/// アニメーション開始
		/// </summary>
		/// <param name="name">再生するアニメーション名</param>
		/// <param name="loop">ループするかのフラグ</param>
		/// <param name="startTime">開始時間(0.0f ~ 1.0f)</param>
		void Play(const std::string animName, bool loop, float startTime);
		
		/// <summary>
		/// アニメーション停止
		/// </summary>
		void Stop();

		/// <summary>
		/// 更新（ユーザー呼び出し不要）
		/// </summary>
		void Update();

		/// <summary>
		/// アニメーション中か返す関数
		/// </summary>
		bool GetPlaying();
		/// <summary>
		/// 指定のアニメーションが再生中か返す関数
		/// </summary>
		bool GetPlaying(const std::string& animName);
		
		/// <summary>
		/// 全体の進捗を受け取る関数
		/// </summary>
		float GetProgress() { return time_; }

		/// <summary>
		/// ImGui
		/// </summary>
		void DebugGUI();

		/// <summary>
		/// アニメーションのデータを読み込む
		/// </summary>
		/// <param name="fileName">読み込むファイルの名前</param>
		//void Load(const std::string& fileName, Resource::SkinningModel* ptr);
		/// <summary>
		/// アニメーションのデータを読み込む（resources/model/を短縮ver）
		/// </summary>
		/// <param name="filePath">読み込むファイルの名前</param>
		void LoadShortPath(const std::string& filePath, Resource::SkinningModel* ptr);
		/// <summary>
		/// アニメーションのデータを読み込む（exeからのパス指定）
		/// </summary>
		/// <param name="filePath">読み込むファイルの名前</param>
		void LoadFullPath(const std::string& filePath, Resource::SkinningModel* ptr);

		/// <summary>
		/// 読み込んだパスを返す関数
		/// </summary>
		std::string GetLoadedPath() { return loadedPath; }

	private: // ** メンバ変数 ** //

		struct AnimationData {
			// NodeAnimationの集合。Node名でひけるようにしておく
			std::map<std::string, NodeAnimation> node;
			// アニメーショントータル時間（秒）
			float totalTime;
		};
		std::map<std::string, AnimationData> data;

		// 経過割合(0.0f ~ 1.0f)
		float time_ = 0.0f;
		
		// 再生中のアニメーションの名前
		std::string playingAnimationName_ = "";
		// ループするかフラグ
		bool loopFlag_ = false;

		// 適応するModelのポインタ
		Resource::SkinningModel* modelPtr_ = nullptr;
		// 読み込んだファイル名
		std::string loadedPath = "";

		// アニメーションファイルの位置
		const static std::string kDirectoryPath;

	
	private: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// 任意の時刻の値を取得する関数
		/// </summary>
		Math::Vector3 CalculateValue(const std::vector<Keyframe<Math::Vector3>>& keyframes, float time);
		/// <summary>
		/// 任意の時刻の値を取得する関数
		/// </summary>
		Math::Quaternion CalculateValue(const std::vector<Keyframe<Math::Quaternion>>& keyframes, float time);
	};
};