/*
	@file	Utils.h
	@brief	�e���v���[�g�N���X�N���X
*/
#pragma once

namespace mylib
{
    // �ėpClamp
    template <typename T>
    T Clamp(T value, T minVal, T maxVal)
    {
        return std::max(minVal, std::min(value, maxVal));
    }
    
}
