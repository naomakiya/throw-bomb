#pragma once
#ifndef GRAPH_DEFINED
#define GRAPH_DEFINED
#include <memory>
#include <vector>
#include <algorithm>
#include <string>
#include "SimpleMath.h"
#include "Vertex.h"

// Graphクラスを宣言する
class Graph
{
public:
    //  アクセッサ: 指定されたインデックスのVertexを取得する
    const Vertex& GetVertex(int vertexIndex);
    // アクセッサ: 頂点リストを取得する
    const std::vector<Vertex>& GetVertexList();
    // アクセッサ: 隣接リストを取得する
    const std::vector<std::vector<int>>& GetAdjacencyList();

public:
    // コンストラクタ
    Graph();
   // デストラクタ
    ~Graph();
   // グラフデータをクリアする
   void Clear();
   // 頂点を生成する
   bool CreateVertex(const Vertex& vertex);
   // 頂点リストの頂点位置をスケーリングする
   void ScaleVertexPositionOfVertexList(const float& scale);
   // 隣接リストにエッジの頂点を追加する
   bool AddVertexOfEdgeToAdjacencyList(const int& vertexIndex, const int& vertexOfEdge);
   // 頂点リストから頂点を削除する
   void RemoveVertexFromVertexListByIndex(const int& index);
   // 隣接リストからエッジの頂点を削除する
   void RemoveVertexOfEdgeFromAdjacencyListByIndex(const int& index);
   // 頂点を削除する
   bool RemoveVertexFromVertexList(const Vertex& vertex);
   // 頂点リストから頂点を削除する
   bool RemoveVertexFromVertexList(const int& vertexIndex);

   // 隣接リストのエッジの頂点を削除する
   bool RemoveVertexOfEdgeFromAdjacencyList(const int& vertexNumber);
   // 隣接リストからインデックス番目の頂点のエッジインデックス番目のエッジを削除する
   void RemoveEgdeFromAdjacencyListByIndex(const int& index, const int& edgeIndex);
   // 頂点番号を再設定する
   void ReNumber();
   // グラフデータを表示する
   void PrintGraphData();
   // JSON形式でグラフデータを保存する
   bool SaveGraphDataByJSON(const std::wstring& fileName);
   // JSON形式でグラフデータを読み込む
   bool LoadGraphDataByJSON(const std::wstring& fileName);
   // データを変換する
   void ParseJSON();

private:
    // 頂点リスト
    std::vector<Vertex> m_vertexList;
    // 隣接リスト
    std::vector<std::vector<int>> m_adjacencyList;
    // 入力グラフデータ
    std::vector<std::string> m_graphData;
};

#endif	// GRAPH_DEFINED
