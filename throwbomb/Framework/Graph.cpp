#include "pch.h"
#include "Graph.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

//  アクセッサ:  指定されたインデックスのVertexを取得する
const Vertex& Graph::GetVertex(int vertexIndex)
{
    // 指定された頂点を返す
    return m_vertexList[vertexIndex];
}

// アクセッサ:  頂点リストを取得する
const std::vector<Vertex>& Graph::GetVertexList()
{
    return m_vertexList;
}

//  アクセッサ:  隣接リストを取得する
const std::vector<std::vector<int>>& Graph::GetAdjacencyList()
{
    return m_adjacencyList;
}


// コンストラクタ
Graph::Graph()
    :
    m_vertexList{},
    m_adjacencyList{}
{
}


// デストラクタ
Graph::~Graph()
{
    // グラフデータをクリアする
    Clear();
}

// グラフデータをクリアする
void Graph::Clear()
{
    // 頂点リストをクリアする
    m_vertexList.clear();
    // 隣接リストをクリアする
    m_adjacencyList.clear();
}

// 頂点を生成する
bool Graph::CreateVertex(const Vertex& vertex)
{
    // 頂点リストに頂点を追加する
    m_vertexList.emplace_back(vertex);
    // 隣接リストにリストの頂点を追加する
    m_adjacencyList.emplace_back(std::vector<int>(0, 0));
    // 正常終了
    return true;
}

// 頂点リストの頂点位置をスケーリングする
void Graph::ScaleVertexPositionOfVertexList(const float& scale)
{
    for (auto it = m_vertexList.begin(); it != m_vertexList.end(); it++)
    {
        it->SetPosition(it->GetPosition() * scale);
    }
}

// 頂点リストから頂点を削除する
bool Graph::RemoveVertexFromVertexList(const Vertex& vertex)
{
    // 頂点リストから頂点を削除する
    m_vertexList.erase(
        std::remove_if(
            m_vertexList.begin(),
            m_vertexList.end(),
            [&](Vertex const& vertexIndex) { return vertexIndex.GetPosition() == vertex.GetPosition(); }),
        m_vertexList.end()
    );
    // 頂点番号を再設定する
    ReNumber();
    // 正常終了
    return true;
}

// 頂点リストから頂点を削除する
bool Graph::RemoveVertexFromVertexList(const int& vertexIndex)
{
    // 頂点リストから頂点を削除する
    m_vertexList.erase(m_vertexList.begin() + vertexIndex);
    // 頂点番号を再設定する
    ReNumber();
    // 正常終了
    return true;
}

// 隣接リストにエッジの頂点を追加する
bool Graph::AddVertexOfEdgeToAdjacencyList(const int& vertexNumber, const int& vertexNumberOfEdge)
{
    // 隣接リストにエッジの頂点番号を追加する
    m_adjacencyList[vertexNumber].push_back(vertexNumberOfEdge);
    // 正常終了
    return true;
}

// 隣接リストのエッジ頂点を削除する
bool Graph::RemoveVertexOfEdgeFromAdjacencyList(const int& vertexNumber)
{
    // 隣接リストを削除する
    m_adjacencyList[vertexNumber].clear();
    // 隣接リストで削除すべき頂点番号を持つ要素を削除する
    for (int vertexIndex = 0; vertexIndex < (int)m_adjacencyList.size(); vertexIndex++)
    {
        // 隣接リストから頂点インデックスを削除する
        m_adjacencyList[vertexIndex].erase(
            std::remove_if(
                m_adjacencyList[vertexIndex].begin(),
                m_adjacencyList[vertexIndex].end(),
                [&](int index) {return index == vertexNumber; }
            ),
            m_adjacencyList[vertexIndex].end()
        );
    }
    // 近隣リストのエッジの頂点インデックスを調整する
    for (int vertexIndex = 0; vertexIndex < (int)m_adjacencyList.size(); vertexIndex++)
    {
        for (int edgeIndex = 0; edgeIndex < (int)m_adjacencyList[vertexIndex].size(); edgeIndex++)
        {
            // 隣接リストの頂点インデックスが削除する頂点インデックスより大きい場合
            if (m_adjacencyList[vertexIndex][edgeIndex] > vertexNumber)
            {
                // 頂点インデックスを１減らす
                m_adjacencyList[vertexIndex][edgeIndex]--;
            }
        }
    }
    // 隣接リストの配列の要素を削除する
    m_adjacencyList.erase(m_adjacencyList.begin() + vertexNumber);
    // 正常終了
    return true;
}

