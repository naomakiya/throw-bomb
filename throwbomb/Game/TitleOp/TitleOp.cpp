/*
	@file	TitleOp.cpp
	@brief  �^�C�g���I�[�v�j���O�N���X
*/
#include "pch.h"
#include "TitleOp.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/ResourceManager/ResourceManager.h"
#include "Game/Sound/Sound.h"
#include "Framework/LoadJson.h"
#include "Game/Dithering/Dithering.h"
#include <Libraries/MyLib/BinaryFile.h>
#include "Game/UI/UI.h"
#include <PrimitiveBatch.h>

TitleOp::TitleOp()
    : m_commonResources(nullptr),
    m_position(0.0f, 0.0f, -20.0f),
    m_scale(1.0f),
    m_elapsedTime(0.0f),
    m_visible(true),
    m_IsFade(false),
    m_bgmVolume(0.0f),
    m_seVolume(0.0f),
    m_bombs(5),
    m_time(0.0f),
    m_exprtime(0.0f)
{

}

TitleOp::~TitleOp() {
    Finalize();
}

void TitleOp::Initialize(CommonResources* resources)
{
    assert(resources);
    m_commonResources = resources;

    auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
    auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

   
    CreateShader();
    LoadTexture(L"Resources/Textures/WallCrack.jpg");
    LoadTexture(L"Resources/Textures/hight.jpg");
    // ���f���̓ǂݍ���
    auto fxExpl = std::make_unique<DirectX::DX11::EffectFactory>(device);
    fxExpl->SetDirectory(L"Resources/Models/exprol");
    m_explosionSphere = DirectX::Model::CreateFromCMO(device, ResourceManager::getModelPath("Exprol").c_str(), *fxExpl);

    auto fxBom = std::make_unique<DirectX::DX11::EffectFactory>(device);
    fxBom->SetDirectory(L"Resources/Models");
    m_bomModel = DirectX::Model::CreateFromCMO(device, ResourceManager::getModelPath("Bom").c_str(), *fxBom);

    m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
    m_dithering = std::make_unique<Dithering>(device);
    m_states = std::make_unique<DirectX::CommonStates>(device);
    //	�v���~�e�B�u�o�b�`�̍쐬
    m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>>(context);
    // ���ʓǂݍ���
    LoadJson json("Resources/Json/Music.json");
    m_bgmVolume = json.GetJson()["BGM"].value("Volume", 0.0f);
    m_seVolume = json.GetJson()["SE"].value("Volume", 0.0f);

    ResetBombs();

    // �T�E���h�Đ�
    auto& sound = Sound::GetInstance();
    sound.Initialize();
   
}

void TitleOp::Update(float elapsedTime)
{
    m_elapsedTime = elapsedTime;
    m_time += elapsedTime;
    m_position.z += elapsedTime * 10.0f;
    std::vector<Bomb> explodedBombs;
   
    for (auto& bomb : m_bombs)
    {
        bomb.position.z += 25.0f;

        TriggerMainExplosionIfNeeded(bomb);
        TriggerChainExplosionIfNeeded(bomb);

        if (bomb.isExploded)
            explodedBombs.push_back(bomb);
    }
    for (auto& obj : m_explosionObjects)
    {
        if (!obj.active) continue;

        // �Ō�̔��e�ɑΉ����锚�������g��
        if (obj.sourceBombIndex == static_cast<int>(m_bombs.size() - 1))
        {
            obj.scaleMultiplier += elapsedTime ;

            if (obj.scaleMultiplier >= 5.0f)
            {
                // ���ׂĂ̔����𖳌��ɂ���
                for (auto& explosion : m_explosionObjects)
                {
                    explosion.active = false;
                }
            }
        }
    }
}

void TitleOp::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
    auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
    auto states = m_commonResources->GetCommonStates();

    m_view = view;
    m_projection = projection;

    ShaderRender();

    //�`����̈ʒu����O�ɂ���
    auto depthStencil = m_commonResources->GetDeviceResources()->GetDepthStencilView();
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 1);

    for (auto& obj : m_explosionObjects)
    {
        if (!obj.active) continue;

        m_exprtime += m_elapsedTime;
        if (m_exprtime > 5.0f)
        {
            obj.active = false;
        }
       
        DirectX::SimpleMath::Vector3 drawPos = m_position + obj.offset;

        float drawScale = obj.scaleMultiplier * 2.0f;

        auto world = DirectX::SimpleMath::Matrix::CreateScale(drawScale * 0.5f) *
            DirectX::SimpleMath::Matrix::CreateRotationY(m_elapsedTime * 0.5f) *
            DirectX::SimpleMath::Matrix::CreateTranslation(drawPos);

        m_dithering->RenderObjects(context, states, projection, view, drawPos, world, drawPos, m_explosionSphere.get());
    }
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    for (const auto& bomb : m_bombs)
    {
        auto world = DirectX::SimpleMath::Matrix::CreateScale(bomb.scale * 2.0f) *
            DirectX::SimpleMath::Matrix::CreateTranslation(bomb.position);

        m_dithering->RenderObjects(context, states, projection, view, bomb.position,
            world, bomb.position, m_bomModel.get());
    }
   
}

