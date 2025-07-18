#pragma once
#include "CatmullRom.h"
#include "Color.h"
#include "Counter.h"
#include "IndexManager.h"
#include "motionEffects/Easing.h"
#include "motionEffects/Interpolation.h"
#include "observers/Observer.h"
#include "observers/Observer2.h"
#include "observers/ObserverPtr.h"
#include "PtrManager.h"
#include "StatePattern.h"
#include "JsonIO.h"
#include "Random.h"
//#include "UI.h"

#include "MyString.h"
#include <random>
#include <list>
#include <map>

namespace LWP::Utility {

	/// <summary>
	/// 任意の型Tのn個のデータから平均を求めて返す関数
	/// </summary>
	/// <typeparam name="T">任意の型</typeparam>
	/// <param name="data">データの配列</param>
	/// <param name="n">個数</param>
	/// <returns>平均値</returns>
	template<typename T>
	T CalculateAverage(const T* data, int n) {
		T sum = data[0];
		for (int i = 1; i < n; ++i)
		{
			sum += data[i];
		}

		return static_cast<T>(n) / sum;
	}

	/// <summary>
	/// radian から degreeへ
	/// </summary>
	float RadianToDegree(float radian);
	/// <summary>
	/// degree から radian へ
	/// </summary>
	float DegreeToRadian(float degree);

	/// <summary>
	/// 二つの方向ベクトル間のラジアンを求める関数
	/// </summary>
	/// <param name="vec1"></param>
	/// <param name="vec2"></param>
	/// <param name="axis"></param>
	/// <returns></returns>
	float GetRadian(const LWP::Math::Vector3& vec1, const LWP::Math::Vector3& vec2, LWP::Math::Vector3 axis = LWP::Math::Vector3::UnitY());


	// インデックスを指定してstd::listのイテレーターを取得する関数
	template<typename T>
	typename std::list<T>::iterator GetIteratorAtIndex(std::list<T>& lst, size_t index) {
		typename std::list<T>::iterator iter = lst.begin();
		std::advance(iter, index); // イテレーターをindex分進める
		return iter;
	}
	// インデックスを指定してstd::mapのイテレーターを取得する関数
	template<typename Key, typename Value>
	typename std::map<Key, Value>::iterator GetIteratorAtIndex(std::map<Key, Value>& mp, size_t index) {
		typename std::map<Key, Value>::iterator iter = mp.begin();
		std::advance(iter, index); // イテレーターをindex分進める
		return iter;
	}

	// 二つのリストをマージする関数
	template<typename T>
	std::list<T> MergeLists(const std::list<T>& list1, const std::list<T>& list2) {
		std::list<T> mergedList;
		mergedList.insert(mergedList.end(), list1.begin(), list1.end());
		mergedList.insert(mergedList.end(), list2.begin(), list2.end());
		return mergedList;
	}
};

