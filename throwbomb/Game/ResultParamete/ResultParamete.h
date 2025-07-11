/*
    @file	ResultParamete.h
    @brief	リザルトパラメータークラス
*/
#pragma once
#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>
#include <fstream>
#include <mutex>

class ResultParamete
{
public:
    //　パラメータの確保
    float GetScore() { return m_score; }
    // 残りの敵の数
    int RemainingNumber(){ return m_remainingNumber; }
    //パラメータの保存
    void SetResultParamete(int defeatdEnemies,int sumEnemy,float time){
        m_score = (static_cast<int>(time) + defeatdEnemies) * ((static_cast<float>(defeatdEnemies + 1) / sumEnemy) * 100);

    }
    // ゲームオーバーパラメータの保存
    void SetGameOverResultParamete(int remainingNumber,int sumEnemy,float time) {
        m_remainingNumber = remainingNumber;
        m_sumEnemy = sumEnemy;
        m_time = time;
    }
    // ゲームオーバー時の残りの敵の数の取得
    void SetGameOverScoreResult(int& remainingNumber,int &sumEnemy) {
        remainingNumber = m_remainingNumber;
        sumEnemy = m_sumEnemy;
    }
public:
    // コンストラクタ
    ResultParamete();
    // デストラクタ
    ~ResultParamete();

private:
    // 読み取る
    void LoadResultParamete();

private:
    // 時間
    float m_time;
    //スコアの保存    
    float m_score;
    // 残りの敵の数
    int m_remainingNumber;
    // 倒した数
    int  m_defeatdEnemies;
    // 突進する敵の残り
    int m_straighteningEnemy;
    // 索敵する敵の残り
    int m_patorlEnemy;
    // 敵の数
    int m_sumEnemy;
    // ゴールしているか
    bool m_isgoal;
};