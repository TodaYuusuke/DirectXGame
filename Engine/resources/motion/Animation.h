#pragma once
#include "math/vector/Vector3.h"
#include "math/Quaternion.h"
#include <string>
#include <vector>
#include <map>

// 前方宣言
namespace LWP::Resource {
	class Model;
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
	public: // **　メンバ関数 ** //
		// デフォルトコンストラクタ
		Animation();
		// デストラクタ
		~Animation();

		/// <summary>
		/// デルタタイム係数の影響を受けるかどうかを切り替え（default = true）
		/// </summary>
		/// <returns></returns>
		void ToggleDeltaTimeMultiply() { useDeltaTimeMultiply_ = !useDeltaTimeMultiply_; }

		/// <summary>
		/// モーション開始
		/// </summary>
		void Start();
		void Start(float startSec);

		/// <summary>
		/// モーション停止
		/// </summary>
		void Stop();

		/// <summary>
		/// 更新（ユーザー呼び出し不要）
		/// </summary>
		void Update();

		/// <summary>
		/// アニメーションが実行中ならばfalse、終了済みならばtrueを返す
		/// </summary>
		/// <returns></returns>
		bool isEnd();
		
		/// <summary>
		/// アニメーション読み込み
		/// </summary>
		void LoadAnimation(std::string filepath, Resource::Model* ptr);
		void LoadAnimationLongPath(std::string filepath, Resource::Model* ptr);

	private: // ** メンバ変数 ** //

		// アニメーション全体の尺（秒）
		float duration_;
		// NodeAnimationの集合。Node名でひけるようにしておく
		std::map<std::string, NodeAnimation> nodeAnimations;

		// 開始フラグ
		bool isStart_ = false;
		// モーション経過時間（秒数）
		float time_ = 0.0f;
		// deltaTimeの係数の影響OnOff
		bool useDeltaTimeMultiply_ = true;

		// 適応するModelのポインタ
		Resource::Model* modelPtr_ = nullptr;

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