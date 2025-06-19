/*
	@file	CollisionMesh.cpp
	@brief	Obj形式のメッシュをコリジョンにするクラス
*/
#include "pch.h"
#include "CollisionMesh.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <Game/ResourceManager/ResourceManager.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

//-----------------------------------------------------
// コンストラクタ
//-----------------------------------------------------
mylib::CollisionMesh::CollisionMesh(DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Quaternion rotation)
	:
	m_position{ pos },
	m_rotation{ rotation },
	m_vertices{},
	m_indices{},
	m_triangles{},
	m_inputLayout{},
	m_basicEffect{},
	m_primitiveBatch{},
	m_model{  }
{
}

//-----------------------------------------------------
// 初期化する
//-----------------------------------------------------
void mylib::CollisionMesh::Initialize(
	ID3D11Device* device,
	const std::wstring& objName,
	int typeValue
)
{
	assert(device);
	assert(context);

	// Objファイルを読み込む
	this->LoadObjFile(objName);

	// ポリゴン配列を作成する
	for (size_t i = 0; i < m_indices.size() / 3; i++)
	{
		this->AddTriangle(
			m_vertices[m_indices[i * 3 + 0]].position,
			m_vertices[m_indices[i * 3 + 1]].position,
			m_vertices[m_indices[i * 3 + 2]].position
		);
	}
	//種類によってオブジェクトの読み込みを変更
	if (1 == typeValue)
	{
		//読み込み準備
		std::unique_ptr<DirectX::EffectFactory> fx = std::make_unique<DirectX::EffectFactory>(device);
		fx->SetDirectory(L"Resources/Models");
		//壁モデルの読み取り
		std::shared_ptr<DirectX::Model> CollisionMeshsModel = DirectX::Model::CreateFromCMO(device, ResourceManager::getModelPath("stairs1").c_str(), *fx);
		if (CollisionMeshsModel == nullptr)
		{
			assert(CollisionMeshsModel);
		}

		m_model = CollisionMeshsModel;
	}
	else if (2 == typeValue)
	{
		//壁モデルの読み取り
		std::unique_ptr<DirectX::EffectFactory> fx = std::make_unique<DirectX::EffectFactory>(device);
		fx->SetDirectory(L"Resources/Models");
		std::shared_ptr<DirectX::Model> second = DirectX::Model::CreateFromCMO(device, ResourceManager::getModelPath("stairs2").c_str(), *fx);
	
		m_model = second;
	}
}

//-----------------------------------------------------
// 描画する
//-----------------------------------------------------
void mylib::CollisionMesh::Draw(
	ID3D11DeviceContext* context,
	DirectX::CommonStates* states,
	const DirectX::SimpleMath::Matrix& view,
	const DirectX::SimpleMath::Matrix& projection
)
{
	assert(context);
	assert(states);

	// ワールド行列を計算する
	m_rotation.Normalize();

	// 回転の補正を行う (逆回転の修正)
	Matrix rotation = Matrix::CreateFromQuaternion(m_rotation);
	Matrix inverseRotation = Matrix::CreateFromAxisAngle(Vector3::UnitY, XMConvertToRadians(180.0f)) * rotation;

	// 位置とスケールを適用
	Matrix translation = Matrix::CreateTranslation(m_position);
	Matrix world = Matrix::CreateScale(Vector3(0.05f, 0.1f, 0.10f)) * inverseRotation * translation;

	// モデルを表示する
  	m_model->Draw(context, *states, world, view, projection);

	//坂の.objの描画するための処理
	 
	//// デバイスコンテキストのパラメータを設定する
	//context->OMSetBlendState(states->AlphaBlend(), nullptr, 0xFFFFFFFF);
	//context->OMSetDepthStencilState(states->DepthRead(), 0);	// ステンシルバッファーへ書き込まない
	//context->RSSetState(states->Wireframe());
	//context->IASetInputLayout(m_inputLayout.Get());
	 
	//// ベーシックエフェクトのパラメータを設定する
	//m_basicEffect->SetWorld(world);
	//m_basicEffect->SetView(view);
	//m_basicEffect->SetProjection(projection);
	//m_basicEffect->Apply(context);

	//// メッシュを描画する
	//m_primitiveBatch->Begin();
	//m_primitiveBatch->DrawIndexed(
	//	D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	//	&m_indices[0], m_indices.size(),
	//	&m_vertices[0], m_vertices.size()
	//);
	//m_primitiveBatch->End();
}