// 頂点リストからインデックス番目の頂点を削除する
void Graph::RemoveVertexFromVertexListByIndex(const int& index)
{
    // 頂点リストの配列の要素を削除する
    m_vertexList.erase(m_vertexList.begin() + index);
}

// 隣接リストからインデックス番目のエッジの頂点を削除する
void Graph::RemoveVertexOfEdgeFromAdjacencyListByIndex(const int& index)
{
    // 隣接リストの配列の要素を削除する
    m_adjacencyList.erase(m_adjacencyList.begin() + index);
}

// 隣接リストからインデックス番目の頂点のエッジインデックス番目のエッジを削除する
void Graph::RemoveEgdeFromAdjacencyListByIndex(const int& index, const int& edgeIndex)
{
    // 隣接リストからインデックス番目の頂点のエッジインデックス番目のエッジを削除する
    m_adjacencyList[index].erase(m_adjacencyList[index].begin() + edgeIndex);
}

// 頂点番号を再設定する
void Graph::ReNumber()
{
    for (int index = 0; index < (int)m_vertexList.size(); index++)
    {
        // 頂点番号を再設定する
        m_vertexList[index].SetVertexNumber(index);
    }
    // 最大頂点番号を再設定する
    Vertex::ResetMaxNumber((int)m_vertexList.size());
}

// グラフデータを表示する
void Graph::PrintGraphData()
{
    for (int vertexIndex = 0; vertexIndex < (int)m_adjacencyList.size(); vertexIndex++)
    {
        // 頂点情報を表示する
        std::cout << "VertexList: " << vertexIndex << std::endl;
        // 頂点番号を表示する
        std::cout << "Number: " << m_vertexList[vertexIndex].GetVertexNumber() << std::endl;
        // 頂点の位置を表示する
        std::cout << "Position: (" << m_vertexList[vertexIndex].GetPosition().x << "," << m_vertexList[vertexIndex].GetPosition().y << ")" << std::endl;
        // 隣接リストを表示する
        std::cout << "AdjacencyList: ";
        for (int edgeIndex = 0; edgeIndex < (int)m_adjacencyList[vertexIndex].size(); edgeIndex++)
        {
            // 隣接リストの頂点を表示する
            std::cout << m_adjacencyList[vertexIndex][edgeIndex] << " ";
        }
        std::cout << std::endl << std::endl;
    }
}

// JSON形式でグラフデータを保存する
bool Graph::SaveGraphDataByJSON(const std::wstring& fileName)
{
    // 出力ファイルストリームを宣言する
    std::ofstream out;

    // 出力ファイルストリームをオープンする
    out.open(fileName, std::ios::out);
    if (!out.is_open())
        return false;

    std::string text;
    out << "{" << std::endl;
    // 頂点リストの文字列を出力する
    out << "\t\"vertex_list\": [" << std::endl;
    // 頂点リストを出力する
    for (int vertexIndex = 0; vertexIndex < (int)m_adjacencyList.size(); vertexIndex++)
    {
        // 頂点番号の文字列を出力する
        out << "\t\t{\"vertex_number\": ";
        // 頂点番号を出力する
        out << m_vertexList[vertexIndex].GetVertexNumber() << ", ";
        // 位置の文字列を出力する
        out << "\"position\": ";
        out << "{";
        // X座標を出力する
        out << "\"x\": " << m_vertexList[vertexIndex].GetPosition().x << ", ";
        // Y座標を出力する
        out << "\"y\": " << m_vertexList[vertexIndex].GetPosition().y;
        out << "}";
        // 頂点リストのデータが継続しているかどうかを調べる
        if (vertexIndex < m_adjacencyList.size() - 1)
            // ブロックとコンマを出力する
            out << "}," << std::endl;
        else
            // ブロックを出力する
            out << "}" << std::endl;
    }
    out << "\t]," << std::endl;
    // 隣接リストの文字列を出力する
    out << "\t\"adjacency_list\": [" << std::endl;
    // 隣接リスト(ジャグ配列)を出力する
    for (int vertexIndex = 0; vertexIndex < (int)m_adjacencyList.size(); vertexIndex++)
    {
        out << "\t\t[";
        for (int edgeIndex = 0; edgeIndex < (int)m_adjacencyList[vertexIndex].size(); edgeIndex++)
        {
            // 隣接リストの頂点データを表示する
            out << m_adjacencyList[vertexIndex][edgeIndex];
            // 頂点リストの頂点データが継続しているかどうかを調べる
            if (edgeIndex < m_adjacencyList[vertexIndex].size() - 1)
            {
                // 頂点データが継続している
                out << ", ";
            }
        }
        // 頂点が隣接リストの最後かどうかを調べる
        if(vertexIndex < m_adjacencyList.size() - 1)
            // 頂点データは継続している
            out << "]," << std::endl;
        else
            // 頂点データは最後のデータ
            out << "]" << std::endl;
    }
    out << "\t]" << std::endl;
    out << "}" << std::endl;

    // 出力ファイルストリームをクローズする
    out.close();
    // 正常終了
    return true;
}

