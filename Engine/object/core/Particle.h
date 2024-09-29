#pragma once
#include "../IObject.h"

#include <functional>

#include "resources/model/RigidModel.h"
#include "../collider/Collider.h"

#include "../Externals/imgui/imgui.h"
#include "../Externals/imgui/imgui_impl_dx12.h"
#include "../Externals/imgui/imgui_impl_win32.h"
#include "../Externals/imgui/imgui_stdlib.h"

namespace LWP::Object {
	/// <summary>
	/// パーティクル
	/// </summary>
	template<IsRigidModel T>
	class Particle : public IObject {
	public: // ** パブリックなメンバ変数 ** //
		// 構造体
		struct Data {
			T m;
			LWP::Math::Vector3 velocity = { 0.0f,0.0f,0.0f };
			LWP::Object::Collider::Collider* collider = nullptr;
			int elapsedFrame = 0;
			int idNumber;

			// デフォルトコンストラクタ
			Data() = delete;
			Data(const T& t) {
				m = t;
			}

			// デストラクタ
			~Data() {
				if (collider) { delete collider; }
			};


			Data& operator=(const Data& other) {
				if (this != &other) {
					m = other.m;
					velocity = other.velocity;
					collider = other.collider;
					elapsedFrame = other.elapsedFrame;
				}
				return *this;
			}
		};

		// 基準となるモデルクラス
		T model;
		// 識別番号
		int IDNumber = 0;	// 21億個生成したらバグるけどいったん放置

	public: // ** メンバ関数 ** //
		// デストラクタ
		~Particle() = default;

		// 初期化
		void Initialize() override final {};
		// 更新
		void Update(Base::RendererManager* manager) override final {
			if (!isActive) { return; }
			// 必須の設定だがいい感じに設定できるタイミングがないので無理やりここで設定
			model.isActive = false;

			// イテレータを使用してリストを走査しながら要素を削除
			for (auto it = data_.begin(); it != data_.end(); ) {
				if (UpdateParticle(*it)) {
					it = data_.erase(it); // 要素を削除し、新しいイテレータを取得
				}
				else {
					it++; // 次の要素に進む
				}
			}
			// パーティクル用のデータ登録関数を呼び出す
			//manager->AddParticleData(primitive, data);
			manager;
		}

		// パーティクルを追加
		void Add(int value) {
			for (int i = 0; i < value; i++) {
				data_.emplace_back(model);
				data_.back().m.isActive = true;
				data_.back().idNumber = IDNumber++;	// 識別番号を付与
				Generate(data_.back());
			}
		}
		void Add(int value, LWP::Math::Vector3 position) {
			model.worldTF.translation = position;
			Add(value);
		}
		// デバッグ用GUI
		void DebugGUI() override final {
			model.DebugGUI();
			ImGui::Checkbox("isActive", &isActive);
			ImGui::Text("Count %d", data_.size());
		}

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