void TitleOp::Finalize()
{
    Sound::GetInstance().Finalize();
}


void TitleOp::TriggerMainExplosionIfNeeded(Bomb& bomb)
{
    if (!bomb.isExploded && bomb.position.z > 550.0f)
    {
        bomb.isExploded = true;
        bomb.position.z = 600.0f;
        bomb.explodedTime = m_elapsedTime;

        int bombIndex = static_cast<int>(&bomb - &m_bombs[0]);
        CreateMainExplosion(bomb.position, bombIndex);

    }
}

void TitleOp::TriggerChainExplosionIfNeeded(Bomb& bomb)
{
    if (bomb.isExploded && !bomb.hasChainedExplosion)
    {
        if (m_elapsedTime > bomb.explodedTime + 1.0f && m_elapsedTime <= bomb.explodedTime + 3.0f)
        {
            CreateChainExplosions(bomb.position);
            bomb.hasChainedExplosion = true;
        }
    }
}

void TitleOp::CreateMainExplosion(const DirectX::SimpleMath::Vector3& pos, int bombIndex)
{
    // �Ō�̔��e���ǂ����`�F�b�N
    bool isLastBomb = false;
    if (!m_bombs.empty() && m_bombs.back().position == pos)
    {
        isLastBomb = true;
    }

    float scale = isLastBomb ? 3.0f : 1.5f; // �� �Ōゾ���X�P�[����傫��

    m_explosionObjects.push_back({
        pos - m_position,
        scale,
        m_elapsedTime,
        1.5f,
        true,
        bombIndex
        });

    // �����̈ʒu���i�[���� std::vector<DirectX::XMFLOAT4> ���쐬
    std::vector<DirectX::XMFLOAT4> positions;
   
    // ��Ƃ��Ĕ����ʒu��ǉ�
    positions.push_back(DirectX::XMFLOAT4(pos.x, pos.y, pos.z, 0.0));
    SetExplosions(positions);
    m_exprtime = 0.0f;
}

