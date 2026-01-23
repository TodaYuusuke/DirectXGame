#pragma once
#include "primitive/component/Skeleton.h"
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
		// モーションブレンドの度合い（0.0f~1.0f）
		// 0.0fに近いほどmain　～　1.0fに近いほどblendのアニメーションに近づく
		float blendT = 0.5f;

		/// <summary>
		/// アニメーションを再生するトラック
		/// </summary>
		enum class TrackType {
			Main,	// 通常の再生
			Blend,	// 通常と合わせてブレンドする用
			Multi0, // 同時再生スロット1
			Multi1, // 同時再生スロット2
			Multi2, // 同時再生スロット3
			Multi3, // 同時再生スロット4
			Multi4, // 同時再生スロット5
			Count, // カウント
		};


	public: // **　メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		Animation();
		/// <summary>
		/// デストラクタ
		/// </summary>
		~Animation();

		/// <summary>
		/// モーション経過時間の初期化
		/// </summary>
		void Init();
		/// <summary>
		/// 更新（ユーザー呼び出し不要）
		/// </summary>
		void Update();

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
		/// アニメーション開始
		/// </summary>
		/// <param name="name">再生するアニメーション名</param>
		/// <param name="transitionTime">モーションの遷移にかかる時間(0.0f以上)</param>
		/// <param name="startTime">開始時間(0.0f ~ 1.0f)</param>
		/// <param name="track">再生するトラックを指定</param>
		Animation& Play(const std::string animName, float transitionTime = 0.0f, float startTime = 0.0f, TrackType type = TrackType::Main);
		/// <summary>
		/// アニメーション一時停止
		/// </summary>
		Animation& Pause(TrackType type = TrackType::Main);
		/// <summary>
		/// アニメーション停止から再生
		/// </summary>
		Animation& Resume(TrackType type = TrackType::Main);
		/// <summary>
		/// アニメーション完全に停止
		/// </summary>
		Animation& Stop(TrackType type = TrackType::Main);
		/// <summary>
		/// アニメーションの進行時間を直接指定する
		/// </summary>
		void SetTime(float second, TrackType type = TrackType::Main);

		/// <summary>
		/// ループ再生するかのフラグを切り替え
		/// </summary>
		Animation& Loop(TrackType type = TrackType::Main);
		/// <summary>
		/// ループ再生するかを設定
		/// </summary>
		Animation& Loop(bool b, TrackType type = TrackType::Main);
		/// <summary>
		/// 逆再生するかのフラグを切り替え
		/// </summary>
		Animation& Reverse(TrackType type = TrackType::Main);
		/// <summary>
		/// 逆再生するかを設定
		/// </summary>
		Animation& Reverse(bool b, TrackType type = TrackType::Main);
		/// <summary>
		/// タイムスケールを使うかのフラグを切り替え
		/// </summary>
		Animation& UseTimeScale(TrackType type = TrackType::Main);
		/// <summary>
		/// タイムスケールを使うかを設定
		/// </summary>
		Animation& UseTimeScale(bool b, TrackType type = TrackType::Main);
		/// <summary>
		/// 再生速度を設定
		/// </summary>
		/// <param name="type"></param>
		/// <returns></returns>
		float& GetPlayBackSpeed(TrackType type = TrackType::Main) { return tracks_[type].playbackSpeed; }

		/// <summary>
		/// mainとblendの進行度を同期する
		/// </summary>
		void Sync();
		/// <summary>
		/// Debug用のImGui
		/// </summary>
		void DebugGUI();

		/// <summary>
		/// <para>指定のアニメーションが再生中か返す関数</para>
		/// <para>※ 指定しない場合は何かしらが再生しているかを返す</para>
		/// </summary>
		bool GetPlaying(const std::string& animName = "", TrackType type = TrackType::Main) { return tracks_[type].GetPlaying(animName); }
		/// <summary>
		/// 再生できるアニメーションの名前を取得する関数
		/// </summary>
		std::vector<std::string> GetAnimationNames() const;
		/// <summary>
		/// アニメーションに掛かる時間を受け取る関数
		/// </summary>
		float GetTotalSeconds(const std::string& animName = "");
		/// <summary>
		/// アニメーションに掛かる時間を受け取る関数
		/// </summary>
		float GetTotalSeconds(TrackType type = TrackType::Main) { return tracks_[type].totalSeconds; }
		/// <summary>
		/// 全体の進捗を受け取る関数
		/// </summary>
		float GetProgress(TrackType type = TrackType::Main) { return tracks_[type].time; }
		/// <summary>
		/// 進捗を秒で受け取る関数
		/// </summary>
		float GetProgressSeconds(TrackType type = TrackType::Main) { return tracks_[type].totalSeconds * tracks_[type].time; }
		/// <summary>
		/// 1ループが終わった瞬間のフレームだけtrueを返す関数
		/// </summary>
		/// <returns></returns>
		bool GetEndOneLoop(TrackType type = TrackType::Main) { return tracks_[type].isEndOneLoop; }

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

		// 再生するアニメーションごとのデータ
		struct State {
			// 再生中のアニメーションの名前
			std::string playingAnimationName = "";
			// 経過割合(0.0f ~ 1.0f)
			float time = 0.0f;
			// アニメーションの長さ（秒s）
			float totalSeconds = 0.0f;

			// アニメーションの再生速度
			float playbackSpeed = 1.0f;
			// 停止フラグ
			bool isPause = true;	// 最初はなんも再生しないので初期値true
			// ループするかフラグ
			bool isLoop = false;
			// アニメーション逆再生フラグ
			bool isReverse = false;
			// タイムスケールの影響OnOff
			bool isUseTimeScale = true;

			// 1ループが終わるたびに1フレームだけtrueになるフラグ
			bool isEndOneLoop = false;

			/// <summary>
			/// 初期化
			/// </summary>
			void Init() {
				time = 0.0f;
				playingAnimationName = "";
			}
			/// <summary>
			/// 更新
			/// </summary>
			void Update();

			/// <summary>
			/// システムからデルタタイムを取得する関数
			/// </summary>
			/// <returns></returns>
			float GetDeltaTime();
			/// <summary>
			/// <para>指定のアニメーションが再生中か返す関数</para>
			/// <para>※ 指定しない場合は何かしらが再生しているかを返す</para>
			/// </summary>
			bool GetPlaying(const std::string& animName = "");

			/// <summary>
			/// Debug用のImGui
			/// </summary>
			/// <returns>変更があったかどうか</returns>
			bool DebugGUI();
		};
		std::map<TrackType, State> tracks_;

		// モーショントランジションの情報
		struct Transition {
			// トランジション用のスケルトン
			Primitive::Skeleton tempSkeleton;
			// モーショントランジションにかかる時間
			float totalTime = 0.0f;
			// トランジションの経過割合(0.0f ~ 1.0f)
			float t = 0.0f;
			
			// モーショントランジション中かのフラグ
			bool isActive = false;
		}transition_;


		// アニメーションファイルの位置
		const static std::string kDirectoryPath_;
		// 読み込んだファイル名
		std::string loadedPath = "";

		// 適応するModelのポインタ
		Resource::SkinningModel* modelPtr_ = nullptr;

	private: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// 任意の時刻の値を取得する関数
		/// </summary>
		Math::Vector3 CalculateValue(const std::vector<Keyframe<Math::Vector3>>& keyframes, float time);
		/// <summary>
		/// 任意の時刻の値を取得する関数
		/// </summary>
		Math::Quaternion CalculateValue(const std::vector<Keyframe<Math::Quaternion>>& keyframes, float time);

		/// <summary>
		/// Jointを更新する関数
		/// </summary>
		void UpdateJoint();
		
	};
};