// JSON形式でグラフデータを読み込む
bool Graph::LoadGraphDataByJSON(const std::wstring& fileName)
{
    std::stringstream ss;
    std::string graphData;

    // 入力ファイルストリーム
    std::ifstream in(fileName, std::ifstream::in);
    // 出力ファイルストリームをオープンする
    in.open(fileName, std::ifstream::in);
    if (!in.is_open())
        return false;

    // ストリングストリーム
    ss << in.rdbuf();
    // 配列をクリアする
    m_graphData.clear();
    // 改行までの一行分の文字列を取得する
    while (std::getline(ss, graphData)) 
    {
        // グラフデータから不要な文字を消去する
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
            // 配列にワードを追加する
            m_graphData.push_back(graphData);
        }
    }
    // 入力ファイルストリームをクローズする
    in.close();
    // 正常終了
    return true;
}

/// JSON形式のデータをパースしオブジェクトに変換する
void Graph::ParseJSON()
{
    int position;
    // JSONデータをパースする
    for (int index = 0; index < (int)m_graphData.size(); index++)
    {
        int vertexNumber;
        float x;
        float y;
        // 書式：vertex_number:0,position:x:-40,y:20,
        std::stringstream ss(m_graphData[index].c_str());
        // 頂点番号の文字列を検索する
        position = (int)m_graphData[index].find("vertex_number:");
        // 文字列が見つからない場合
        if (position == std::string::npos)
            continue;
        // "vertex_number:"を削除する
        m_graphData[index].replace(position, strlen("vertex_number:"), "");
        // ",position:x:"を検索する
        position = (int)m_graphData[index].find(",position:x:");
        // ",position:x:"を空文字に置き換える
        m_graphData[index].replace(position, strlen(",position:x:"), " ");
        // ",y:"を探索する
        position = (int)m_graphData[index].find(",y:");
        // ",y:"を空文字に置き換える
        m_graphData[index].replace(position, strlen(",y:"), " ");

        ss.clear();
        ss.str(m_graphData[index]);
        // 頂点の座標を取得する
        ss >> vertexNumber >> x >> y;
        // 頂点を生成する
        CreateVertex(Vertex(DirectX::SimpleMath::Vector2(roundf(x), roundf(y))));
    }

    // 頂点インデックス
    int vertexIndex = 0;
    // 検出フラグ
    bool found = false;
    for (int index = 0; index < (int)m_graphData.size(); index++)
    {
        if (!found)
        {
            // 隣接リストの文字列を検索する
            position = (int)m_graphData[index].find("adjacency_list:");
            // 文字列が見つからない場合
            if (position == std::string::npos)
                continue;

            // 隣接リストの文字列が見つかった
            found = true;
            // インデックスをインクリメントする
            if (index < (int)m_graphData.size() - 1)
                index++;
            else
                continue;
        }

        // 隣接リストの行末のコンマが存在する場合は削除する
        if (m_graphData[index].at((m_graphData[index].size() - 1)) == ',')
            m_graphData[index].erase(--m_graphData[index].end());
        
        // ストリングストリームを生成する
        std::stringstream ss(m_graphData[index].c_str());
        // 頂点番号
        std::string vertexNumber;
        // コンマまでの文字列の取得を繰り返す
        while(std::getline(ss, vertexNumber, ','))
        {
            // エッジの頂点番号を数値化する
            int vertexIndexOfEdge = std::stoi(vertexNumber);
            // 隣接リストにエッジの頂点を追加する
            AddVertexOfEdgeToAdjacencyList(vertexIndex, vertexIndexOfEdge);
        }
        // 頂点インデックスをインクリメントする
        vertexIndex++;
    }
}
