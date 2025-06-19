/*
   @file IPatrolStrategy.h
   @brief パトロールのインターフェイスクラス
*/
#pragma once
#ifndef IPATROLSTRATEGY_DEFINED
#define IPATROLSTRATEGY_DEFINED
#include "pch.h"

class IPatrolStrategy
{
public:
    virtual ~IPatrolStrategy() = default;
    virtual DirectX::SimpleMath::Vector3 GetNextTarget(const std::vector<DirectX::SimpleMath::Vector3>& path, int& currentIndex) = 0;
};

#endif