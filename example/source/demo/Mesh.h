#pragma once

#include "Material.h"
#include "ResourceHeap.h"

class GraphicsManager;
class TextureManager;
class MaterialManager;
class MeshManager;

class Mesh
{
public:
	struct LoadDesc
	{
		Matrix4x4 rotation;
		bool invertTex[2];
		bool invertNormal;
		bool flipFace;
		bool isSmoosing;
		float smoosingAngle;
	};

	struct Vertex
	{
		Vector3 pos;
		Vector2 uv;
		Vector3 normal;
		Vector3 tangent;
		Vector3 binormal;
	};

	struct Subset
	{
		uint32_t material;
		uint32_t indexCount;
		uint32_t firstIndex;
	};

	Mesh();
	~Mesh();

	std::shared_ptr<Mesh> Clone();

	uint64_t GetKey() const;

	uint32_t GetMaterialCount() const;
	MaterialPtr GetMaterial(uint32_t material);

	uint32_t GetSubsetCount() const;
	const Mesh::Subset& GetSubset(uint32_t subset) const;

	const Matrix4x4& GetWorldMatrix() const;
	void SetWorldMatrix(const Matrix4x4& worldMatrix);

	V3D_RESULT Update(const Matrix4x4& viewProjMat);

	void BindVertexIndexBuffer(IV3DCommandBuffer* pCommandBuffer);
	void BindDescriptorSet(IV3DCommandBuffer* pCommandBuffer);

	void Draw(IV3DCommandBuffer* pCommandBuffer);

private:
	/************/
	/* ç≈ìKâªóp */
	/************/

	struct FaceOpt
	{
		uint32_t indices[3];
	};

	struct SubsetOpt
	{
		uint32_t material;

		uint32_t firstIndex;
		uint32_t indexCount;

		uint32_t firstFace;
		uint32_t faceCount;
		std::vector<Mesh::Vertex> vertices;
		std::vector<uint32_t> indices;
	};

	struct IndexOpt
	{
		uint32_t index;
		bool assigned;
	};

	struct MaterialOpt
	{
		std::wstring name;
		Vector3 ambientColor; // RGB
		Vector4 diffuseColor; // RGBA
		Vector3 specularColor; // RGB
		Vector3 emissiveColor; // RGB
		float specularPower;
		float shininess;

		std::wstring diffuseTexture;
	};

	/************/
	/* Ç‡ÇÎÇ‡ÇÎ */
	/************/

	struct Uniform
	{
		Matrix4x4 worldMat;
		Matrix4x4 worldViewProjMat;
	};

	GraphicsManager* m_pGraphicsManager;
	TextureManager* m_pTextureManager;
	MaterialManager* m_pMaterialManager;
	MeshManager* m_pMeshManager;

	IV3DPipelineLayout* m_pPipelineLayout;

	Mesh::Uniform m_Uniform;
	IV3DBuffer* m_pUniformBuffer;
	IV3DBufferView* m_pUniformBufferView;
	ResourceHeap::Handle m_UniformBufferHandle;
	IV3DDescriptorSet* m_pDescriptorSet;

	IV3DBuffer* m_pBuffer;
	IV3DBufferView* m_pVertexBufferView;
	IV3DBufferView* m_pIndexBufferView;
	V3D_INDEX_TYPE m_IndexType;

	std::vector<Mesh::Subset> m_Subsets;
	std::vector<std::shared_ptr<Material>> m_Materials;

	uint64_t m_Key;
	std::wstring m_Name;

	V3D_RESULT Initialize(GraphicsManager* pGraphicsManager, TextureManager* pTextureManager, MaterialManager* pMaterialManager, MeshManager* pMeshManager);
	void Dispose();

	V3D_RESULT LoadFromFile(const wchar_t* pFilePath, const Mesh::LoadDesc& loadDesc);

	static void Optimize(
		std::vector<Mesh::Vertex>& srcVertices, std::vector<Mesh::SubsetOpt>& srcSubsets,
		std::vector<Mesh::Vertex>& dstVertices, std::vector<uint32_t>& dstIndices, std::vector<Mesh::Subset>& dstSubsets,
		bool flipFace, bool isSmoosing, float smoosingCos);

	static V3D_RESULT LoadObj(
		const wchar_t* pDirPath,
		uint64_t bufferSize, void* pBuffer,
		std::vector<Mesh::Vertex>& vertices,
		std::vector<Mesh::SubsetOpt>& subsets,
		std::vector<Mesh::MaterialOpt>& materials);

	static V3D_RESULT LoadObjMaterial(const wchar_t* pDirPath, const wchar_t* pFilePath, std::vector<Mesh::MaterialOpt>& materials);
	static V3D_RESULT LoadObjMaterial(const wchar_t* pDirPath, uint64_t bufferSize, void* pBuffer, std::vector<Mesh::MaterialOpt>& materials);

	friend class MeshManager;
};

typedef std::shared_ptr<Mesh> MeshPtr;
