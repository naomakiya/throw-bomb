#include "pch.h"
#include "Graph.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

//  �A�N�Z�b�T:  �w�肳�ꂽ�C���f�b�N�X��Vertex���擾����
const Vertex& Graph::GetVertex(int vertexIndex)
{
    // �w�肳�ꂽ���_��Ԃ�
    return m_vertexList[vertexIndex];
}

// �A�N�Z�b�T:  ���_���X�g���擾����
const std::vector<Vertex>& Graph::GetVertexList()
{
    return m_vertexList;
}

//  �A�N�Z�b�T:  �אڃ��X�g���擾����
const std::vector<std::vector<int>>& Graph::GetAdjacencyList()
{
    return m_adjacencyList;
}


// �R���X�g���N�^
Graph::Graph()
    :
    m_vertexList{},
    m_adjacencyList{}
{
}


// �f�X�g���N�^
Graph::~Graph()
{
    // �O���t�f�[�^���N���A����
    Clear();
}

// �O���t�f�[�^���N���A����
void Graph::Clear()
{
    // ���_���X�g���N���A����
    m_vertexList.clear();
    // �אڃ��X�g���N���A����
    m_adjacencyList.clear();
}

// ���_�𐶐�����
bool Graph::CreateVertex(const Vertex& vertex)
{
    // ���_���X�g�ɒ��_��ǉ�����
    m_vertexList.emplace_back(vertex);
    // �אڃ��X�g�Ƀ��X�g�̒��_��ǉ�����
    m_adjacencyList.emplace_back(std::vector<int>(0, 0));
    // ����I��
    return true;
}

// ���_���X�g�̒��_�ʒu���X�P�[�����O����
void Graph::ScaleVertexPositionOfVertexList(const float& scale)
{
    for (auto it = m_vertexList.begin(); it != m_vertexList.end(); it++)
    {
        it->SetPosition(it->GetPosition() * scale);
    }
}

// ���_���X�g���璸�_���폜����
bool Graph::RemoveVertexFromVertexList(const Vertex& vertex)
{
    // ���_���X�g���璸�_���폜����
    m_vertexList.erase(
        std::remove_if(
            m_vertexList.begin(),
            m_vertexList.end(),
            [&](Vertex const& vertexIndex) { return vertexIndex.GetPosition() == vertex.GetPosition(); }),
        m_vertexList.end()
    );
    // ���_�ԍ����Đݒ肷��
    ReNumber();
    // ����I��
    return true;
}

// ���_���X�g���璸�_���폜����
bool Graph::RemoveVertexFromVertexList(const int& vertexIndex)
{
    // ���_���X�g���璸�_���폜����
    m_vertexList.erase(m_vertexList.begin() + vertexIndex);
    // ���_�ԍ����Đݒ肷��
    ReNumber();
    // ����I��
    return true;
}

// �אڃ��X�g�ɃG�b�W�̒��_��ǉ�����
bool Graph::AddVertexOfEdgeToAdjacencyList(const int& vertexNumber, const int& vertexNumberOfEdge)
{
    // �אڃ��X�g�ɃG�b�W�̒��_�ԍ���ǉ�����
    m_adjacencyList[vertexNumber].push_back(vertexNumberOfEdge);
    // ����I��
    return true;
}

// �אڃ��X�g�̃G�b�W���_���폜����
bool Graph::RemoveVertexOfEdgeFromAdjacencyList(const int& vertexNumber)
{
    // �אڃ��X�g���폜����
    m_adjacencyList[vertexNumber].clear();
    // �אڃ��X�g�ō폜���ׂ����_�ԍ������v�f���폜����
    for (int vertexIndex = 0; vertexIndex < (int)m_adjacencyList.size(); vertexIndex++)
    {
        // �אڃ��X�g���璸�_�C���f�b�N�X���폜����
        m_adjacencyList[vertexIndex].erase(
            std::remove_if(
                m_adjacencyList[vertexIndex].begin(),
                m_adjacencyList[vertexIndex].end(),
                [&](int index) {return index == vertexNumber; }
            ),
            m_adjacencyList[vertexIndex].end()
        );
    }
    // �ߗ׃��X�g�̃G�b�W�̒��_�C���f�b�N�X�𒲐�����
    for (int vertexIndex = 0; vertexIndex < (int)m_adjacencyList.size(); vertexIndex++)
    {
        for (int edgeIndex = 0; edgeIndex < (int)m_adjacencyList[vertexIndex].size(); edgeIndex++)
        {
            // �אڃ��X�g�̒��_�C���f�b�N�X���폜���钸�_�C���f�b�N�X���傫���ꍇ
            if (m_adjacencyList[vertexIndex][edgeIndex] > vertexNumber)
            {
                // ���_�C���f�b�N�X���P���炷
                m_adjacencyList[vertexIndex][edgeIndex]--;
            }
        }
    }
    // �אڃ��X�g�̔z��̗v�f���폜����
    m_adjacencyList.erase(m_adjacencyList.begin() + vertexNumber);
    // ����I��
    return true;
}

