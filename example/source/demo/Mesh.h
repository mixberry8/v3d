#pragma once
#pragma once

#include "Material.h"

class GraphicsManager;
class TextureManager;
class MaterialManager;
class MeshManager;

enum MESH_LOAD_FLAGS : uint32_t
{
	MESH_LOAD_INVERT_TEX_U = 0x00000001,
	MESH_LOAD_INVERT_TEX_V = 0x00000002,
	MESH_LOAD_INVERT_NORMAL = 0x00000004,
	MESH_LOAD_FLIP_FACE = 0x00000008,
	MESH_LOAD_SMOOSING = 0x00000010,

	MESH_LOAD_NO_OPTIMIZE = 0x00000020,
	MESH_LOAD_NO_DIFFUSE_TEXTURE = 0x00000040,
	MESH_LOAD_NO_NORMAL_TEXTURE = 0x00000080,

	MESH_LOAD_NO_TEXTURE = MESH_LOAD_NO_DIFFUSE_TEXTURE | MESH_LOAD_NO_NORMAL_TEXTURE,
};

struct MeshLoadDesc
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

class Mesh
{
public:
	Mesh();
	virtual ~Mesh();

protected:
	struct OptNode
	{
		std::wstring name;
		uint32_t parentNode;
		uint32_t firstSubset;
		uint32_t subsetCount;
	};

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
		std::wstring bumpTexture;
	};

	GraphicsManager* m_pGraphicsManager;
	TextureManager* m_pTextureManager;
	MaterialManager* m_pMaterialManager;
	MeshManager* m_pMeshManager;

	virtual V3D_RESULT Initialize(GraphicsManager* pGraphicsManager, TextureManager* pTextureManager, MaterialManager* pMaterialManager, MeshManager* pMeshManager);
	virtual void Dispose();

	static V3D_RESULT Load(
		const wchar_t* pFilePath,
		const MeshLoadDesc& loadDesc,
		std::vector<MeshVertex>& vertices,
		std::vector<Mesh::OptNode>& nodes,
		std::vector<Mesh::OptSubset>& subsets,
		std::vector<Mesh::OptMaterial>& materials);

	static V3D_RESULT LoadMaterial(const wchar_t* pDirPath, const wchar_t* pFilePath, std::vector<Mesh::OptMaterial>& materials);
	static V3D_RESULT LoadMaterial(const wchar_t* pDirPath, uint64_t bufferSize, void* pBuffer, std::vector<Mesh::OptMaterial>& materials);
};
