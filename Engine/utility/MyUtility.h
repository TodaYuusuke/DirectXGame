#pragma once
#include <string>
#include <random>

namespace LWP::Utility {

	/// <summary>
	/// ログの表示
	/// </summary>
	void Log(const std::string& message);


	/// <summary>
	/// string -> wstringへの変換
	/// </summary>
	std::wstring ConvertString(const std::string& str);

	/// <summary>
	/// wstring -> stringへの変換
	/// </summary>
	std::string ConvertString(const std::wstring& str);


    /// <summary>
    /// パスを１つ上のディレクトリパスのパスに変換する関数
    /// </summary>
    /// <param name="filePath"></param>
    /// <returns></returns>
	std::string ConvertToParentDirectory(const std::string& filePath);

    
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

        return sum / static_cast<T>(n);
    }

    /// <summary>
    /// 任意の型の最小値から最大値の間の値をランダムに生成する関数
    /// </summary>
    /// <typeparam name="T">任意の型</typeparam>
    /// <param name="minValue">最小値</param>
    /// <param name="maxValue">最大値</param>
    /// <returns>最小値と最大値の間のランダムな値</returns>
    template <typename T>
    T GenerateRandamNum(const T& minValue, const T& maxValue) {
        // 使用する乱数生成器
        std::random_device rd;
        std::mt19937 generator(rd());

        // 型Tに応じた分布
        std::uniform_int_distribution<T> distribution(minValue, maxValue);

        // 乱数を生成して返す
        return distribution(generator);
    }


    /// <summary>
    /// radian から degreeへ
    /// </summary>
    int RadianToDegree(float radian);

    /// <summary>
    /// degree から radian へ
    /// </summary>
    float DegreeToRadian(int degree);
    
};