// ���_���X�g����C���f�b�N�X�Ԗڂ̒��_���폜����
void Graph::RemoveVertexFromVertexListByIndex(const int& index)
{
    // ���_���X�g�̔z��̗v�f���폜����
    m_vertexList.erase(m_vertexList.begin() + index);
}

// �אڃ��X�g����C���f�b�N�X�Ԗڂ̃G�b�W�̒��_���폜����
void Graph::RemoveVertexOfEdgeFromAdjacencyListByIndex(const int& index)
{
    // �אڃ��X�g�̔z��̗v�f���폜����
    m_adjacencyList.erase(m_adjacencyList.begin() + index);
}

// �אڃ��X�g����C���f�b�N�X�Ԗڂ̒��_�̃G�b�W�C���f�b�N�X�Ԗڂ̃G�b�W���폜����
void Graph::RemoveEgdeFromAdjacencyListByIndex(const int& index, const int& edgeIndex)
{
    // �אڃ��X�g����C���f�b�N�X�Ԗڂ̒��_�̃G�b�W�C���f�b�N�X�Ԗڂ̃G�b�W���폜����
    m_adjacencyList[index].erase(m_adjacencyList[index].begin() + edgeIndex);
}

// ���_�ԍ����Đݒ肷��
void Graph::ReNumber()
{
    for (int index = 0; index < (int)m_vertexList.size(); index++)
    {
        // ���_�ԍ����Đݒ肷��
        m_vertexList[index].SetVertexNumber(index);
    }
    // �ő咸�_�ԍ����Đݒ肷��
    Vertex::ResetMaxNumber((int)m_vertexList.size());
}

// �O���t�f�[�^��\������
void Graph::PrintGraphData()
{
    for (int vertexIndex = 0; vertexIndex < (int)m_adjacencyList.size(); vertexIndex++)
    {
        // ���_����\������
        std::cout << "VertexList: " << vertexIndex << std::endl;
        // ���_�ԍ���\������
        std::cout << "Number: " << m_vertexList[vertexIndex].GetVertexNumber() << std::endl;
        // ���_�̈ʒu��\������
        std::cout << "Position: (" << m_vertexList[vertexIndex].GetPosition().x << "," << m_vertexList[vertexIndex].GetPosition().y << ")" << std::endl;
        // �אڃ��X�g��\������
        std::cout << "AdjacencyList: ";
        for (int edgeIndex = 0; edgeIndex < (int)m_adjacencyList[vertexIndex].size(); edgeIndex++)
        {
            // �אڃ��X�g�̒��_��\������
            std::cout << m_adjacencyList[vertexIndex][edgeIndex] << " ";
        }
        std::cout << std::endl << std::endl;
    }
}

// JSON�`���ŃO���t�f�[�^��ۑ�����
bool Graph::SaveGraphDataByJSON(const std::wstring& fileName)
{
    // �o�̓t�@�C���X�g���[����錾����
    std::ofstream out;

    // �o�̓t�@�C���X�g���[�����I�[�v������
    out.open(fileName, std::ios::out);
    if (!out.is_open())
        return false;

    std::string text;
    out << "{" << std::endl;
    // ���_���X�g�̕�������o�͂���
    out << "\t\"vertex_list\": [" << std::endl;
    // ���_���X�g���o�͂���
    for (int vertexIndex = 0; vertexIndex < (int)m_adjacencyList.size(); vertexIndex++)
    {
        // ���_�ԍ��̕�������o�͂���
        out << "\t\t{\"vertex_number\": ";
        // ���_�ԍ����o�͂���
        out << m_vertexList[vertexIndex].GetVertexNumber() << ", ";
        // �ʒu�̕�������o�͂���
        out << "\"position\": ";
        out << "{";
        // X���W���o�͂���
        out << "\"x\": " << m_vertexList[vertexIndex].GetPosition().x << ", ";
        // Y���W���o�͂���
        out << "\"y\": " << m_vertexList[vertexIndex].GetPosition().y;
        out << "}";
        // ���_���X�g�̃f�[�^���p�����Ă��邩�ǂ����𒲂ׂ�
        if (vertexIndex < m_adjacencyList.size() - 1)
            // �u���b�N�ƃR���}���o�͂���
            out << "}," << std::endl;
        else
            // �u���b�N���o�͂���
            out << "}" << std::endl;
    }
    out << "\t]," << std::endl;
    // �אڃ��X�g�̕�������o�͂���
    out << "\t\"adjacency_list\": [" << std::endl;
    // �אڃ��X�g(�W���O�z��)���o�͂���
    for (int vertexIndex = 0; vertexIndex < (int)m_adjacencyList.size(); vertexIndex++)
    {
        out << "\t\t[";
        for (int edgeIndex = 0; edgeIndex < (int)m_adjacencyList[vertexIndex].size(); edgeIndex++)
        {
            // �אڃ��X�g�̒��_�f�[�^��\������
            out << m_adjacencyList[vertexIndex][edgeIndex];
            // ���_���X�g�̒��_�f�[�^���p�����Ă��邩�ǂ����𒲂ׂ�
            if (edgeIndex < m_adjacencyList[vertexIndex].size() - 1)
            {
                // ���_�f�[�^���p�����Ă���
                out << ", ";
            }
        }
        // ���_���אڃ��X�g�̍Ōォ�ǂ����𒲂ׂ�
        if(vertexIndex < m_adjacencyList.size() - 1)
            // ���_�f�[�^�͌p�����Ă���
            out << "]," << std::endl;
        else
            // ���_�f�[�^�͍Ō�̃f�[�^
            out << "]" << std::endl;
    }
    out << "\t]" << std::endl;
    out << "}" << std::endl;

    // �o�̓t�@�C���X�g���[�����N���[�Y����
    out.close();
    // ����I��
    return true;
}

