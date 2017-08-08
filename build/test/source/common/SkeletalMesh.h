#pragma once

#include "Mesh.h"

class SkeletalMeshNode
{
public:
	SkeletalMeshNode();
	virtual ~SkeletalMeshNode();

	const wchar_t* GetName() const;

	IV3DDescriptorSet* GetDescriptorSetPtr();
	uint32_t GetDynamicOffset() const;

	const Matrix4x4& GetLocalMatrix() const;
	void SetLocalMatrix(const Matrix4x4& localMatrix);

	const Matrix4x4& GetWorldMatrix() const;

	const MeshBounds& GetBounds() const;

	uint32_t GetSubsetCount() const;
	const MeshSubset& GetSubset(uint32_t subset) const;

	void BindDescriptorSet(IV3DCommandBuffer* pCommandBuffer);

private:
	struct Uniform
	{
		Matrix4x4 worldMat;
	};

	std::wstring m_Name;
	uint32_t m_ParentIndex;
	std::weak_ptr<SkeletalMeshNode> m_Parent;
	std::vector<std::shared_ptr<SkeletalMeshNode>> m_Childs;
	std::vector<MeshSubset> m_Subsets;

	Vector3 m_LocalCenter;
	Vector3 m_HalfExtent;
	MeshBounds m_Bounds;

	Matrix4x4 m_LocalMatrix;

	IV3DPipelineLayout* m_pPipelineLayout;

	SkeletalMeshNode::Uniform m_Uniform;
	IV3DBuffer* m_pUniformBuffer;
	DynamicOffsetHeap::Handle m_UniformDynamicOffsetHandle;
	IV3DDescriptorSet* m_pDescriptorSet;

	V3D_RESULT Update(const Matrix4x4& parentWorldMatrix);

	friend class SkeletalMesh;
};

typedef std::shared_ptr<SkeletalMeshNode> SkeletalMeshNodePtr;

class SkeletalMesh : public Mesh
{
public:
	/****************/
	/* SkeletalMesh */
	/****************/

	SkeletalMesh();
	virtual ~SkeletalMesh();

	SkeletalMeshNodePtr GetRootNode();

	uint32_t GetNodeCount() const;
	SkeletalMeshNodePtr GetNode(uint32_t node);

	uint32_t GetMeshNodeCount() const;
	SkeletalMeshNodePtr GetMeshNode(uint32_t meshNode);

	const Matrix4x4& GetWorldMatrix() const;
	void SetWorldMatrix(const Matrix4x4& worldMatrix);

	void BindVertexIndexBuffer(IV3DCommandBuffer* pCommandBuffer);

	void Draw(IV3DCommandBuffer* pCommandBuffer);

	std::shared_ptr<SkeletalMesh> Clone();

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
	virtual void Dispose() override;

	virtual V3D_RESULT Import(const wchar_t* pFilePath, const MeshImportDesc& importDesc) override;
	virtual bool Load(const wchar_t* pFilePath) override;

private:
	/****************/
	/* SkeletalMesh */
	/****************/

	static constexpr uint32_t MagicNumber = ('V' << 24) | ('S' << 16) | ('K' << 8) | 'M';
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
		uint32_t nodeCount;
		uint64_t verticesSize;
		uint64_t indicesSize;
		uint32_t indexType;
		uint32_t reserve;
	};

	struct NodeInfo
	{
		wchar_t name[256];

		uint32_t parentNode;
		uint32_t reserve[1];

		uint32_t opSubsetCount;
		uint32_t opFirstSubset;

		uint32_t trSubsetCount;
		uint32_t trFirstSubset;

		Vector4 localCenter;
		Vector4 halfExtent;

		Matrix4x4 localMatrix;
	};

	IV3DPipelineLayout* m_pPipelineLayout;

	IV3DBuffer* m_pBuffer;
	uint64_t m_VertexOffset;
	uint64_t m_VerticesSize;
	uint64_t m_IndexOffset;
	uint64_t m_IndicesSize;
	V3D_INDEX_TYPE m_IndexType;

	std::vector<SkeletalMeshNodePtr> m_Nodes;
	std::vector<SkeletalMeshNodePtr> m_MeshNodes;

	Matrix4x4 m_WorldMatrix;

	friend class MeshManager;
};

typedef std::shared_ptr<SkeletalMesh> SkeletalMeshPtr;