//-----------------------------------------------------
// メッシュとRayとの衝突判定 
//-----------------------------------------------------
//bool mylib::CollisionMesh::IntersectRay(
//	const DirectX::SimpleMath::Ray& ray,        // レイ
//	DirectX::SimpleMath::Vector3* hitPosition,  // 衝突座標
//	DirectX::SimpleMath::Vector3* normal,       // 衝突したポリゴンの法線
//	int* polygonNo                              // 衝突したポリゴンの番号
//)
//{
//	assert(hitPosition);
//	assert(normal);
//
//	// コリジョンメッシュの回転や座標情報から、worldの逆行列を求める
//	Matrix rotation = Matrix::CreateFromQuaternion(m_rotation);
//	Matrix transration = Matrix::CreateTranslation(m_position);
//	Matrix world = rotation * transration;
//	Matrix invertWorld = world.Invert();
//
//	// レイをローカル座標系に変換
//	Ray unitRay{};
//	unitRay.position = Vector3::Transform(ray.position, invertWorld);
//	unitRay.direction = Vector3::TransformNormal(ray.direction, invertWorld);
//	unitRay.direction.Normalize();
//
//	float distance = 0.0f;
//	bool hitDetected = false;
//	float closestDistance = FLT_MAX;
//
//	for (int i = 0; i < m_triangles.size(); i++)
//	{
//		Vector3 triangleNormal = m_triangles[i].normal;
//		triangleNormal.Normalize();
//
//		// ★ 45度制限を削除 ★
//		// float cosTheta = triangleNormal.Dot(-unitRay.direction);
//		// if (cosTheta < 0.707f) continue;
//
//		// レイとポリゴンの衝突判定
//		if (unitRay.Intersects(
//			m_triangles[i].triangle[0],
//			m_triangles[i].triangle[1],
//			m_triangles[i].triangle[2],
//			distance))
//		{
//			if (distance < closestDistance) // 最も近い衝突点を優先
//			{
//				closestDistance = distance;
//				*hitPosition = Vector3{ unitRay.position + unitRay.direction * distance };
//				*hitPosition = Vector3::Transform(*hitPosition, world);
//
//				*normal = Vector3::TransformNormal(m_triangles[i].normal, rotation);
//				//*normal = normal->Normalize();
//
//				*polygonNo = i;
//				hitDetected = true;
//			}
//		}
//	}
//
//	return hitDetected;
//}

