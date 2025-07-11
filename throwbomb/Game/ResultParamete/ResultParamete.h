/*
    @file	ResultParamete.h
    @brief	���U���g�p�����[�^�[�N���X
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
    //�@�p�����[�^�̊m��
    float GetScore() { return m_score; }
    // �c��̓G�̐�
    int RemainingNumber(){ return m_remainingNumber; }
    //�p�����[�^�̕ۑ�
    void SetResultParamete(int defeatdEnemies,int sumEnemy,float time){
        m_score = (static_cast<int>(time) + defeatdEnemies) * ((static_cast<float>(defeatdEnemies + 1) / sumEnemy) * 100);

    }
    // �Q�[���I�[�o�[�p�����[�^�̕ۑ�
    void SetGameOverResultParamete(int remainingNumber,int sumEnemy,float time) {
        m_remainingNumber = remainingNumber;
        m_sumEnemy = sumEnemy;
        m_time = time;
    }
    // �Q�[���I�[�o�[���̎c��̓G�̐��̎擾
    void SetGameOverScoreResult(int& remainingNumber,int &sumEnemy) {
        remainingNumber = m_remainingNumber;
        sumEnemy = m_sumEnemy;
    }
public:
    // �R���X�g���N�^
    ResultParamete();
    // �f�X�g���N�^
    ~ResultParamete();

private:
    // �ǂݎ��
    void LoadResultParamete();

private:
    // ����
    float m_time;
    //�X�R�A�̕ۑ�    
    float m_score;
    // �c��̓G�̐�
    int m_remainingNumber;
    // �|������
    int  m_defeatdEnemies;
    // �ːi����G�̎c��
    int m_straighteningEnemy;
    // ���G����G�̎c��
    int m_patorlEnemy;
    // �G�̐�
    int m_sumEnemy;
    // �S�[�����Ă��邩
    bool m_isgoal;
};