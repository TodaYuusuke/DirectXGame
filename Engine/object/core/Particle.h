#pragma once
#include "../IObject.h"

#include <functional>

#include "resources/model/RigidModel.h"
#include "collision/Collision.h"

namespace LWP::Object {
	/// <summary>
	/// パーティクル
	/// </summary>
	class Particle : public IObject {
	public: // ** パブリックなメンバ変数 ** //
		// 構造体
		struct Data {
			Resource::RigidModel m;
			LWP::Math::Vector3 velocity = { 0.0f,0.0f,0.0f };
			LWP::Object::Collision* collider = nullptr;	// コライダーを設定する場合はインスタンスをここに
			float elapsedTime = 0.0f;
			int idNumber = -1;

			// デフォルトコンストラクタ
			Data() = delete;
			Data(const Resource::RigidModel& m);
			// デストラクタ
			~Data();

			// コピー演算子
			Data& operator=(const Data& other);
		};

		// 基準となるモデルクラス
		Resource::RigidModel model;
		// 識別番号
		int IDNumber = 0;	// 21億個生成したらバグるけどいったん放置


	public: // ** メンバ関数 ** //
		
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Particle();
		/// <summary>
		/// デストラクタ
		/// </summary>
		~Particle();
		
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize() override final {};
		/// <summary>
		/// 更新
		/// </summary>
		/// <param name="manager"></param>
		void Update(Base::RendererManager* manager) override final;

		/// <summary>
		/// パーティクルを生成
		/// </summary>
		/// <param name="value">生成する個数</param>
		void Add(int value);
		/// <summary>
		/// 場所を指定してパーティクルを生成
		/// </summary>
		/// <param name="value">生成する個数</param>
		void Add(int value, LWP::Math::Vector3 position);

		/// <summary>
		/// デバッグ用GUI
		/// </summary>
		void DebugGUI() override final;

	protected: // ** 派生先で定義してもらう関数 ** //
		// コンストラクタとデストラクタの定義は禁止
		
		/// <summary>
		/// パーティクルを１つ生成する度に呼ばれる関数
		/// </summary>
		/// <param name="newData">生成された実体の参照（mに基準となるmodelは既に代入されている）</param>
		/// <returns></returns>
		virtual void Generate(Data& newData) = 0;
		/// <summary>
		/// パーティクルの更新処理
		/// </summary>
		/// <param name="data">更新する実態の参照</param>
		/// <returns></returns>
		virtual bool UpdateParticle(Data& data) = 0;


	private: // ** メンバ変数 ** //

		// パーティクルのトランスフォーム
		std::list<Data> data_;
	};
}