// JSON�`���ŃO���t�f�[�^��ǂݍ���
bool Graph::LoadGraphDataByJSON(const std::wstring& fileName)
{
    std::stringstream ss;
    std::string graphData;

    // ���̓t�@�C���X�g���[��
    std::ifstream in(fileName, std::ifstream::in);
    // �o�̓t�@�C���X�g���[�����I�[�v������
    in.open(fileName, std::ifstream::in);
    if (!in.is_open())
        return false;

    // �X�g�����O�X�g���[��
    ss << in.rdbuf();
    // �z����N���A����
    m_graphData.clear();
    // ���s�܂ł̈�s���̕�������擾����
    while (std::getline(ss, graphData)) 
    {
        // �O���t�f�[�^����s�v�ȕ�������������
        graphData.erase(std::remove_if(
            graphData.begin(), 
            graphData.end(),
            [](char c) { 
                return (
                    c == '\r' || c == '\t' || c == ' '  || c == '\n' || c == '\"' ||
                    c == '['  ||  c == ']'  ||  c == '{'  || c == '}'
                ); 
            }),
            graphData.end()
        );
        if (!graphData.empty())
        {
            // �z��Ƀ��[�h��ǉ�����
            m_graphData.push_back(graphData);
        }
    }
    // ���̓t�@�C���X�g���[�����N���[�Y����
    in.close();
    // ����I��
    return true;
}

/// JSON�`���̃f�[�^���p�[�X���I�u�W�F�N�g�ɕϊ�����
void Graph::ParseJSON()
{
    int position;
    // JSON�f�[�^���p�[�X����
    for (int index = 0; index < (int)m_graphData.size(); index++)
    {
        int vertexNumber;
        float x;
        float y;
        // �����Fvertex_number:0,position:x:-40,y:20,
        std::stringstream ss(m_graphData[index].c_str());
        // ���_�ԍ��̕��������������
        position = (int)m_graphData[index].find("vertex_number:");
        // �����񂪌�����Ȃ��ꍇ
        if (position == std::string::npos)
            continue;
        // "vertex_number:"���폜����
        m_graphData[index].replace(position, strlen("vertex_number:"), "");
        // ",position:x:"����������
        position = (int)m_graphData[index].find(",position:x:");
        // ",position:x:"���󕶎��ɒu��������
        m_graphData[index].replace(position, strlen(",position:x:"), " ");
        // ",y:"��T������
        position = (int)m_graphData[index].find(",y:");
        // ",y:"���󕶎��ɒu��������
        m_graphData[index].replace(position, strlen(",y:"), " ");

        ss.clear();
        ss.str(m_graphData[index]);
        // ���_�̍��W���擾����
        ss >> vertexNumber >> x >> y;
        // ���_�𐶐�����
        CreateVertex(Vertex(DirectX::SimpleMath::Vector2(roundf(x), roundf(y))));
    }

    // ���_�C���f�b�N�X
    int vertexIndex = 0;
    // ���o�t���O
    bool found = false;
    for (int index = 0; index < (int)m_graphData.size(); index++)
    {
        if (!found)
        {
            // �אڃ��X�g�̕��������������
            position = (int)m_graphData[index].find("adjacency_list:");
            // �����񂪌�����Ȃ��ꍇ
            if (position == std::string::npos)
                continue;

            // �אڃ��X�g�̕����񂪌�������
            found = true;
            // �C���f�b�N�X���C���N�������g����
            if (index < (int)m_graphData.size() - 1)
                index++;
            else
                continue;
        }

        // �אڃ��X�g�̍s���̃R���}�����݂���ꍇ�͍폜����
        if (m_graphData[index].at((m_graphData[index].size() - 1)) == ',')
            m_graphData[index].erase(--m_graphData[index].end());
        
        // �X�g�����O�X�g���[���𐶐�����
        std::stringstream ss(m_graphData[index].c_str());
        // ���_�ԍ�
        std::string vertexNumber;
        // �R���}�܂ł̕�����̎擾���J��Ԃ�
        while(std::getline(ss, vertexNumber, ','))
        {
            // �G�b�W�̒��_�ԍ��𐔒l������
            int vertexIndexOfEdge = std::stoi(vertexNumber);
            // �אڃ��X�g�ɃG�b�W�̒��_��ǉ�����
            AddVertexOfEdgeToAdjacencyList(vertexIndex, vertexIndexOfEdge);
        }
        // ���_�C���f�b�N�X���C���N�������g����
        vertexIndex++;
    }
}
