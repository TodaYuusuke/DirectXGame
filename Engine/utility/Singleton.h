#pragma once
#include <cassert>
#include <list>

namespace LWP::Utility {
	/// <summary>
	/// シングルトンをまとめて解放するための処理
	/// </summary>
	namespace Singleton {
		// デストラクタ呼び出し用のクラス
		class DeleteObj {
		public:
			virtual ~DeleteObj() = default;
		};

		// ISingletonクラスを継承して作られたインスタンスをまとめたリスト
		extern std::list<DeleteObj*> ptrs_;

		/// <summary>
		/// 残りのすべてのシングルトンインスタンスを生成と逆順に解放する
		/// </summary>
		void DestroyAll();
	}

	/// <summary>
	/// 継承してシングルトンを作るクラス
	/// <para>【継承先で必要なこと（実装例はGPUDeviceを参照）】</para>
	/// <para>１．コンストラクタをプライベートに</para>
	/// <para>２．テンプレートに自身のクラスを渡す</para>
	/// <para>３．コンストラクタをこっちで呼び出すためにフレンドクラスにしてもらう</para>
	/// </summary>
	template<typename T>
	class ISingleton : public Singleton::DeleteObj {
	protected:	// ** プライベートなメンバ関数 ** //

		// コンストラクタ内でインスタンスを配列に登録
		ISingleton() { Singleton::ptrs_.push_back(this); }
		// デストラクタ内でインスタンスを配列から削除
		virtual ~ISingleton() {
			for (auto itr = Singleton::ptrs_.begin(); itr != Singleton::ptrs_.end(); itr++) {
				// 一致するアドレスを検索
				if (*itr == this) {
					Singleton::ptrs_.erase(itr);	// 削除
					return;
				}
			}
		};

		// コピーコンストラクタ削除
		ISingleton(const ISingleton&) = delete;
		ISingleton& operator=(const ISingleton&) = delete;
		// ムーブコンストラクタ削除
		ISingleton(ISingleton&&) = delete;
		ISingleton& operator=(ISingleton&&) = delete;


	public: // ** メンバ関数 ** //

		/// <summary>
		/// インスタンスを生成する関数
		/// </summary>
		static void Create() {
			if (!instance_) { instance_ = new T(); }
			else { assert(false); }		// 生成済みなのにCreateしているのでエラー
		}
		/// <summary>
		/// インスタンスを破棄する関数
		/// </summary>
		static void Destroy() {
			if (instance_) { delete instance_; }
			else { assert(false); }		// 生成していないのにDestroyしているのでエラー
		}
		/// <summary>
		/// インスタンスのポインタを受け取る関数
		/// </summary>
		static T* GetInstance() {
			assert(instance_);	// インスタンスがないのでエラー
			return instance_;
		}


	protected: // ** メンバ変数 ** //

		// インスタンス
		static T* instance_;
	};

	// 静的なメンバの実体宣言
	template<typename T>
	T* ISingleton<T>::instance_ = nullptr;
}

/// <summary>
/// 継承してシングルトンを作るクラス
/// <para>【継承先で必要なこと（実装例はGPUDeviceを参照）】</para>
/// <para>１．コンストラクタをプライベートに</para>
/// <para>２．テンプレートに自身のクラスを渡す</para>
/// <para>３．コンストラクタをこっちで呼び出すためにフレンドクラスにしてもらう</para>
/// </summary>
template<typename T>
class ISingleton : public Singleton::DeleteObj {
public: // ** メンバ関数 ** //

	// コピーコンストラクタ削除
	ISingleton(const ISingleton&) = delete;
	ISingleton& operator=(const ISingleton&) = delete;
	// ムーブコンストラクタ削除
	ISingleton(ISingleton&&) = delete;
	ISingleton& operator=(ISingleton&&) = delete;

	/// <summary>
	/// インスタンスのポインタを受け取る関数
	/// </summary>
	static T* GetInstance() {
		assert(instance_);	// インスタンスがないのでエラー
		return instance_;
	}


protected: // ** メンバ変数 ** //

	// インスタンス
	static T* instance_;
};