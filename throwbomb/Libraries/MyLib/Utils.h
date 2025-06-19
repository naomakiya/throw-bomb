/*
	@file	Utils.h
	@brief	テンプレートクラスクラス
*/
#pragma once

namespace mylib
{
    // 汎用Clamp
    template <typename T>
    T Clamp(T value, T minVal, T maxVal)
    {
        return std::max(minVal, std::min(value, maxVal));
    }
    
}
