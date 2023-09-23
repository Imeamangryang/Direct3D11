#pragma once
#include "Common.h"

#include "assimp/Importer.hpp" // C++ importer interface
#include "assimp/scene.h" // output data structure
#include "assimp/postprocess.h" // post processing flags

#include "Renderer/Renderable.h"


class Model : public Renderable
{
public:
	Model(_In_ const std::filesystem::path& filePath);
	~Model() override;
	HRESULT Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext) override;
	void Update(_In_ FLOAT deltaTime) override;

	UINT GetNumVertices() const override;
	UINT GetNumIndices() const override;

	const SimpleVertex* getVertices() const override;
	const WORD* getIndices() const override;

	std::filesystem::path m_filePath;
	std::vector<SimpleVertex> m_aVertices;
	std::vector<WORD> m_aIndices;

protected:
	HRESULT initFromScene(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext, _In_ const aiScene* pScene, _In_ const std::filesystem::path& filePath);
	void countVerticesAndIndices(_Inout_ UINT& uOutNumVertices, _Inout_ UINT& uOutNumIndices, _In_ const aiScene* pScene);
	void reserveSpace(_In_ UINT uNumVertices, _In_ UINT uNumIndices);
	void initAllMeshes(_In_ const aiScene* pScene);
	void initSingleMesh(_In_ const aiMesh* pMesh);
	HRESULT initMaterials(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext, _In_ const aiScene* pScene, _In_ const std::filesystem::path& filePath);
	HRESULT loadTextures(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext, _In_ const std::filesystem::path& parentDirectory, _In_ const aiMaterial* pMaterial, _In_ UINT uIndex);
	HRESULT loadDiffuseTexture(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext, _In_ const std::filesystem::path& parentDirectory, _In_ const aiMaterial* pMaterial, _In_ UINT uIndex);
	HRESULT loadSpecularTexture(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext, _In_ const std::filesystem::path& parentDirectory, _In_ const aiMaterial* pMaterial, _In_ UINT uIndex);
	void loadColors(_In_ const aiMaterial* pMaterial, _In_ UINT uIndex);
};