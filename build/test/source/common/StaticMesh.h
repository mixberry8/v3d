#pragma once

#include "Mesh.h"
#include "DynamicOffsetHeap.h"

class StaticMesh : public Mesh
{
public:
	/**************/
	/* StaticMesh */
	/**************/

	StaticMesh();
	virtual ~StaticMesh();

	const Matrix4x4& GetWorldMatrix() const;
	void SetWorldMatrix(const Matrix4x4& worldMatrix);

	void BindVertexIndexBuffer(IV3DCommandBuffer* pCommandBuffer);
	void BindDescriptorSet(IV3DCommandBuffer* pCommandBuffer);

	uint32_t GetSubsetCount() const;
	const MeshSubset& GetSubset(uint32_t subset) const;

	void Draw(IV3DCommandBuffer* pCommandBuffer);

	std::shared_ptr<StaticMesh> Clone();

	/********/
	/* Mesh */
	/********/

	virtual MESH_TYPE GetType() const override;

	virtual IV3DBuffer* GetVertexIndexBuffer() override;
	virtual uint64_t GetVertexOffset() const override;
	virtual uint64_t GetIndexOffset() const override;
	virtual V3D_INDEX_TYPE GetIndexType() const override;

	virtual V3D_RESULT Update() override;

	virtual bool Save(const wchar_t* pFilePath) override;

protected:
	/********/
	/* Mesh */
	/********/

	virtual V3D_RESULT Initialize(GraphicsManager* pGraphicsManager, TextureManager* pTextureManager, MaterialManager* pMaterialManager, MeshManager* pMeshManager) override;
	virtual void Dispose()  override;

	virtual V3D_RESULT Import(const wchar_t* pFilePath, const MeshImportDesc& importDesc) override;
	virtual bool Load(const wchar_t* pFilePath) override;

private:
	/**************/
	/* StaticMesh */
	/**************/

	static constexpr uint32_t MagicNumber = ('V' << 24) | ('S' << 16) | ('T' << 8) | 'M';
	static constexpr uint32_t Version = 0x01000000;

	struct FileHeader
	{
		uint32_t magicNumber;
		uint32_t version;
	};

	struct InfoHeader
	{
		uint32_t materialCount;
		uint32_t opSubsetCount;
		uint32_t trSubsetCount;
		uint64_t verticesSize;
		uint64_t indicesSize;
		uint32_t indexType;
		float localCenter[4];
		float halfExtent[4];
	};

	struct Uniform
	{
		Matrix4x4 worldMat;
	};

	Vector3 m_LocalCenter;
	Vector3 m_HalfExtent;

	IV3DPipelineLayout* m_pPipelineLayout;

	StaticMesh::Uniform m_Uniform;
	IV3DBuffer* m_pUniformBuffer;
	DynamicOffsetHeap::Handle m_UniformDynamicOffsetHandle;
	IV3DDescriptorSet* m_pDescriptorSet;

	IV3DBuffer* m_pBuffer;
	uint64_t m_VertexOffset;
	uint64_t m_VerticesSize;
	uint64_t m_IndexOffset;
	uint64_t m_IndicesSize;
	V3D_INDEX_TYPE m_IndexType;

	std::vector<MeshSubset> m_Subsets;

	friend class MeshManager;
};

typedef std::shared_ptr<StaticMesh> StaticMeshPtr;
