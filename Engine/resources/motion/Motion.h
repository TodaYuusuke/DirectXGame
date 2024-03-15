#pragma once
#include "utility/motionEffects/Easing.h"			// イージング関数
#include "utility/motionEffects/Interpolation.h"	// 補間関数

#include <functional>	// 関数ポインタ用

namespace LWP::Resource {
	/// <summary>
	/// モーションデータ構造体
	/// </summary>
	struct MotionData {
		LWP::Math::Vector3* target;
		LWP::Math::Vector3* start;	// 開始地点はモーション開始時に決定するのでポインタ
		LWP::Math::Vector3 move;
		float startSec;
		float durationSec;
		Utility::Easing::Type easingType;
		Utility::Interp::Type interType;
		bool isEnd;

		// 終点を返す関数
		LWP::Math::Vector3 End() {
			return *start + move;
		}
	};

	/// <summary>
	/// モーションクラス（ビルダーデザインパターン）
	/// </summary>
	class Motion final {
	public: // **　メンバ関数 ** //
		// デフォルトコンストラクタ
		Motion();
		~Motion();

		/// <summary>
		/// 動作を追加する関数
		/// </summary>
		/// <param name="target">動かす対象のアドレス</param>
		/// <param name="move">移動量</param>
		/// <param name="startSec">開始する秒数</param>
		/// <param name="durationSec">掛かる秒数</param>
		/// <param name="easingType">イージングタイプを指定(default = Liner)</param>
		/// <param name="interType">補間タイプを指定(default = Lerp)</param>
		/// <returns></returns>
		Motion& Add(LWP::Math::Vector3* target, const LWP::Math::Vector3& move, 
					const float& startSec, const float& durationSec,
					Utility::Easing::Type easingType = Utility::Easing::Type::Liner, Utility::Interp::Type interType = Utility::Interp::Type::Lerp);
		/// <summary>
		/// デルタタイム係数の影響を受けないように設定（default = true）
		/// </summary>
		/// <returns></returns>
		Motion& DisableDeltaTimeMultiply();

		/// <summary>
		/// モーション開始
		/// </summary>
		void Start();
		void Start(float startSec);

		/// <summary>
		/// 更新（ユーザー呼び出し不要）
		/// </summary>
		void Update();

		/// <summary>
		/// アニメーションが実行中ならばfalse、終了済みならばtrueを返す
		/// </summary>
		/// <returns></returns>
		bool isEnd();


	private: // ** メンバ変数 ** //
		// モーションデータ一覧
		std::vector<MotionData> data_;
		// デルタタイム係数の影響をうけるか
		bool isUseDeltaTimeMultiply_ = true;

		// 開始フラグ
		bool isStart_ = false;
		// モーション経過時間（秒数）
		float currentSec_ = 0.0f;

	private: // ** プライベートなメンバ関数 ** //
		// 追加処理
		Motion& Add(MotionData data);


	private: // ** 共通の関数ポインタ ** //
		// イージング関数ポインタ
		static std::function<float(float)> easingFunc[];
		// 補間関数ポインタ
		static std::function<Math::Vector3(const Math::Vector3&, const Math::Vector3&, const float&)> interpFunc[];
	};
};