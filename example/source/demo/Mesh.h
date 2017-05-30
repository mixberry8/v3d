#pragma once
#pragma once

#include "Sphere.h"
#include "AABB.h"
#include "Material.h"

class GraphicsManager;
class TextureManager;
class MaterialManager;
class MeshManager;
class ObjParser;

enum MESH_IMPORT_FLAGS : uint32_t
{
	MESH_IMPORT_INVERT_TEX_U = 0x00000001,
	MESH_IMPORT_INVERT_TEX_V = 0x00000002,
	MESH_IMPORT_INVERT_NORMAL = 0x00000004,
	MESH_IMPORT_FLIP_FACE = 0x00000008,
	MESH_IMPORT_SMOOSING = 0x00000010,

	MESH_IMPORT_NO_OPTIMIZE = 0x00000020,
	MESH_IMPORT_NO_DIFFUSE_TEXTURE = 0x00000040,
	MESH_IMPORT_NO_SPECULAR_TEXTURE = 0x00000080,
	MESH_IMPORT_NO_MASK_TEXTURE = 0x00000100,
	MESH_IMPORT_NO_NORMAL_TEXTURE = 0x00000200,

	MESH_IMPORT_NO_TEXTURE = MESH_IMPORT_NO_DIFFUSE_TEXTURE | MESH_IMPORT_NO_SPECULAR_TEXTURE | MESH_IMPORT_NO_MASK_TEXTURE | MESH_IMPORT_NO_NORMAL_TEXTURE,
};

enum MESH_TYPE
{
	MESH_TYPE_STATIC = 0,
	MESH_TYPE_SKELETAL = 1,
};

struct MeshImportDesc
{
	uint32_t flags;
	Matrix4x4 matrix;
	float smoosingAngle;
};

struct MeshVertex
{
	Vector3 pos;
	Vector2 uv;
	Vector3 normal;
	Vector3 tangent;
	Vector3 binormal;
};

struct MeshSubset
{
	uint32_t material;
	uint32_t indexCount;
	uint32_t firstIndex;
};

struct MeshBounds
{
	Sphere sphere;
	AABB aabb;
};

class Mesh
{
public:
	Mesh();
	virtual ~Mesh();

	const wchar_t* GetName() const;

	uint64_t GetKey() const;

	uint32_t GetMaterialCount() const;
	MaterialPtr GetMaterial(uint32_t material);

	const MeshBounds& GetBounds() const;

public:;
	virtual MESH_TYPE GetType() const = 0;

	virtual IV3DBuffer* GetVertexIndexBuffer() = 0;
	virtual uint64_t GetVertexOffset() const = 0;
	virtual uint64_t GetIndexOffset() const = 0;
	virtual V3D_INDEX_TYPE GetIndexType() const = 0;

	virtual V3D_RESULT Update() = 0;

	virtual bool Save(const wchar_t* pFilePath) = 0;

protected:
	struct OptPolygon
	{
		uint32_t material;
		MeshVertex vertices[3];
	};

	struct OptNode
	{
		std::wstring name;
		uint32_t parentNode;

		uint32_t polygonCount;
		uint32_t firstPolygon;

		Matrix4x4 localMatrix;
	};

	struct OptMaterial
	{
		std::wstring name;
		Vector3 ambientColor;
		Vector4 diffuseColor;
		Vector3 specularColor;
		Vector3 emissiveColor;
		float specularPower;
		float shininess;

		std::wstring diffuseTexture;
		std::wstring maskTexture;
		std::wstring specularTexture;
		std::wstring bumpTexture;

		bool hasTexture;
	};

	GraphicsManager* m_pGraphicsManager;
	TextureManager* m_pTextureManager;
	MaterialManager* m_pMaterialManager;
	MeshManager* m_pMeshManager;

	uint64_t m_Key;

	std::wstring m_Name;
	MeshBounds m_Bounds;

