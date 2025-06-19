/*
	@file	CollisionMesh.h
	@brief	Obj形式のメッシュをコリジョンにするクラス
*/
#pragma once

namespace mylib
{
	class CollisionMesh  
	{
	public:
		//坂の種類
		enum class CollisionMeshType {
			NONE = 0,		//	存在しない
			stairs1 = 1,	// 1段目の坂
			stairs2 = 2     // 2段目の坂
		};
	private:
		// 三角形ポリゴンを表現する構造体
		struct Triangle
		{
			DirectX::SimpleMath::Vector3 triangle[3];	// 頂点座標
			DirectX::SimpleMath::Vector3 normal;		// 法線

			Triangle(
				DirectX::SimpleMath::Vector3 a,
				DirectX::SimpleMath::Vector3 b,
				DirectX::SimpleMath::Vector3 c
			)
			{
				// ポリゴン
				triangle[0] = a;
				triangle[1] = b;
				triangle[2] = c;

				// 法線
				DirectX::SimpleMath::Vector3 ab = triangle[1] - triangle[0];
				DirectX::SimpleMath::Vector3 ac = triangle[2] - triangle[0];
				normal = ab.Cross(ac);	// 右手座標系でポリゴンの頂点が反時計回りの時

				// 注意！
				// normal = ab.Cross(ac) * (-1);	// 右手座標系でポリゴンの頂点が時計回りの時
				// objデータの頂点情報は反時計回りで定義されている
			}
		};


	private:
		// 座標、回転、頂点情報配列、頂点インデックス配列、ポリゴン配列
		DirectX::SimpleMath::Vector3 m_position;
		DirectX::SimpleMath::Quaternion m_rotation;
		std::vector<DirectX::VertexPosition> m_vertices;
		std::vector<uint16_t> m_indices;
		std::vector<Triangle> m_triangles;

		// メッシュ描画用オブジェクト
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
		std::unique_ptr<DirectX::BasicEffect> m_basicEffect;
		
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPosition>> m_primitiveBatch;

		// モデル
		std::shared_ptr<DirectX::Model> m_model;

	public:
		CollisionMesh(DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Quaternion rotation);
		~CollisionMesh() = default;

		// メッシュオブジェクトを作成する
		void Initialize(
			ID3D11Device* device,
			const std::wstring& objName,
			int typeValue
		);

		// メッシュデータをワイヤーフレームで描画する
		void Draw(
			ID3D11DeviceContext* context,
			DirectX::CommonStates* states,
			const DirectX::SimpleMath::Matrix& view,
			const DirectX::SimpleMath::Matrix& projection
		);

		// メッシュとRayとの衝突判定
		bool IntersectRay(
			const DirectX::SimpleMath::Ray& ray,
			DirectX::SimpleMath::Vector3* hitPosition,
			DirectX::SimpleMath::Vector3* normal,
			int* polygonNo
		);

	private:
		// Objファイルをロードする
		void LoadObjFile(const std::wstring& objName);

		// ポリゴンを作り配列に登録する
		void AddTriangle(
			DirectX::SimpleMath::Vector3 a,
			DirectX::SimpleMath::Vector3 b,
			DirectX::SimpleMath::Vector3 c
		);

	public:
		// 衝突したポリゴンの頂点情報を返す
		std::vector<DirectX::SimpleMath::Vector3> GetHitPolygon(unsigned int hitPolygonNo);
	};
}
