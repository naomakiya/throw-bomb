#pragma once
#ifndef GRAPH_DEFINED
#define GRAPH_DEFINED
#include <memory>
#include <vector>
#include <algorithm>
#include <string>
#include "SimpleMath.h"
#include "Vertex.h"

// Graph�N���X��錾����
class Graph
{
public:
    //  �A�N�Z�b�T: �w�肳�ꂽ�C���f�b�N�X��Vertex���擾����
    const Vertex& GetVertex(int vertexIndex);
    // �A�N�Z�b�T: ���_���X�g���擾����
    const std::vector<Vertex>& GetVertexList();
    // �A�N�Z�b�T: �אڃ��X�g���擾����
    const std::vector<std::vector<int>>& GetAdjacencyList();

public:
    // �R���X�g���N�^
    Graph();
   // �f�X�g���N�^
    ~Graph();
   // �O���t�f�[�^���N���A����
   void Clear();
   // ���_�𐶐�����
   bool CreateVertex(const Vertex& vertex);
   // ���_���X�g�̒��_�ʒu���X�P�[�����O����
   void ScaleVertexPositionOfVertexList(const float& scale);
   // �אڃ��X�g�ɃG�b�W�̒��_��ǉ�����
   bool AddVertexOfEdgeToAdjacencyList(const int& vertexIndex, const int& vertexOfEdge);
   // ���_���X�g���璸�_���폜����
   void RemoveVertexFromVertexListByIndex(const int& index);
   // �אڃ��X�g����G�b�W�̒��_���폜����
   void RemoveVertexOfEdgeFromAdjacencyListByIndex(const int& index);
   // ���_���폜����
   bool RemoveVertexFromVertexList(const Vertex& vertex);
   // ���_���X�g���璸�_���폜����
   bool RemoveVertexFromVertexList(const int& vertexIndex);

   // �אڃ��X�g�̃G�b�W�̒��_���폜����
   bool RemoveVertexOfEdgeFromAdjacencyList(const int& vertexNumber);
   // �אڃ��X�g����C���f�b�N�X�Ԗڂ̒��_�̃G�b�W�C���f�b�N�X�Ԗڂ̃G�b�W���폜����
   void RemoveEgdeFromAdjacencyListByIndex(const int& index, const int& edgeIndex);
   // ���_�ԍ����Đݒ肷��
   void ReNumber();
   // �O���t�f�[�^��\������
   void PrintGraphData();
   // JSON�`���ŃO���t�f�[�^��ۑ�����
   bool SaveGraphDataByJSON(const std::wstring& fileName);
   // JSON�`���ŃO���t�f�[�^��ǂݍ���
   bool LoadGraphDataByJSON(const std::wstring& fileName);
   // �f�[�^��ϊ�����
   void ParseJSON();

private:
    // ���_���X�g
    std::vector<Vertex> m_vertexList;
    // �אڃ��X�g
    std::vector<std::vector<int>> m_adjacencyList;
    // ���̓O���t�f�[�^
    std::vector<std::string> m_graphData;
};

#endif	// GRAPH_DEFINED