	virtual V3D_RESULT Initialize(GraphicsManager* pGraphicsManager, TextureManager* pTextureManager, MaterialManager* pMaterialManager, MeshManager* pMeshManager);
	virtual void Dispose();

	virtual V3D_RESULT Import(const wchar_t* pFilePath, const MeshImportDesc& importDesc) = 0;
	virtual bool Load(const wchar_t* pFilePath) = 0;

	V3D_RESULT Import(
		const wchar_t* pFilePath,
		const MeshImportDesc& importDesc,
		std::vector<Mesh::OptMaterial>& materials,
		std::vector<Mesh::OptPolygon>& polygons,
		std::vector<Mesh::OptNode>& nodes);

	static void CreateSource(
		const MeshImportDesc& loadDesc,
		std::vector<Mesh::OptPolygon>::iterator& it_polygon_begin, std::vector<Mesh::OptPolygon>::iterator& it_polygon_end,
		std::vector<MeshVertex>& dstVertices, std::vector<uint32_t>& dstIndices, std::vector<MeshSubset>& dstSubsets, Vector3* pDstCenter, Vector3* pDstHalfExtent);

	V3D_RESULT CreateVertexIndexBuffer(
		std::vector<MeshVertex>& vertices, std::vector<uint32_t>& indices,
		IV3DBuffer** ppDstBuffer, uint64_t* pDstVertexOffset, uint64_t* pDstVertexSize,
		uint64_t* pDstIndexOffset, uint64_t* pDstIndexSize, V3D_INDEX_TYPE* pDstIndexType);

	bool LoadVertexIndexBuffer(IV3DBuffer* pSrcBuffer, IV3DBuffer** ppDstBuffer);
	bool StoreVertexIndexBuffer(uint64_t verticesSize, const void* pVertices, uint64_t indicesSize, const void* pIndices, IV3DBuffer** ppDstBuffer, uint64_t* pDstVertexOffset, uint64_t* pDstIndexOffset);

	V3D_RESULT CreateMaterials(const wchar_t* pFilePath, const MeshImportDesc& importDesc, std::vector<Mesh::OptMaterial>& materials);
	bool CreateMaterials(const wchar_t* pFilePath, std::vector<Material::SerialData>& materials);

	void CloneMaterials(Mesh* pMesh);

	bool SerializeMaterials(std::vector<Material::SerialData>& materials);

private:
	struct OptFace
	{
		uint32_t indices[3];
	};

	struct OptSubset
	{
		uint32_t material;

		uint32_t firstIndex;
		uint32_t indexCount;

		uint32_t firstFace;
		uint32_t faceCount;
		std::vector<MeshVertex> vertices;
		std::vector<uint32_t> indices;
	};

	struct OptIndex
	{
		uint32_t index;
		bool assigned;
	};

	std::vector<MaterialPtr> m_Materials;

	static bool Import(
		const wchar_t* pDriPath,
		uint64_t bufferSize, const uint8_t* pBuffer,
		const MeshImportDesc& importDesc,
		std::vector<Mesh::OptPolygon>& polygons,
		std::vector<Mesh::OptNode>& nodes,
		std::vector<Mesh::OptMaterial>& materials);

	static bool ImportMaterial(const wchar_t* pFilePath, std::vector<Mesh::OptMaterial>& materials);
	static bool ImportMaterial(uint64_t bufferSize, void* pBuffer, std::vector<Mesh::OptMaterial>& materials);
	static bool ImportName(ObjParser* pParser, std::wstring& name);

	static void Optimize(
		uint32_t flags, float smoosingCos,
		std::vector<Mesh::OptPolygon>::iterator& it_polygon_begin, std::vector<Mesh::OptPolygon>::iterator& it_polygon_end,
		std::vector<MeshVertex>& dstVertices, std::vector<uint32_t>& dstIndices, std::vector<MeshSubset>& dstSubsets);

	friend class MeshManager;
};