void TitleOp::CreateChainExplosions(const DirectX::SimpleMath::Vector3& center)
{
    const int explosionCount = 8;

    for (int i = 0; i < explosionCount; ++i)
    {
        float offsetX = (rand() % 100 - 50) * 3.0f;
        float offsetY = (rand() % 100 - 50) * 3.0f;
        float offsetZ = (rand() % 100 - 50) * 3.0f;

        m_explosionObjects.push_back({
            DirectX::SimpleMath::Vector3(offsetX, offsetY, offsetZ) + center - m_position,
            0.8f + static_cast<float>(rand() % 100) / 300.0f,
            m_elapsedTime + static_cast<float>(rand() % 30) / 60.0f,
            0.8f,
            true
            });
    }
}
void TitleOp::SetExplosions(const std::vector<DirectX::XMFLOAT4>& positions)
{

    ID3D11DeviceContext1* context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

    ExplosionBufferData bufferData{};
    for (size_t i = 0; i < positions.size() && i < 5; ++i)
    {
        bufferData.explosionPositions[i] = positions[i];

        // UV���W�ϊ��F���[���h���W���r���[�E�v���W�F�N�V�����ŃX�N���[����Ԃɕϊ�
        DirectX::XMFLOAT3 worldPos = { positions[i].x, positions[i].y, positions[i].z };
        DirectX::XMFLOAT2 uv = WorldToUV(worldPos, m_view, m_projection);

        bufferData.explosionPositions[i] = DirectX::XMFLOAT4(uv.x,uv.y, 1.0f, 0.0f); // ���� uvs[] ����`����Ă����

    }

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = context->Map(m_CBufferexp.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (SUCCEEDED(hr)) {
        memcpy(mappedResource.pData, &bufferData, sizeof(ExplosionBufferData));
        context->Unmap(m_CBufferexp.Get(), 0);
    }
    else {
        // ���O�Ɏ��s�������Ƃ������ƃf�o�b�O���₷��
        OutputDebugStringA("Failed to map explosion constant buffer.\n");
    }

    // �V�F�[�_�[�Ƀo�b�t�@��n��
    ID3D11Buffer* cb[1] = { m_CBufferexp.Get() };
    context->PSSetConstantBuffers(1, 1, cb);
}

void TitleOp::ResetBombs()
{
    m_bombs.clear();
    m_time = 0.0f;
    m_elapsedTime = 0.0f;

    std::vector<DirectX::SimpleMath::Vector3> positions = {
     {-100.0f, 100.0f, -1000.0f},
     {100.0f, 100.0f, -1000.0f},
     {-250.0f, -50.0f, -2000.0f},
     {250.0f, -50.0f, -2000.0f},
     {0.0f, 0.0f, -5000.0f}
    };
   /* std::vector<DirectX::SimpleMath::Vector3> positions = {
     {-00.0f, 00.0f, -1000.0f},
     {00.0f, 00.0f, -1000.0f},
     {-0.0f, -0.0f, -2000.0f},
     {0.0f, -0.0f, -2000.0f},
     {0.0f, 0.0f, -5000.0f}
    };*/

    for (const auto& pos : positions) {
        m_bombs.push_back(Bomb{ pos, false, false, 1.0f, -1.0f, 1.5f });
    }
}

void TitleOp::CreateShader()
{
    ID3D11Device1* device = m_commonResources->GetDeviceResources()->GetD3DDevice();

    // �R���p�C�����ꂽ�V�F�[�_�t�@�C����ǂݍ���
    BinaryFile VSData = BinaryFile::LoadFile(L"Resources/Shaders/fadeVS.cso");
    BinaryFile GSData = BinaryFile::LoadFile(L"Resources/Shaders/fadeGS.cso");
    BinaryFile PSData = BinaryFile::LoadFile(L"Resources/Shaders/OPFadePS.cso");

    // �C���v�b�g���C�A�E�g�̍쐬
    device->CreateInputLayout(&UI::INPUT_LAYOUT[0],
        static_cast<UINT>(UI::INPUT_LAYOUT.size()),
        VSData.GetData(), VSData.GetSize(),
        m_inputLayout.GetAddressOf());

    // ���_�V�F�[�_�쐬
    if (FAILED(device->CreateVertexShader(VSData.GetData(), VSData.GetSize(), NULL, m_vertexShader.ReleaseAndGetAddressOf())))
    {
        MessageBox(0, L"CreateVertexShader Failed.", NULL, MB_OK);
        return;
    }

    // �W�I���g���V�F�[�_�쐬
    if (FAILED(device->CreateGeometryShader(GSData.GetData(), GSData.GetSize(), NULL, m_geometryShader.ReleaseAndGetAddressOf())))
    {
        MessageBox(0, L"CreateGeometryShader Failed.", NULL, MB_OK);
        return;
    }

    // �s�N�Z���V�F�[�_�쐬
    if (FAILED(device->CreatePixelShader(PSData.GetData(), PSData.GetSize(), NULL, m_pixelShader.ReleaseAndGetAddressOf())))
    {
        MessageBox(0, L"CreatePixelShader Failed.", NULL, MB_OK);
        return;
    }

    // �V�F�[�_�[�Ƀf�[�^��n�����߂̃R���X�^���g�o�b�t�@����
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    device->CreateBuffer(&bd, nullptr, &m_CBuffer);

    D3D11_BUFFER_DESC cbDesc = {};
    ZeroMemory(&cbDesc, sizeof(bd));
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.ByteWidth = sizeof(ExplosionBufferData);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    /*cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;*/

    HRESULT hr = device->CreateBuffer(&cbDesc, nullptr, &m_CBufferexp);
    assert(SUCCEEDED(hr));
}

void TitleOp::LoadTexture(const wchar_t* path)
{
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
    DirectX::CreateWICTextureFromFile(m_commonResources->GetDeviceResources()->GetD3DDevice(), path, nullptr, texture.ReleaseAndGetAddressOf());
    m_texture.push_back(texture);
}

void TitleOp::ShaderRender()
{
    ID3D11DeviceContext1* context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

    //	���_���(�|���S���̂S���_�̍��W���j
    DirectX::VertexPositionColorTexture vertex[4] =
    {
        DirectX::VertexPositionColorTexture(DirectX::SimpleMath::Vector3(0.0f,  0.0f, 0.0f), DirectX::SimpleMath::Vector4::One, DirectX::SimpleMath::Vector2(0.0f, 0.0f)),
    };

    // �V�F�[�_�[�ɓn���ǉ��̃o�b�t�@���쐬���� (ConstBuffer)
    ConstBuffer cbuff;
    cbuff.matView = DirectX::SimpleMath::Matrix::Identity;
    cbuff.matProj = DirectX::SimpleMath::Matrix::Identity;
    cbuff.matWorld = DirectX::SimpleMath::Matrix::Identity;
    cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
    cbuff.time = DirectX::SimpleMath::Vector4(m_time, 0., 0, 0);
    
    // �󂯓n���p�o�b�t�@�̓��e�X�V
    context->UpdateSubresource(m_CBuffer.Get(), 0, NULL, &cbuff, 0, 0);

    // �V�F�[�_�[�Ƀo�b�t�@��n��
    ID3D11Buffer* cb[1] = { m_CBuffer.Get() };
    context->VSSetConstantBuffers(0, 1, cb);
    context->GSSetConstantBuffers(0, 1, cb);
    context->PSSetConstantBuffers(0, 1, cb);

    ExplosionBufferData bufferData{};
    for (size_t i = 0; i < m_bombs.size() && i < 5; ++i)
    {
        // UV���W�ϊ��F���[���h���W���r���[�E�v���W�F�N�V�����ŃX�N���[����Ԃɕϊ�
        DirectX::XMFLOAT3 worldPos = { m_bombs[i].position.x,  m_bombs[i].position.y,  m_bombs[i].position.z };
        DirectX::XMFLOAT2 uv = WorldToUV(worldPos, m_view, m_projection);

        float Expr = 1.0f;
        if (m_bombs[i].isExploded) Expr = 0.0f;
        bufferData.explosionPositions[i] = DirectX::XMFLOAT4(uv.x, uv.y, Expr, 0.0f); // ���� uvs[] ����`����Ă����

    }

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = context->Map(m_CBufferexp.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (SUCCEEDED(hr)) {
        memcpy(mappedResource.pData, &bufferData, sizeof(ExplosionBufferData));
        context->Unmap(m_CBufferexp.Get(), 0);
    }
    else {
        // ���O�Ɏ��s�������Ƃ������ƃf�o�b�O���₷��
        OutputDebugStringA("Failed to map explosion constant buffer.\n");
    }
    // �V�F�[�_�[�Ƀo�b�t�@��n��
    ID3D11Buffer* cbexp[1] = { m_CBufferexp.Get() };
    context->PSSetConstantBuffers(1, 1, cbexp);

    // �摜�p�T���v���[�̓o�^
    ID3D11SamplerState* sampler[1] = { m_states->LinearWrap() };
    context->PSSetSamplers(0, 1, sampler);

    // �������`��w��
    ID3D11BlendState* blendstate = m_states->NonPremultiplied();
    context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);

    // �[�x�o�b�t�@�ݒ�
    context->OMSetDepthStencilState(m_states->DepthDefault(), 0);

    // �J�����O�ݒ�
    context->RSSetState(m_states->CullNone());

    // �V�F�[�_���Z�b�g����
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    context->GSSetShader(m_geometryShader.Get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

    // �s�N�Z���V�F�[�_�Ƀe�N�X�`����o�^����
    for (int i = 0; i < m_texture.size(); i++)
    {
        context->PSSetShaderResources(i, 1, m_texture[i].GetAddressOf());
    }

    // �C���v�b�g���C�A�E�g��ݒ�
    context->IASetInputLayout(m_inputLayout.Get());

    // �|���S���`��
    m_batch->Begin();
    m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertex[0], 4);
    m_batch->End();

    // �V�F�[�_�̓o�^����
    context->VSSetShader(nullptr, nullptr, 0);
    context->GSSetShader(nullptr, nullptr, 0);
    context->PSSetShader(nullptr, nullptr, 0);
}

DirectX::XMFLOAT2 TitleOp::WorldToUV(
    const DirectX::XMFLOAT3& worldPos,
    const DirectX::XMMATRIX& view,
    const DirectX::XMMATRIX& projection)
{
    using namespace DirectX;

    // ���[���h���W��4D�x�N�g���ɕϊ��iw=1.0�j
    XMVECTOR pos = XMLoadFloat3(&worldPos);
    pos = XMVectorSetW(pos, 1.0f);

    // �r���[�E�v���W�F�N�V�������������ăN���b�v��Ԃɕϊ�
    XMMATRIX viewProj = XMMatrixMultiply(view, projection);
    XMVECTOR clipPos = XMVector4Transform(pos, viewProj);

    // clipPos (x/w, y/w, z/w) �����o��
    XMFLOAT4 clip;
    XMStoreFloat4(&clip, clipPos);

    // w��0�ɋ߂��Ƃ��̑΍�
    if (fabsf(clip.w) < 1e-5f)
        return XMFLOAT2(0.0f, 0.0f);

    float ndcX = clip.x / clip.w; // -1 ? +1
    float ndcY = clip.y / clip.w;

    // NDC��UV���W�Ƀ}�b�s���O�i0?1�j
    float u = (ndcX * 0.5f) + 0.5f;
    float v = (ndcY * 0.5f) + 0.5f; // Y�����]�ɒ��Ӂi���オ(0,0)�̂��߁j

    return XMFLOAT2(u, v);
}