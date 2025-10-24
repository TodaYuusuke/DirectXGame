#pragma once
#include <list>
#include <map>

namespace LWP::Utility {
	/// <summary>
	/// インデックスを指定してstd::listのイテレーターを取得する関数
	/// </summary>
	/// <param name="list">配列</param>
	/// <param name="index">何番目か</param>
	/// <returns>イテレータ</returns>
	template<typename T>
	typename std::list<T>::iterator GetIteratorAtIndex(std::list<T>& list, size_t index) {
		typename std::list<T>::iterator iter = list.begin();
		std::advance(iter, index); // イテレーターをindex分進める
		return iter;
	}
	/// <summary>
	/// インデックスを指定してstd::mapのイテレーターを取得する関数
	/// </summary>
	/// <param name="lst">配列</param>
	/// <param name="index">何番目か</param>
	/// <returns>イテレータ</returns>
	template<typename Key, typename Value>
	typename std::map<Key, Value>::iterator GetIteratorAtIndex(std::map<Key, Value>& mp, size_t index) {
		typename std::map<Key, Value>::iterator iter = mp.begin();
		std::advance(iter, index); // イテレーターをindex分進める
		return iter;
	}

	// 二つのリストをマージする関数list
	template<typename T>
	std::list<T> MergeLists(const std::list<T>& list1, const std::list<T>& list2) {
		std::list<T> mergedList;
		mergedList.insert(mergedList.end(), list1.begin(), list1.end());
		mergedList.insert(mergedList.end(), list2.begin(), list2.end());
		return mergedList;
	}
}