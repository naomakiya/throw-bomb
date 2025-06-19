/*
  @file　AvoidObjectPatrol.h
  @brief 回避するためのパトロール
*/
#pragma once
#ifndef AVOIDOBJECTPATROL_DEFINED
#define AVOIDOBJECTPATROL_DEFINED
#include"Interface/IPatrolStrategy.h"

class AvoidObjectPatrol : public IPatrolStrategy 
{
private:
    DirectX::SimpleMath::Vector3 m_avoidPosition;
    float m_avoidRadius;

public:
    AvoidObjectPatrol(const DirectX::SimpleMath::Vector3& avoidPos, float radius)
        : m_avoidPosition(avoidPos), m_avoidRadius(radius) {}

    DirectX::SimpleMath::Vector3 GetNextTarget(const std::vector<DirectX::SimpleMath::Vector3>& path, int& currentIndex) override {
        // 単純な「遠ざかる」実装
        float maxDist = 0.0f;
        int bestIndex = currentIndex;

        for (int i = 0; i < path.size(); ++i) {
            float dist = (path[i] - m_avoidPosition).Length();
            if (dist > m_avoidRadius && dist > maxDist) {
                maxDist = dist;
                bestIndex = i;
            }
        }

        currentIndex = bestIndex;
        return path[bestIndex];
    }
};

#endif // ENEMY_DEFINED