bool mylib::CollisionMesh::IntersectRay(
	const DirectX::SimpleMath::Ray& ray,        // レイ
	DirectX::SimpleMath::Vector3* hitPosition,  // 衝突座標
	DirectX::SimpleMath::Vector3* normal,       // 衝突したポリゴンの法線
	int* polygonNo                              // 衝突したポリゴンの番号
)
{
	assert(hitPosition);
	assert(normal);

	// コリジョンメッシュの回転や座標情報から、worldの逆行列を求める
	Matrix rotation = Matrix::CreateFromQuaternion(m_rotation);
	Matrix transration = Matrix::CreateTranslation(m_position);
	Matrix world = rotation * transration;
	Matrix invertWorld = world.Invert();

	// レイをローカル座標系に変換
	Ray unitRay{};
	unitRay.position = Vector3::Transform(ray.position, invertWorld);
	unitRay.direction = Vector3::TransformNormal(ray.direction, invertWorld);
	unitRay.direction.Normalize();

	float distance = 0.0f;
	bool hitDetected = false;
	float closestDistance = FLT_MAX;

	// 1. 三角形ポリゴンとの衝突判定
	for (int i = 0; i < m_triangles.size(); i++)
	{
		Vector3 triangleNormal = m_triangles[i].normal;
		triangleNormal.Normalize();

		// レイとポリゴンの衝突判定
		if (unitRay.Intersects(
			m_triangles[i].triangle[0],
			m_triangles[i].triangle[1],
			m_triangles[i].triangle[2],
			distance))
		{
			if (distance < closestDistance) // 最も近い衝突点を優先
			{
				closestDistance = distance;
				*hitPosition = Vector3{ unitRay.position + unitRay.direction * distance };
				*hitPosition = Vector3::Transform(*hitPosition, world);

				// 法線の回転
				*normal = Vector3::TransformNormal(triangleNormal, rotation); 
				*polygonNo = i;
				hitDetected = true;
			}
		}
	}

	// 2. エッジに沿った補完レイを使った衝突判定
	//for (int i = 0; i < m_triangles.size(); i++)
	//{
	//	const auto& triangle = m_triangles[i];
	//	// 三角形の頂点情報から法線を計算
	//	Vector3 A = triangle.triangle[0];
	//	Vector3 B = triangle.triangle[1];
	//	Vector3 C = triangle.triangle[2];
	//	// 外積で法線を計算
	//	Vector3 edgeAB = B - A;
	//	Vector3 edgeAC = C - A;
	//	Vector3 triangleNormal = edgeAB.Cross(edgeAC); // インスタンスでの呼び出し
	//	triangleNormal.Normalize();  // 法線を正規化
	//	// 各辺（エッジ）の中点を補完点として使う
	//	for (int j = 0; j < 3; j++)
	//	{
	//		Vector3 edgeStart = triangle.triangle[j];
	//		Vector3 edgeEnd = triangle.triangle[(j + 1) % 3];
	//		// エッジの方向を求める
	//		Vector3 edgeDirection = edgeEnd - edgeStart;
	//		edgeDirection.Normalize(); // エッジの方向を正規化
	//		// エッジの中点を求める
	//		Vector3 edgeMidPoint = (edgeStart + edgeEnd) / 2.0f;
	//		// エッジの中点に向かって補完レイを生成
	//		Ray edgeRay{ edgeMidPoint, edgeDirection };
	//		float edgeDistance = 0.0f;
	//		// 補完レイと三角形の衝突判定
	//		if (edgeRay.Intersects(
	//			triangle.triangle[0],
	//			triangle.triangle[1],
	//			triangle.triangle[2],
	//			edgeDistance))
	//		{
	//			if (edgeDistance < closestDistance) // 最も近い衝突点を優先
	//			{
	//				closestDistance = edgeDistance;
	//				*hitPosition = Vector3{ edgeRay.position + edgeRay.direction * edgeDistance };
	//				*hitPosition = Vector3::Transform(*hitPosition, world); // 世界座標系に戻す
	//				// 回転行列を使用して法線を回転させる
	//				*normal = Vector3::TransformNormal(triangleNormal, rotation);
	//				*polygonNo = i;
	//				hitDetected = true;
	//			}
	//		}
	//	}
	//}

	return hitDetected;
}
//-----------------------------------------------------
// Obj形式のファイルをロードする
//-----------------------------------------------------
void mylib::CollisionMesh::LoadObjFile(const std::wstring& objName)
{
	// Obj形式のファイルを開く
	std::wstring fileName = L"Resources/Models/" + objName + L".obj";
	std::ifstream ifs(fileName);

	assert(ifs && ".objファイルが読み込めませんでした");

	// 1行ごとに処理する
	std::string line;
	while (getline(ifs, line))
	{
		// 読み取った１行分のデータをストリングストリームで扱う
		std::stringstream iss(line);
		std::string type;

		// 先頭要素を受け取る
		iss >> type;

		// 頂点データを作成する
		// ※Objデータにvt,vnが入る時もあるので注意する
		if (type.compare("v") == 0)
		{
			Vector3 vec;
			iss >> vec.x >> vec.y >> vec.z;
			m_vertices.emplace_back(vec);
		}
		// インデックスデータを作成する
		else if (type.compare("f") == 0)
		{
			int a, b, c;
			iss >> a >> b >> c;

			// 三角形の頂点インデックス番号が同じ物を含む場合は無視する：３点が異なる点を指している
			if (a != b && a != c && b != c)
			{
				/*
					・Objデータのポリゴンの頂点は反時計回りに記録されている
					・左手座標系では頂点は時計回りに指定する
					・DirectXTKでこちらを使用する場合は法線情報が反転するので、注意が必要
				*/
				/*m_indices.push_back(static_cast<uint16_t>(a - 1));
				m_indices.push_back(static_cast<uint16_t>(c - 1));
				m_indices.push_back(static_cast<uint16_t>(b - 1));*/

				/*
					・Objデータのポリゴンの頂点は反時計回りに記録されている
					・右手座標系ではそのほうが都合がよいので、そのまま利用する
					・法線を求める際も反転しなくてよい
				*/
				m_indices.emplace_back(static_cast<uint16_t>(a - 1));
				m_indices.emplace_back(static_cast<uint16_t>(b - 1));
				m_indices.emplace_back(static_cast<uint16_t>(c - 1));
			}
		}
	}

	// 開いたファイルを閉じる
	ifs.close();
}

//-----------------------------------------------------
// 三角形ポリゴンを作り、ポリゴン配列に登録する
//-----------------------------------------------------
void mylib::CollisionMesh::AddTriangle(
	DirectX::SimpleMath::Vector3 a,
	DirectX::SimpleMath::Vector3 b,
	DirectX::SimpleMath::Vector3 c
)
{
	m_triangles.emplace_back(Triangle(a, b, c));
}
