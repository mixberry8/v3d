#include "Mesh.h"
#include "ObjParser.h"
#include "GraphicsManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"

Mesh::Mesh() :
	m_Key(0)
{
}

Mesh::~Mesh()
{
	Dispose();
}

const wchar_t* Mesh::GetName() const
{
	return m_Name.c_str();
}

uint64_t Mesh::GetKey() const
{
	return m_Key;
}

uint32_t Mesh::GetMaterialCount() const
{
	return TO_UI32(m_Materials.size());
}

MaterialPtr Mesh::GetMaterial(uint32_t material)
{
	return m_Materials[material];
}

const MeshBounds& Mesh::GetBounds() const
{
	return m_Bounds;
}

V3D_RESULT Mesh::Initialize(GraphicsManager* pGraphicsManager, TextureManager* pTextureManager, MaterialManager* pMaterialManager, MeshManager* pMeshManager)
{
	m_pGraphicsManager = pGraphicsManager;
	m_pTextureManager = pTextureManager;
	m_pMaterialManager = pMaterialManager;
	m_pMeshManager = pMeshManager;

	return V3D_OK;
}

void Mesh::Dispose()
{
	if (m_Materials.empty() == false)
	{
		m_Materials.clear();
		m_Materials.shrink_to_fit();
	}

	m_pMeshManager = nullptr;
	m_pMaterialManager = nullptr;
	m_pTextureManager = nullptr;
	m_pGraphicsManager = nullptr;
}

V3D_RESULT Mesh::Import(
	const wchar_t* pFilePath,
	const MeshImportDesc& importDesc,
	std::vector<Mesh::OptMaterial>& materials,
	std::vector<Mesh::OptPolygon>& polygons,
	std::vector<Mesh::OptNode>& nodes)
{
	// ----------------------------------------------------------------------------------------------------
	// メッシュをバッファに読み込む
	// ----------------------------------------------------------------------------------------------------

	HANDLE fileHandle = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		return V3D_ERROR_FAIL;
	}

	DWORD readSize = 0;
	DWORD bufferSize = ::GetFileSize(fileHandle, nullptr);
	uint8_t* pBuffer = new(std::nothrow) uint8_t[bufferSize];
	if (pBuffer == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	if (ReadFile(fileHandle, pBuffer, bufferSize, &readSize, nullptr) == FALSE)
	{
		SAFE_DELETE_ARRAY(pBuffer);
		CloseHandle(fileHandle);
		return V3D_ERROR_FAIL;
	}

	CloseHandle(fileHandle);

	// ----------------------------------------------------------------------------------------------------
	// ディレクトリパスの抽出
	// ----------------------------------------------------------------------------------------------------

	std::wstring dirPath;

	int32_t filePathLength = static_cast<int32_t>(wcslen(pFilePath));
	int32_t dirPathPos = 0;
	for (int32_t i = filePathLength - 1; i >= 0; i--)
	{
		if ((pFilePath[i] == L'\\') || (pFilePath[i] == L'/'))
		{
			dirPathPos = i;
			break;
		}
	}

	if (dirPathPos > 0)
	{
		dirPath = pFilePath;
		dirPath.resize(dirPathPos + 1);
		dirPath.shrink_to_fit();
	}

	// ----------------------------------------------------------------------------------------------------
	// 展開
	// ----------------------------------------------------------------------------------------------------

	if(Import(dirPath.c_str(), bufferSize, pBuffer, importDesc, polygons, nodes, materials) == false)
	{
		SAFE_DELETE_ARRAY(pBuffer);
		return V3D_ERROR_FAIL;
	}

	SAFE_DELETE_ARRAY(pBuffer);

	// ----------------------------------------------------------------------------------------------------
	// 回転
	// ----------------------------------------------------------------------------------------------------

	{
		Mesh::OptPolygon* pPolygon = polygons.data();
		Mesh::OptPolygon* pPolygonEnd = pPolygon + polygons.size();

		while (pPolygon != pPolygonEnd)
		{
			MeshVertex* pVertex = &pPolygon->vertices[0];
			MeshVertex* pVertexEnd = pVertex + 3;

			while (pVertex != pVertexEnd)
			{
				pVertex->pos = importDesc.matrix * pVertex->pos;

				if (importDesc.flags & MESH_IMPORT_INVERT_TEX_U) { pVertex->uv.x = 1.0f - pVertex->uv.x; }
				if (importDesc.flags & MESH_IMPORT_INVERT_TEX_V) { pVertex->uv.y = 1.0f - pVertex->uv.y; }

				pVertex++;
			}

			pPolygon++;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// 法線、接線空間を求める
	// ----------------------------------------------------------------------------------------------------

	{
		size_t i0;
		size_t i1;
		size_t i2;

		if (importDesc.flags & MESH_IMPORT_FLIP_FACE)
		{
			i0 = 2;
			i1 = 1;
			i2 = 0;
		}
		else
		{
			i0 = 0;
			i1 = 1;
			i2 = 2;
		}

		Mesh::OptPolygon* pPolygon = polygons.data();
		Mesh::OptPolygon* pPolygonEnd = pPolygon + polygons.size();

		while (pPolygon != pPolygonEnd)
		{
			MeshVertex* vertices = &pPolygon->vertices[0];

			// 法線
			Vector3 ab;
			Vector3 bc;

			if (importDesc.flags & MESH_IMPORT_INVERT_NORMAL)
			{
				ab = vertices[i0].pos - vertices[i1].pos;
				bc = vertices[i2].pos - vertices[i1].pos;
			}
			else
			{
				ab = vertices[i0].pos - vertices[i1].pos;
				bc = vertices[i1].pos - vertices[i2].pos;
			}

			Vector3 normal = Vector3::Cross(ab, bc).ToNormalize();

			vertices[i0].normal = normal;
			vertices[i1].normal = normal;
			vertices[i2].normal = normal;

			// 接線空間
			MeshVertex* pVertices[3] =
			{
				&(vertices[i0]),
				&(vertices[i1]),
				&(vertices[i2]),
			};

			Vector3 p0 = pVertices[0]->pos;
			Vector3 p1 = pVertices[1]->pos;
			Vector3 p2 = pVertices[2]->pos;

			Vector2 t0 = pVertices[0]->uv;
			Vector2 t1 = pVertices[1]->uv;
			Vector2 t2 = pVertices[2]->uv;

			Vector3 cp0[3] =
			{
				Vector3(p0.x, t0.x, t0.y),
				Vector3(p0.y, t0.x, t0.y),
				Vector3(p0.z, t0.x, t0.y),
			};

			Vector3 cp1[3] =
			{
				Vector3(p1.x, t1.x, t1.y),
				Vector3(p1.y, t1.x, t1.y),
				Vector3(p1.z, t1.x, t1.y),
			};

			Vector3 cp2[3] =
			{
				Vector3(p2.x, t2.x, t2.y),
				Vector3(p2.y, t2.x, t2.y),
				Vector3(p2.z, t2.x, t2.y),
			};

			Vector3 v1;
			Vector3 v2;
			Vector3 abc;

			float u[3];
			float v[3];

			for (uint32_t i = 0; i < 3; i++)
			{
				v1 = cp1[i] - cp0[i];
				v2 = cp2[i] - cp1[i];

				abc = Vector3::Cross(v1, v2);

				if ((-FLOAT_EPSILON <= abc.x) && (FLOAT_EPSILON >= abc.x))
				{
					//縮退しているポリゴン
					u[i] = 1.0f;
					v[i] = 1.0f;
				}
				else
				{
					u[i] = (abc.y / abc.x);
					v[i] = (abc.z / abc.x);
				}
			}

			for (uint32_t i = 0; i < 3; i++)
			{
				pVertices[i]->tangent.x += u[0];
				pVertices[i]->tangent.y += u[1];
				pVertices[i]->tangent.z += u[2];
				pVertices[i]->tangent.Normalize();

				pVertices[i]->binormal.x += v[0];
				pVertices[i]->binormal.y += v[1];
				pVertices[i]->binormal.z += v[2];
				pVertices[i]->binormal.Normalize();
			}

			pPolygon++;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// マテリアル
	// ----------------------------------------------------------------------------------------------------

	if (materials.empty() == true)
	{
		Mesh::OptMaterial materialOpt{};
		materialOpt.name = L"default";
		materialOpt.ambientColor.Set(0.0f, 0.0f, 0.0f);
		materialOpt.diffuseColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
		materialOpt.specularColor.Set(0.0f, 0.0f, 0.0f);
		materialOpt.emissiveColor.Set(0.0f, 0.0f, 0.0f);
		materialOpt.shininess = 1.0f;
		materialOpt.specularPower = 50.0f;

		materials.push_back(materialOpt);
	}

	return V3D_OK;
}

bool Mesh::Import(
	const wchar_t* pDriPath,
	uint64_t bufferSize, const uint8_t* pBuffer,
	const MeshImportDesc& importDesc,
	std::vector<Mesh::OptPolygon>& polygons,
	std::vector<Mesh::OptNode>& nodes,
	std::vector<Mesh::OptMaterial>& materials)
{
#ifdef V3D64
	ObjParser parser(bufferSize, pBuffer);
#else //V3D64
	ObjParser parser(TO_UI32(bufferSize), pBuffer);
#endif //V3D64

	std::vector<MeshVertex> tempVertices;
	tempVertices.reserve(4);

	std::vector<Vector3> positions;
	std::vector<Vector2> textures;
	std::vector<Vector3> normals;

	uint32_t currentMaterialIndex = 0;
	uint32_t objectNodeIndex = ~0U;
	uint32_t currentNodeIndex = ~0U;

	Mesh::OptNode* pNode = nullptr;

	while (parser.Read() == true)
	{
		const char* pStr = parser.Pop();

		if ((pStr == nullptr) || (pStr[0] == '#'))
		{
		}
		else if (strcmp(pStr, "mtllib") == 0)
		{
			// マテリアルライブラリ
			std::wstring materialLib;

			if (ImportName(&parser, materialLib) == false)
			{
				return false;
			}

			std::wstringstream mlStream;
			mlStream << pDriPath << materialLib.c_str();

			if(ImportMaterial(mlStream.str().c_str(), materials) == false)
			{
				return false;
			}
		}
		else if (strcmp(pStr, "v") == 0)
		{
			// 頂点座標
			float data[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

			for (uint32_t i = 0; i < 4; i++)
			{
				pStr = parser.Pop();
				if (pStr == nullptr)
				{
					break;
				}

				data[i] = static_cast<float>(atof(pStr));
			}

			float w = FLOAT_RECIPROCAL(data[3]);
			positions.push_back(Vector3(data[0] * w, data[1] * w, data[2] * w));
		}
		else if (strcmp(pStr, "vt") == 0)
		{
			// 頂点テクスチャ座標
			float data[3] = { 0.0f, 0.0f, 0.0f };

			for (uint32_t i = 0; i < 3; i++)
			{
				pStr = parser.Pop();
				if (pStr == nullptr)
				{
					break;
				}

				data[i] = static_cast<float>(atof(pStr));
			}

			textures.push_back(Vector2(data[0], data[1]));
		}
		else if (strcmp(pStr, "vn") == 0)
		{
			// 頂点法線
			float data[3] = { 0.0f, 0.0f, 0.0f };

			for (uint32_t i = 0; i < 3; i++)
			{
				pStr = parser.Pop();
				if (pStr == nullptr)
				{
					break;
				}

				data[i] = static_cast<float>(atof(pStr));
			}

			normals.push_back(Vector3(data[0], data[1], data[2]));
		}
		else if (strcmp(pStr, "o") == 0)
		{
			// オブジェクト
			Mesh::OptNode node{};

			if (ImportName(&parser, node.name) == false)
			{
				return false;
			}

			node.parentNode = objectNodeIndex;
			node.firstPolygon = TO_UI32(polygons.size());
			node.polygonCount = 0;

			objectNodeIndex = currentNodeIndex = TO_UI32(nodes.size());
			nodes.push_back(node);

			pNode = &nodes.back();

			std::wstringstream stringStream;
			stringStream << L"object : " << node.name << L"\n";
			OutputDebugStringW(stringStream.str().c_str());
		}
		else if (strcmp(pStr, "g") == 0)
		{
			// グループ
			if (objectNodeIndex == ~0U)
			{
				Mesh::OptNode node{};
				node.name = L"root";
				node.parentNode = ~0U;
				node.firstPolygon = TO_UI32(polygons.size());
				node.polygonCount = 0;

				objectNodeIndex = TO_UI32(nodes.size());
				nodes.push_back(node);

				OutputDebugString(L"object : root\n");
			}

			Mesh::OptNode node{};

			if (ImportName(&parser, node.name) == false)
			{
				return false;
			}

			node.parentNode = objectNodeIndex;
			node.firstPolygon = TO_UI32(polygons.size());
			node.polygonCount = 0;

			currentNodeIndex = TO_UI32(nodes.size());
			nodes.push_back(node);

			pNode = &nodes.back();

			std::wstringstream stringStream;
			stringStream << L"  group : " << node.name << L"\n";
			OutputDebugStringW(stringStream.str().c_str());
		}
		else if (strcmp(pStr, "usemtl") == 0)
		{
			// マテリアルを設定
			if (currentNodeIndex == ~0U)
			{
				return false;
			}

			std::wstring materialName;
			if (ImportName(&parser, materialName) == false)
			{
				return false;
			}

			uint32_t materialCount = static_cast<uint32_t>(materials.size());
			uint32_t materialIndex = ~0U;
			for (uint32_t i = 0; i < materialCount; i++)
			{
				if (materialName == materials[i].name)
				{
					materialIndex = i;
					break;
				}
			}

			if (materialIndex == ~0U)
			{
				materialIndex = 0;
//				return false;
			}

			currentMaterialIndex = materialIndex;

			std::wstringstream stringStreamW;
			stringStreamW << L"    usemtl : " << materialName << L"\n";
			OutputDebugString(stringStreamW.str().c_str());
		}
		else if (strcmp(pStr, "s") == 0)
		{
			// スムージング ( 1 or off )
			parser.Pop();
		}
		else if (strcmp(pStr, "f") == 0)
		{
			// フェイス
			bool hasTexture = (currentMaterialIndex != ~0U)? materials[currentMaterialIndex].hasTexture : false;
			bool faceLoop = true;

			tempVertices.clear();

			while (faceLoop == true)
			{
				long long indices[3] = { 0, 0, 0 };

				for (uint32_t i = 0; i < 3; i++)
				{
					pStr = parser.Pop();

					if (pStr == nullptr)
					{
						faceLoop = false;
						break;
					}
					else if (strcmp(pStr, "/") == 0)
					{
						continue;
					}
					else
					{
						indices[i] = atoll(pStr);
					}

					if (i != 2)
					{
						pStr = parser.Look();
						if ((pStr != nullptr) && (strcmp(pStr, "/") == 0))
						{
							// 次がある
							parser.Pop();
						}
						else
						{
							// 次はない
							break;
						}
					}
				}

				MeshVertex vertex{};

				if (indices[0] == -1)
				{
					vertex.pos = positions.back();
				}
				else if (indices[0] > 0)
				{
					vertex.pos = positions[static_cast<uint32_t>(indices[0] - 1)];
				}
				else if (indices[0] < 0)
				{
					long long index = static_cast<long long>(positions.size()) + indices[0];
					if (index >= 0)
					{
						vertex.pos = positions[static_cast<uint32_t>(static_cast<long long>(positions.size()) + indices[0])];
					}
					else
					{
						vertex.pos = positions.back();
					}
				}

				if (hasTexture == true)
				{
					if (indices[1] == -1)
					{
						vertex.uv = textures.back();
					}
					else if (indices[1] > 0)
					{
						vertex.uv = textures[static_cast<uint32_t>(indices[1] - 1)];
					}
					else if (indices[1] < 0)
					{
						long long index = static_cast<long long>(textures.size()) + indices[1];
						if (index >= 0)
						{
							vertex.uv = textures[static_cast<uint32_t>(static_cast<long long>(textures.size()) + indices[1])];
						}
						else
						{
							vertex.uv = textures.back();
						}
					}
				}

				if (indices[2] == -1)
				{
					vertex.normal = normals.back();
				}
				else if (indices[2] > 0)
				{
					vertex.normal = normals[static_cast<uint32_t>(indices[2] - 1)];
				}
				else if (indices[2] < 0)
				{
					long long index = static_cast<long long>(normals.size()) + indices[2];
					if (index >= 0)
					{
						vertex.normal = normals[static_cast<uint32_t>(static_cast<long long>(normals.size()) + indices[2])];
					}
					else
					{
						vertex.normal = normals.back();
					}
				}

				if ((indices[0] != 0) || (indices[1] != 0) || (indices[2] != 0))
				{
					tempVertices.push_back(vertex);
				}
			}

			uint32_t vertexCount = static_cast<uint32_t>(tempVertices.size());

			Mesh::OptPolygon polygon;
			polygon.material = currentMaterialIndex;

			if (vertexCount == 3)
			{
				if (importDesc.flags & MESH_IMPORT_FLIP_FACE)
				{
					polygon.vertices[0] = tempVertices[2];
					polygon.vertices[1] = tempVertices[1];
					polygon.vertices[2] = tempVertices[0];
				}
				else
				{
					polygon.vertices[0] = tempVertices[0];
					polygon.vertices[1] = tempVertices[1];
					polygon.vertices[2] = tempVertices[2];
				}

				polygons.push_back(polygon);
				pNode->polygonCount += 1;
			}
			else if (vertexCount == 4)
			{
				if (importDesc.flags & MESH_IMPORT_FLIP_FACE)
				{
					polygon.vertices[0] = tempVertices[2];
					polygon.vertices[1] = tempVertices[1];
					polygon.vertices[2] = tempVertices[0];
					polygons.push_back(polygon);

					polygon.vertices[0] = tempVertices[0];
					polygon.vertices[1] = tempVertices[3];
					polygon.vertices[2] = tempVertices[2];
					polygons.push_back(polygon);
				}
				else
				{
					polygon.vertices[0] = tempVertices[0];
					polygon.vertices[1] = tempVertices[1];
					polygon.vertices[2] = tempVertices[2];
					polygons.push_back(polygon);

					polygon.vertices[0] = tempVertices[2];
					polygon.vertices[1] = tempVertices[3];
					polygon.vertices[2] = tempVertices[0];
					polygons.push_back(polygon);
				}

				pNode->polygonCount += 2;
			}
			else if (vertexCount > 4)
			{
				uint32_t pointCount = vertexCount - 3;
				uint32_t lastPoint = vertexCount - 1;

				if (importDesc.flags & MESH_IMPORT_FLIP_FACE)
				{
					for (uint32_t i = 0; i < pointCount; i++)
					{
						polygon.vertices[0] = tempVertices[lastPoint];
						polygon.vertices[1] = tempVertices[i + 1];
						polygon.vertices[2] = tempVertices[i + 0];
						polygons.push_back(polygon);
					}
				}
				else
				{
					for (uint32_t i = 0; i < pointCount; i++)
					{
						polygon.vertices[0] = tempVertices[i + 0];
						polygon.vertices[1] = tempVertices[i + 1];
						polygon.vertices[2] = tempVertices[lastPoint];
						polygons.push_back(polygon);
					}
				}

				pNode->polygonCount += pointCount;
			}
			else if (vertexCount == 0)
			{
			}
			else
			{
				return false;
			}
		}
		else if (strcmp(pStr, "l") == 0)
		{
			//ライン
		}
		else
		{
			return false;
		}
	}

	return true;
}

void Mesh::CreateSource(
	const MeshImportDesc& importDesc,
	std::vector<Mesh::OptPolygon>::iterator& it_polygon_begin, std::vector<Mesh::OptPolygon>::iterator& it_polygon_end,
	std::vector<MeshVertex>& dstVertices, std::vector<uint32_t>& dstIndices, std::vector<MeshSubset>& dstSubsets, Vector3* pDstCenter, Vector3* pDstHalfExtent)
{
	// ----------------------------------------------------------------------------------------------------
	// 最適化
	// ----------------------------------------------------------------------------------------------------

	std::sort(it_polygon_begin, it_polygon_end, [](const Mesh::OptPolygon& lh, const Mesh::OptPolygon& rh) { return lh.material < rh.material; });

	if (importDesc.flags & MESH_IMPORT_NO_OPTIMIZE)
	{
		size_t polygonCount = std::distance(it_polygon_begin, it_polygon_end);
		size_t vertexCount = polygonCount * 3;

		Mesh::OptPolygon* pPolygon = &(*it_polygon_begin);
		Mesh::OptPolygon* pPolygonEnd = pPolygon + polygonCount;

		MeshSubset dummySubset;
		dummySubset.material = ~0U;
		dummySubset.indexCount = TO_UI32(dstIndices.size());
		dummySubset.firstIndex = 0;

		MeshSubset* pSubset = &dummySubset;

		uint32_t indexOffset = TO_UI32(dstVertices.size());

		while (pPolygon != pPolygonEnd)
		{
			if (pSubset->material != pPolygon->material)
			{
				MeshSubset preSubset = *pSubset;

				dstSubsets.push_back(MeshSubset{});
				pSubset = &dstSubsets.back();

				pSubset->material = pPolygon->material;
				pSubset->indexCount = 0;
				pSubset->firstIndex = preSubset.firstIndex + preSubset.indexCount;
			}

			dstVertices.push_back(pPolygon->vertices[0]);
			dstVertices.push_back(pPolygon->vertices[1]);
			dstVertices.push_back(pPolygon->vertices[2]);

			dstIndices.push_back(indexOffset + 0);
			dstIndices.push_back(indexOffset + 1);
			dstIndices.push_back(indexOffset + 2);
			indexOffset += 3;

			pSubset->indexCount += 3;

			pPolygon++;
		}
	}
	else
	{
		Optimize(importDesc.flags, cosf(importDesc.smoosingAngle), it_polygon_begin, it_polygon_end, dstVertices, dstIndices, dstSubsets);
	}

	// ----------------------------------------------------------------------------------------------------
	// 大きさを求める
	// ----------------------------------------------------------------------------------------------------

	{
		MeshVertex* pVertex = dstVertices.data();
		MeshVertex* pVertexEnd = pVertex + dstVertices.size();

		Vector3 minimum(+FLT_MAX, +FLT_MAX, +FLT_MAX);
		Vector3 maximum(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		while (pVertex != pVertexEnd)
		{
			minimum = Vector3::Minimum(minimum, pVertex->pos);
			maximum = Vector3::Maximum(maximum, pVertex->pos);
			pVertex++;
		}

		*pDstCenter = (minimum + maximum) * 0.5f;

		pDstHalfExtent->x = fabs(minimum.x - pDstCenter->x);
		pDstHalfExtent->y = fabs(minimum.y - pDstCenter->y);
		pDstHalfExtent->z = fabs(minimum.z - pDstCenter->z);
	}
}

V3D_RESULT Mesh::CreateVertexIndexBuffer(
	std::vector<MeshVertex>& vertices, std::vector<uint32_t>& indices,
	IV3DBuffer** ppDstBuffer, uint64_t* pDstVertexOffset, uint64_t* pDstVertexSize,
	uint64_t* pDstIndexOffset, uint64_t* pDstIndexSize, V3D_INDEX_TYPE* pDstIndexType)
{
	// ----------------------------------------------------------------------------------------------------
	// インデックスタイプ
	// ----------------------------------------------------------------------------------------------------

	std::vector<uint16_t> indices_u16;

	if (indices.size() <= USHRT_MAX)
	{
		std::vector<uint32_t>::iterator it_begin = indices.begin();
		std::vector<uint32_t>::iterator it_end = indices.end();
		std::vector<uint32_t>::iterator it;

		indices_u16.reserve(indices.size());

		for (it = it_begin; it != it_end; ++it)
		{
			indices_u16.push_back(static_cast<uint16_t>(*it));
		}

		*pDstIndexType = V3D_INDEX_TYPE_UINT16;
	}
	else
	{
		size_t indexCount = indices.size();

		indices_u16.resize(indices.size() * 2);
		uint32_t* pIndices_u32 = reinterpret_cast<uint32_t*>(indices_u16.data());

		for (size_t i = 0; i < indexCount; i++)
		{
			pIndices_u32[i] = indices[i];
		}

		*pDstIndexType = V3D_INDEX_TYPE_UINT32;
	}

	// ----------------------------------------------------------------------------------------------------
	// バーテックス、インデックスバッファを作成
	// ----------------------------------------------------------------------------------------------------

	Array1<BufferSubresourceDesc, 2> tempBufferSubresources;
	tempBufferSubresources[0].usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_VERTEX;
	tempBufferSubresources[0].size = sizeof(MeshVertex) * vertices.size();
	tempBufferSubresources[0].count = 1;
	tempBufferSubresources[1].usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_INDEX;
	tempBufferSubresources[1].size = sizeof(uint16_t) * indices_u16.size();
	tempBufferSubresources[1].count = 1;

	Array1<BufferMemoryLayout, 2> tempBufferMemoryLayouts;
	uint64_t tempBufferMemorySize;

	CalcBufferMemoryLayout(
		m_pGraphicsManager->GetDevicePtr(),
		V3D_MEMORY_PROPERTY_HOST_VISIBLE,
		TO_UI32(tempBufferSubresources.size()),
		tempBufferSubresources.data(),
		tempBufferMemoryLayouts.data(),
		&tempBufferMemorySize);

	// テンポラリバッファ
	V3DBufferDesc tempBufferDesc{};
	tempBufferDesc.usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_VERTEX | V3D_BUFFER_USAGE_INDEX;
	tempBufferDesc.size = tempBufferMemorySize;

	IV3DBuffer* pTempBuffer;

	V3D_RESULT result = m_pGraphicsManager->GetDevicePtr()->CreateBuffer(tempBufferDesc, &pTempBuffer);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pTempBuffer);
		return result;
	}

	result = m_pGraphicsManager->GetDevicePtr()->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_HOST_VISIBLE, pTempBuffer);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pTempBuffer);
		return result;
	}

	void* pMemory;
	result = pTempBuffer->Map(0, pTempBuffer->GetResourceDesc().memorySize, &pMemory);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pTempBuffer);
		return result;
	}

	uint8_t* pAddr = static_cast<uint8_t*>(pMemory);

	// バーテックスをコピー
	MemCopy(pAddr + tempBufferMemoryLayouts[0].offset, tempBufferMemoryLayouts[0].stride, vertices.data(), tempBufferSubresources[0].size);

	// インデックスをコピー
	MemCopy(pAddr + tempBufferMemoryLayouts[1].offset, tempBufferMemoryLayouts[1].stride, indices_u16.data(), tempBufferSubresources[1].size);

	result = pTempBuffer->Unmap();
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pTempBuffer);
		return result;
	}

	// 描画用バッファを作成
	Array1<BufferSubresourceDesc, 2> bufferSubresources;
	bufferSubresources[0].usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_TRANSFER_DST | V3D_BUFFER_USAGE_VERTEX;
	bufferSubresources[0].size = sizeof(MeshVertex) * vertices.size();
	bufferSubresources[0].count = 1;
	bufferSubresources[1].usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_TRANSFER_DST | V3D_BUFFER_USAGE_INDEX;
	bufferSubresources[1].size = sizeof(uint16_t) * indices_u16.size();
	bufferSubresources[1].count = 1;

	Array1<BufferMemoryLayout, 2> bufferMemoryLayouts;
	uint64_t bufferMemorySize;

	CalcBufferMemoryLayout(
		m_pGraphicsManager->GetDevicePtr(),
		V3D_MEMORY_PROPERTY_DEVICE_LOCAL,
		TO_UI32(bufferSubresources.size()),
		bufferSubresources.data(),
		bufferMemoryLayouts.data(),
		&bufferMemorySize);

	V3DBufferDesc bufferDesc{};
	bufferDesc.usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_TRANSFER_DST | V3D_BUFFER_USAGE_VERTEX | V3D_BUFFER_USAGE_INDEX;
	bufferDesc.size = bufferMemorySize;

	result = m_pGraphicsManager->GetDevicePtr()->CreateBuffer(bufferDesc, ppDstBuffer);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pTempBuffer);
		return result;
	}

	result = m_pGraphicsManager->GetDevicePtr()->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_DEVICE_LOCAL, *ppDstBuffer);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(*ppDstBuffer);
		SAFE_RELEASE(pTempBuffer);
		return result;
	}

	// テンポラリバッファの内容を描画用バッファに転送
	IV3DCommandBuffer* pCommandBuffer = m_pGraphicsManager->ResetCommandBuffer();
	if (pCommandBuffer == nullptr)
	{
		SAFE_RELEASE(*ppDstBuffer);
		SAFE_RELEASE(pTempBuffer);
		return V3D_ERROR_FAIL;
	}

	Array1<V3DCopyBufferRange, 2> copyBufferRanges;
	copyBufferRanges[0].dstOffset = bufferMemoryLayouts[0].offset;
	copyBufferRanges[0].srcOffset = tempBufferMemoryLayouts[0].offset;
	copyBufferRanges[0].size = tempBufferSubresources[0].size;
	copyBufferRanges[1].dstOffset = bufferMemoryLayouts[1].offset;
	copyBufferRanges[1].srcOffset = tempBufferMemoryLayouts[1].offset;
	copyBufferRanges[1].size = tempBufferSubresources[1].size;

	pCommandBuffer->CopyBuffer(*ppDstBuffer, pTempBuffer, TO_UI32(copyBufferRanges.size()), copyBufferRanges.data());

	if (m_pGraphicsManager->FlushCommandBuffer() == false)
	{
		SAFE_RELEASE(*ppDstBuffer);
		SAFE_RELEASE(pTempBuffer);
		return V3D_ERROR_FAIL;
	}

	SAFE_RELEASE(pTempBuffer);

	*pDstVertexOffset = bufferMemoryLayouts[0].offset;
	*pDstVertexSize = bufferSubresources[0].size;
	*pDstIndexOffset = bufferMemoryLayouts[1].offset;
	*pDstIndexSize = bufferSubresources[1].size;

	return V3D_OK;
}

bool Mesh::DownloadVertexIndexBuffer(IV3DBuffer* pSrcBuffer, IV3DBuffer** ppDstBuffer)
{
	V3DBufferDesc bufferDesc{};
	bufferDesc.size = pSrcBuffer->GetDesc().size;
	bufferDesc.usageFlags = V3D_BUFFER_USAGE_TRANSFER_DST | V3D_BUFFER_USAGE_INDEX | V3D_BUFFER_USAGE_VERTEX;

	IV3DBuffer* pBuffer;

	V3D_RESULT result = m_pGraphicsManager->GetDevicePtr()->CreateBuffer(bufferDesc, &pBuffer);
	if (result != V3D_OK)
	{
		return false;
	}

	result = m_pGraphicsManager->GetDevicePtr()->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_HOST_VISIBLE, pBuffer);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pBuffer);
		return false;
	}

	IV3DCommandBuffer* pCommandBuffer = m_pGraphicsManager->ResetCommandBuffer();
	if (pCommandBuffer == nullptr)
	{
		SAFE_RELEASE(pBuffer);
		return false;
	}

	V3DPipelineBarrier pipelineBarrier{};

	V3DBufferMemoryBarrier memoryBarrier{};
	memoryBarrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
	memoryBarrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
	memoryBarrier.offset = 0;
	memoryBarrier.size = bufferDesc.size;

	pipelineBarrier.srcStageMask = V3D_PIPELINE_STAGE_VERTEX_INPUT;
	pipelineBarrier.dstStageMask = V3D_PIPELINE_STAGE_TRANSFER;
	memoryBarrier.srcAccessMask = V3D_ACCESS_VERTEX_READ | V3D_ACCESS_INDEX_READ;
	memoryBarrier.dstAccessMask = V3D_ACCESS_TRANSFER_READ;
	memoryBarrier.pBuffer = pSrcBuffer;
	pCommandBuffer->Barrier(pipelineBarrier, memoryBarrier);

	pipelineBarrier.srcStageMask = V3D_PIPELINE_STAGE_VERTEX_INPUT;
	pipelineBarrier.dstStageMask = V3D_PIPELINE_STAGE_TRANSFER;
	memoryBarrier.srcAccessMask = V3D_ACCESS_VERTEX_READ | V3D_ACCESS_INDEX_READ;
	memoryBarrier.dstAccessMask = V3D_ACCESS_TRANSFER_WRITE;
	memoryBarrier.pBuffer = pBuffer;
	pCommandBuffer->Barrier(pipelineBarrier, memoryBarrier);

	pCommandBuffer->CopyBuffer(pBuffer, 0, pSrcBuffer, 0, bufferDesc.size);

	pipelineBarrier.srcStageMask = V3D_PIPELINE_STAGE_TRANSFER;
	pipelineBarrier.dstStageMask = V3D_PIPELINE_STAGE_HOST;
	memoryBarrier.srcAccessMask = V3D_ACCESS_TRANSFER_WRITE;
	memoryBarrier.dstAccessMask = V3D_ACCESS_HOST_READ;
	memoryBarrier.pBuffer = pBuffer;
	pCommandBuffer->Barrier(pipelineBarrier, memoryBarrier);

	pipelineBarrier.srcStageMask = V3D_PIPELINE_STAGE_TRANSFER;
	pipelineBarrier.dstStageMask = V3D_PIPELINE_STAGE_VERTEX_INPUT;
	memoryBarrier.srcAccessMask = V3D_ACCESS_TRANSFER_READ;
	memoryBarrier.dstAccessMask = V3D_ACCESS_VERTEX_READ | V3D_ACCESS_INDEX_READ;
	memoryBarrier.pBuffer = pSrcBuffer;
	pCommandBuffer->Barrier(pipelineBarrier, memoryBarrier);

	if (m_pGraphicsManager->FlushCommandBuffer() == false)
	{
		SAFE_RELEASE(pBuffer);
		return false;
	}

	*ppDstBuffer = pBuffer;

	return true;
}

bool Mesh::UploadVertexIndexBuffer(uint64_t verticesSize, const void* pVertices, uint64_t indicesSize, const void* pIndices, IV3DBuffer** ppDstBuffer, uint64_t* pDstVertexOffset, uint64_t* pDstIndexOffset)
{
	// ----------------------------------------------------------------------------------------------------
	// 転送元のテンポラリバッファーを作成
	// ----------------------------------------------------------------------------------------------------

	Array1<BufferSubresourceDesc, 2> tempBufferSubresources;
	tempBufferSubresources[0].usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC;// | V3D_BUFFER_USAGE_VERTEX;
	tempBufferSubresources[0].size = verticesSize;
	tempBufferSubresources[0].count = 1;
	tempBufferSubresources[1].usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC;// | V3D_BUFFER_USAGE_INDEX;
	tempBufferSubresources[1].size = indicesSize;
	tempBufferSubresources[1].count = 1;

	Array1<BufferMemoryLayout, 2> tempBufferMemoryLayouts;
	uint64_t tempBufferMemorySize;

	CalcBufferMemoryLayout(
		m_pGraphicsManager->GetDevicePtr(),
		V3D_MEMORY_PROPERTY_HOST_VISIBLE,
		TO_UI32(tempBufferSubresources.size()),
		tempBufferSubresources.data(),
		tempBufferMemoryLayouts.data(),
		&tempBufferMemorySize);

	V3DBufferDesc tempBufferDesc{};
	tempBufferDesc.usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC;// | V3D_BUFFER_USAGE_VERTEX | V3D_BUFFER_USAGE_INDEX;
	tempBufferDesc.size = tempBufferMemorySize;

	IV3DBuffer* pTempBuffer;

	V3D_RESULT result = m_pGraphicsManager->GetDevicePtr()->CreateBuffer(tempBufferDesc, &pTempBuffer);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pTempBuffer);
		return false;
	}

	result = m_pGraphicsManager->GetDevicePtr()->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_HOST_VISIBLE, pTempBuffer);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pTempBuffer);
		return false;
	}

	void* pMemory;
	result = pTempBuffer->Map(0, pTempBuffer->GetResourceDesc().memorySize, &pMemory);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pTempBuffer);
		return false;
	}

	uint8_t* pAddr = static_cast<uint8_t*>(pMemory);

	// バーテックスをコピー
	MemCopy(pAddr + tempBufferMemoryLayouts[0].offset, tempBufferMemoryLayouts[0].stride, pVertices, tempBufferSubresources[0].size);

	// インデックスをコピー
	MemCopy(pAddr + tempBufferMemoryLayouts[1].offset, tempBufferMemoryLayouts[1].stride, pIndices, tempBufferSubresources[1].size);

	result = pTempBuffer->Unmap();
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pTempBuffer);
		return false;
	}

	// ----------------------------------------------------------------------------------------------------
	// 転送先の描画用バッファーを作成
	// ----------------------------------------------------------------------------------------------------

	Array1<BufferSubresourceDesc, 2> bufferSubresources;
	bufferSubresources[0].usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_TRANSFER_DST | V3D_BUFFER_USAGE_VERTEX;
	bufferSubresources[0].size = verticesSize;
	bufferSubresources[0].count = 1;
	bufferSubresources[1].usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_TRANSFER_DST | V3D_BUFFER_USAGE_INDEX;
	bufferSubresources[1].size = indicesSize;
	bufferSubresources[1].count = 1;

	Array1<BufferMemoryLayout, 2> bufferMemoryLayouts;
	uint64_t bufferMemorySize;

	CalcBufferMemoryLayout(
		m_pGraphicsManager->GetDevicePtr(),
		V3D_MEMORY_PROPERTY_DEVICE_LOCAL,
		TO_UI32(bufferSubresources.size()),
		bufferSubresources.data(),
		bufferMemoryLayouts.data(),
		&bufferMemorySize);

	V3DBufferDesc bufferDesc{};
	bufferDesc.usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_TRANSFER_DST | V3D_BUFFER_USAGE_VERTEX | V3D_BUFFER_USAGE_INDEX;
	bufferDesc.size = bufferMemorySize;

	result = m_pGraphicsManager->GetDevicePtr()->CreateBuffer(bufferDesc, ppDstBuffer);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pTempBuffer);
		return false;
	}

	result = m_pGraphicsManager->GetDevicePtr()->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_DEVICE_LOCAL, *ppDstBuffer);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(*ppDstBuffer);
		SAFE_RELEASE(pTempBuffer);
		return false;
	}

	// ----------------------------------------------------------------------------------------------------
	// テンポラリバッファーの内容を描画用バッファーに転送
	// ----------------------------------------------------------------------------------------------------

	IV3DCommandBuffer* pCommandBuffer = m_pGraphicsManager->ResetCommandBuffer();
	if (pCommandBuffer == nullptr)
	{
		SAFE_RELEASE(*ppDstBuffer);
		SAFE_RELEASE(pTempBuffer);
		return false;
	}

	// コピー
	Array1<V3DCopyBufferRange, 2> copyBufferRanges;
	copyBufferRanges[0].dstOffset = bufferMemoryLayouts[0].offset;
	copyBufferRanges[0].srcOffset = tempBufferMemoryLayouts[0].offset;
	copyBufferRanges[0].size = tempBufferSubresources[0].size;
	copyBufferRanges[1].dstOffset = bufferMemoryLayouts[1].offset;
	copyBufferRanges[1].srcOffset = tempBufferMemoryLayouts[1].offset;
	copyBufferRanges[1].size = tempBufferSubresources[1].size;
	pCommandBuffer->CopyBuffer(*ppDstBuffer, pTempBuffer, TO_UI32(copyBufferRanges.size()), copyBufferRanges.data());

	V3DPipelineBarrier pipelineBarrier{};
	pipelineBarrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
	pipelineBarrier.dstStageMask = V3D_PIPELINE_STAGE_VERTEX_INPUT;

	V3DBufferMemoryBarrier memoryBarrier{};
	memoryBarrier.srcAccessMask = V3D_ACCESS_TRANSFER_WRITE;
	memoryBarrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
	memoryBarrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;

	// バーテックスのバリア
	memoryBarrier.dstAccessMask = V3D_ACCESS_VERTEX_READ;
	memoryBarrier.pBuffer = *ppDstBuffer;
	memoryBarrier.offset = bufferMemoryLayouts[0].offset;
	memoryBarrier.size = bufferMemoryLayouts[0].stride;
	pCommandBuffer->Barrier(pipelineBarrier, memoryBarrier);

	// インデックスのバリア
	memoryBarrier.dstAccessMask = V3D_ACCESS_INDEX_READ;
	memoryBarrier.pBuffer = *ppDstBuffer;
	memoryBarrier.offset = bufferMemoryLayouts[1].offset;
	memoryBarrier.size = bufferMemoryLayouts[1].stride;
	pCommandBuffer->Barrier(pipelineBarrier, memoryBarrier);

	if (m_pGraphicsManager->FlushCommandBuffer() == false)
	{
		SAFE_RELEASE(*ppDstBuffer);
		SAFE_RELEASE(pTempBuffer);
		return false;
	}

	SAFE_RELEASE(pTempBuffer);

	*pDstVertexOffset = bufferMemoryLayouts[0].offset;
	*pDstIndexOffset = bufferMemoryLayouts[1].offset;

	return true;
}

V3D_RESULT Mesh::CreateMaterials(const wchar_t* pFilePath, const MeshImportDesc& importDesc, std::vector<Mesh::OptMaterial>& materials)
{
	std::vector<Mesh::OptMaterial>::iterator it_begin = materials.begin();
	std::vector<Mesh::OptMaterial>::iterator it_end = materials.end();
	std::vector<Mesh::OptMaterial>::iterator it;

	for (it = it_begin; it != it_end; ++it)
	{
		Mesh::OptMaterial& srcMaterial = (*it);
		std::wstringstream stringStream;

		stringStream << L"material : name[" << srcMaterial.name << L"]\n";
		OutputDebugString(stringStream.str().c_str());

		stringStream.str(L"");
		stringStream << pFilePath << L"\\" << srcMaterial.name;
		MaterialPtr material = m_pMaterialManager->Create(stringStream.str().c_str(), srcMaterial.name.c_str());

		material->SetDiffuseColor(srcMaterial.diffuseColor);

		if (srcMaterial.diffuseColor.a < 1.0f)
		{
			material->SetBlendMode(BLEND_MODE_NORMAL);
			material->SetCullMode(V3D_CULL_MODE_NONE);
		}

		std::wstring baseTextureName;
		std::wstring baseTextureExt;
		bool baseTextureEnable;

		// ディフューズテクスチャ
		if (srcMaterial.diffuseTexture.empty() == false)
		{
			size_t separatePoint = ~0U;
			for (int32_t i = static_cast<int32_t>(srcMaterial.diffuseTexture.size()) - 1; i >= 0; i--)
			{
				if (srcMaterial.diffuseTexture[i] == L'.')
				{
					separatePoint = i;
					break;
				}
			}

			if (separatePoint == ~0U)
			{
				return V3D_ERROR_FAIL;
			}

			baseTextureName = srcMaterial.diffuseTexture;
			baseTextureName[separatePoint] = L'\0';
			baseTextureName.resize(separatePoint);
			baseTextureExt = &srcMaterial.diffuseTexture[separatePoint];
			baseTextureEnable = true;

			if ((importDesc.flags & MESH_IMPORT_NO_DIFFUSE_TEXTURE) == 0)
			{
				TexturePtr texture = m_pTextureManager->Load(srcMaterial.diffuseTexture.c_str());
				if (texture != nullptr)
				{
					material->SetDiffuseTexture(texture);
				}
				else
				{
					::OutputDebugStringW(srcMaterial.diffuseTexture.c_str());
					::OutputDebugStringW(L"\n");
				}
			}
		}
		else
		{
			baseTextureEnable = false;
		}

		// スペキュラー
		material->SetSpecularPower(srcMaterial.specularPower);
		material->SetShininess(srcMaterial.shininess);

		// スペキュラーテクスチャ
		if ((importDesc.flags & MESH_IMPORT_NO_SPECULAR_TEXTURE) == 0)
		{
			if (srcMaterial.specularTexture.empty() == false)
			{
				TexturePtr texture = m_pTextureManager->Load(srcMaterial.specularTexture.c_str());
				if (texture != nullptr)
				{
					material->SetSpecularTexture(texture);
				}
				else
				{
					::OutputDebugStringW(srcMaterial.specularTexture.c_str());
					::OutputDebugStringW(L"\n");
				}
			}
			else
			{
				std::wstringstream textureFilePath;
				textureFilePath << baseTextureName << L"_s" << baseTextureExt;

				TexturePtr texture = m_pTextureManager->Load(textureFilePath.str().c_str());
				if (texture != nullptr)
				{
					material->SetSpecularTexture(texture);
				}
			}
		}

		// マスクテクスチャ
		if ((importDesc.flags & MESH_IMPORT_NO_MASK_TEXTURE) == 0)
		{
			if (srcMaterial.maskTexture.empty() == false)
			{
				material->SetCullMode(V3D_CULL_MODE_NONE);
				material->SetBlendMode(BLEND_MODE_NORMAL);

				TexturePtr texture = m_pTextureManager->Load(srcMaterial.maskTexture.c_str());
				if (texture != nullptr)
				{
					material->SetMaskTexture(texture);
				}
				else
				{
					::OutputDebugStringW(srcMaterial.maskTexture.c_str());
					::OutputDebugStringW(L"\n");
				}
			}
			else
			{
				std::wstringstream textureFilePath;
				textureFilePath << baseTextureName << L"_m" << baseTextureExt;

				TexturePtr texture = m_pTextureManager->Load(textureFilePath.str().c_str());
				if (texture != nullptr)
				{
					material->SetMaskTexture(texture);
				}
			}
		}

		// 法線テクスチャ
		if ((importDesc.flags & MESH_IMPORT_NO_NORMAL_TEXTURE) == 0)
		{
			if (srcMaterial.bumpTexture.empty() == false)
			{
				TexturePtr texture = m_pTextureManager->Load(srcMaterial.bumpTexture.c_str());
				if (texture != nullptr)
				{
					material->SetNormalTexture(texture);
				}
				else
				{
					::OutputDebugStringW(srcMaterial.bumpTexture.c_str());
					::OutputDebugStringW(L"\n");
				}
			}
			else if (baseTextureEnable == true)
			{
				std::wstringstream textureFilePath;
				textureFilePath << baseTextureName << L"_n" << baseTextureExt;

				TexturePtr texture = m_pTextureManager->Load(textureFilePath.str().c_str());
				if (texture != nullptr)
				{
					material->SetNormalTexture(texture);
				}
			}
		}

		V3D_RESULT result = material->Update();
		if (result != V3D_OK)
		{
			return result;
		}

		m_Materials.push_back(material);
	}

	return V3D_OK;
}

bool Mesh::CreateMaterials(const wchar_t* pFilePath, std::vector<Material::SerialData>& materials)
{
	ASSERT(m_Materials.size() == 0);

	std::vector<Material::SerialData>::iterator it_begin = materials.begin();
	std::vector<Material::SerialData>::iterator it_end = materials.end();
	std::vector<Material::SerialData>::iterator it;

	for (it = it_begin; it != it_end; ++it)
	{
		const Material::SerialData& srcMaterial = *it;

		std::wstringstream stringStream;

		stringStream << L"material : name[" << srcMaterial.name << L"]\n";
		OutputDebugString(stringStream.str().c_str());

		stringStream.str(L"");
		stringStream << pFilePath << L"\\" << srcMaterial.name;

		MaterialPtr material = m_pMaterialManager->CreateFromSerial(stringStream.str().c_str(), &srcMaterial);
		if (material == nullptr)
		{
			return false;
		}

		m_Materials.push_back(material);
	}

	return true;
}

void Mesh::CloneMaterials(Mesh* pMesh)
{
	ASSERT(pMesh->m_Materials.empty() == true);
	pMesh->m_Materials.assign(m_Materials.begin(), m_Materials.end());
}

bool Mesh::SerializeMaterials(std::vector<Material::SerialData>& materials)
{
	ASSERT(m_Materials.size() > 0);

	std::vector<MaterialPtr>::iterator it_begin = m_Materials.begin();
	std::vector<MaterialPtr>::iterator it_end = m_Materials.end();
	std::vector<MaterialPtr>::iterator it;

	for (it = it_begin; it != it_end; ++it)
	{
		Material::SerialData data{};
		
		if ((*it)->Serialize(&data) == false)
		{
			return false;
		}

		materials.push_back(data);
	}

	return true;
}

bool Mesh::ImportMaterial(const wchar_t* pFilePath, std::vector<Mesh::OptMaterial>& materials)
{
	HANDLE fileHandle = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD readSize = 0;
	DWORD fileSize = ::GetFileSize(fileHandle, nullptr);
	uint8_t* pBuffer = new uint8_t[fileSize];

	if (ReadFile(fileHandle, pBuffer, fileSize, &readSize, nullptr) == FALSE)
	{
		delete[] pBuffer;
		CloseHandle(fileHandle);
		return false;
	}

	CloseHandle(fileHandle);

	if (ImportMaterial(fileSize, pBuffer, materials) == false)
	{
		delete[] pBuffer;
		return false;
	}

	delete[] pBuffer;

	return true;
}

bool Mesh::ImportMaterial(uint64_t bufferSize, void* pBuffer, std::vector<Mesh::OptMaterial>& materials)
{
#ifdef V3D64
	ObjParser parser(bufferSize, pBuffer);
#else //V3D64
	ObjParser parser(TO_UI32(bufferSize), pBuffer);
#endif //V3D64

	Mesh::OptMaterial* pMaterial = nullptr;

	while (parser.Read() == true)
	{
		const char* pStr = parser.Pop();

		if ((pStr == nullptr) || (pStr[0] == '#'))
		{
		}
		else if (strcmp(pStr, "newmtl") == 0)
		{
			materials.push_back(Mesh::OptMaterial{});
			pMaterial = &materials.back();

			if (ImportName(&parser, pMaterial->name) == false)
			{
				return false;
			}

			pMaterial->ambientColor.Set(0.0f, 0.0f, 0.0f);
			pMaterial->diffuseColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
			pMaterial->specularColor.Set(1.0f, 1.0f, 1.0f);
			pMaterial->emissiveColor.Set(1.0f, 1.0f, 1.0f);
			pMaterial->specularPower = 50.0f;
			pMaterial->shininess = 0.0f;
			pMaterial->hasTexture = false;
		}
		else if (strcmp(pStr, "Ns") == 0)
		{
			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return false;
			}

			pMaterial->specularPower = static_cast<float>(atof(pStr));
		}
		else if (strcmp(pStr, "Ni") == 0)
		{
			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return false;
			}

			pMaterial->shininess = static_cast<float>(atof(pStr));
		}
		else if (strcmp(pStr, "d") == 0)
		{
			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return false;
			}

			pMaterial->diffuseColor.a = static_cast<float>(atof(pStr));
		}
		else if (strcmp(pStr, "Tr") == 0)
		{
			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return false;
			}

			pMaterial->diffuseColor.a = max(0.0f, 1.0f - static_cast<float>(atof(pStr)));
		}
		else if (strcmp(pStr, "Tf") == 0)
		{
			parser.Pop();
			parser.Pop();
			parser.Pop();
		}
		else if (strcmp(pStr, "illum") == 0)
		{
			parser.Pop();
		}
		else if (strcmp(pStr, "Ka") == 0)
		{
			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return false;
			}

			pMaterial->ambientColor.r = static_cast<float>(atof(pStr));

			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return false;
			}

			pMaterial->ambientColor.g = static_cast<float>(atof(pStr));

			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return false;
			}

			pMaterial->ambientColor.b = static_cast<float>(atof(pStr));
		}
		else if (strcmp(pStr, "Kd") == 0)
		{
			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return false;
			}

			pMaterial->diffuseColor.r = static_cast<float>(atof(pStr));

			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return false;
			}

			pMaterial->diffuseColor.g = static_cast<float>(atof(pStr));

			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return false;
			}

			pMaterial->diffuseColor.b = static_cast<float>(atof(pStr));
		}
		else if (strcmp(pStr, "Ks") == 0)
		{
			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return false;
			}

			pMaterial->specularColor.r = static_cast<float>(atof(pStr));

			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return false;
			}

			pMaterial->specularColor.g = static_cast<float>(atof(pStr));

			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return false;
			}

			pMaterial->specularColor.b = static_cast<float>(atof(pStr));
		}
		else if (strcmp(pStr, "Ke") == 0)
		{
			parser.Pop();
			parser.Pop();
			parser.Pop();
		}
		else if (strcmp(pStr, "Clamp") == 0)
		{
			parser.Pop();
		}
		else if (strcmp(pStr, "map_Ka") == 0)
		{
			if (ImportName(&parser, pMaterial->diffuseTexture) == false)
			{
				return false;
			}

			pMaterial->hasTexture = true;
		}
		else if (strcmp(pStr, "map_Kd") == 0)
		{
			if (ImportName(&parser, pMaterial->diffuseTexture) == false)
			{
				return false;
			}

			pMaterial->hasTexture = true;
		}
		else if (strcmp(pStr, "map_Ks") == 0)
		{
			if (ImportName(&parser, pMaterial->specularTexture) == false)
			{
				return false;
			}

			pMaterial->hasTexture = true;
		}
		else if (strcmp(pStr, "map_d") == 0)
		{
			if (ImportName(&parser, pMaterial->maskTexture) == false)
			{
				return false;
			}

			pMaterial->hasTexture = true;
		}
		else if (strcmp(pStr, "map_bump") == 0)
		{
			if (ImportName(&parser, pMaterial->bumpTexture) == false)
			{
				return false;
			}

			pMaterial->hasTexture = true;
		}
		else if (strcmp(pStr, "bump") == 0)
		{
			if (ImportName(&parser, pMaterial->bumpTexture) == false)
			{
				return false;
			}

			pMaterial->hasTexture = true;
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool Mesh::ImportName(ObjParser* pParser, std::wstring& name)
{
	std::stringstream stringStream;

	for (;;)
	{
		const char* pStr = pParser->Pop();
		if (pStr != nullptr)
		{
			stringStream << pStr;
		}
		else
		{
			break;
		}
	}

	if (stringStream.str().size() == 0)
	{
		return false;
	}

	std::string temp = stringStream.str();

	ToWideChar(temp.c_str(), name);

	return true;
}

void Mesh::Optimize(
	uint32_t flags, float smoosingCos,
	std::vector<Mesh::OptPolygon>::iterator& it_polygon_begin, std::vector<Mesh::OptPolygon>::iterator& it_polygon_end,
	std::vector<MeshVertex>& dstVertices, std::vector<uint32_t>& dstIndices, std::vector<MeshSubset>& dstSubsets)
{
	// ----------------------------------------------------------------------------------------------------
	// ポリゴンを展開
	// ----------------------------------------------------------------------------------------------------

	std::vector<MeshVertex> optVertices;
	std::vector<Mesh::OptFace> optFaces;
	std::vector<Mesh::OptSubset> optSubsets;

	{
		size_t polygonCount = std::distance(it_polygon_begin, it_polygon_end);

		optVertices.resize(polygonCount * 3);
		optFaces.resize(polygonCount);

		Mesh::OptPolygon* pPolygon = &(*it_polygon_begin);
		Mesh::OptPolygon* pPolygonEnd = pPolygon + polygonCount;

		Mesh::OptSubset dummySubset{};
		dummySubset.material = ~0U;
		dummySubset.firstIndex = 0;
		dummySubset.indexCount = 0;
		dummySubset.firstFace = 0;
		dummySubset.faceCount = 0;

		MeshVertex* pVertex = optVertices.data();
		Mesh::OptFace* pFace = optFaces.data();
		Mesh::OptSubset* pSubset = &dummySubset;

		while (pPolygon != pPolygonEnd)
		{
			if (pSubset->material != pPolygon->material)
			{
				Mesh::OptSubset preSubset = *pSubset;

				optSubsets.push_back(Mesh::OptSubset{});
				pSubset = &optSubsets.back();

				pSubset->material = pPolygon->material;
				pSubset->firstIndex = preSubset.firstIndex + preSubset.indexCount;
				pSubset->indexCount = 0;
				pSubset->firstFace = preSubset.firstFace + preSubset.faceCount;
				pSubset->faceCount = 0;
			}

			*pVertex++ = pPolygon->vertices[0];
			*pVertex++ = pPolygon->vertices[1];
			*pVertex++ = pPolygon->vertices[2];

			pFace->indices[0] = pSubset->indexCount++;
			pFace->indices[1] = pSubset->indexCount++;
			pFace->indices[2] = pSubset->indexCount++;
			pFace++;

			pSubset->faceCount++;

			pPolygon++;
		}
	}

	{
		Mesh::OptSubset* pSubset = optSubsets.data();
		Mesh::OptSubset* pSubsetEnd = pSubset + optSubsets.size();

		while (pSubset != pSubsetEnd)
		{
			pSubset->vertices.reserve(pSubset->indexCount);
			pSubset->indices.reserve(pSubset->indexCount);

			for (uint32_t i = 0; i < pSubset->indexCount; i++)
			{
				pSubset->vertices.push_back(optVertices[pSubset->firstIndex + i]);
				pSubset->indices.push_back(i);
			}

			pSubset++;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// 可能ならば頂点を結合し、新しいインデックスを割り振る
	// ----------------------------------------------------------------------------------------------------

	{
		std::vector<Mesh::OptSubset>::iterator it_subset_begin = optSubsets.begin();
		std::vector<Mesh::OptSubset>::iterator it_subset_end = optSubsets.end();
		std::vector<Mesh::OptSubset>::iterator it_subset;

		std::vector<MeshVertex*> combineVertices;
		uint32_t subseMax = TO_UI32(optSubsets.size());
		uint32_t subseCount = 1;

		for (it_subset = it_subset_begin; it_subset != it_subset_end; ++it_subset)
		{
			Mesh::OptSubset* pSubset = &(*it_subset);
			size_t vertexCount = pSubset->vertices.size();

			Mesh::OptFace* pFace = optFaces.data() + pSubset->firstFace;
			Mesh::OptFace* pFaceEnd = pFace + pSubset->faceCount;

			std::wstringstream stringStream;
			stringStream << L"optimize : subset[" << subseCount << L"/" << subseMax << L"] faceCount[" << pSubset->faceCount << L"]";
			OutputDebugString(stringStream.str().c_str());

			uint32_t faceCount = 0;
			uint32_t combineVertexCount = 0;

			while (pFace != pFaceEnd)
			{
				uint32_t* pIndex = &pFace->indices[0];
				uint32_t* pIndexEnd = pIndex + 3;

				if (faceCount == 0)
				{
					OutputDebugString(L".");
				}

				faceCount++;
				if (faceCount == 1000)
				{
					faceCount = 0;
				}

				while (pIndex != pIndexEnd)
				{
					if (pSubset->indices[*pIndex] != *pIndex)
					{
						// 新しいインデックスが割り振られているので調べない
						pIndex++;
						continue;
					}

					uint32_t newIndex = static_cast<uint32_t>(pSubset->vertices.size());
					MeshVertex* pSrcVertex = &(pSubset->vertices[*pIndex]);

					combineVertices.clear();
					combineVertices.push_back(pSrcVertex);

					// 結合相手の頂点を探す
					for (size_t i = 0; i < vertexCount; i++)
					{
						if (i == *pIndex)
						{
							// 自分だった
							continue;
						}

						MeshVertex* pDstVertex = &(pSubset->vertices[i]);

						if ((pSrcVertex->pos == pDstVertex->pos) &&
							(pSrcVertex->uv == pDstVertex->uv))
						{
							if (flags & MESH_IMPORT_SMOOSING)
							{
								float normalCos = Vector3::Dot(pSrcVertex->normal, pDstVertex->normal);

								if (smoosingCos <= normalCos)
								{
									// 結合相手に新規インデックスを割り振り、結合する頂点を追加
									pSubset->indices[i] = newIndex;
									combineVertices.push_back(pDstVertex);
								}
							}
							else
							{
								if (pSrcVertex->normal == pDstVertex->normal)
								{
									//結合相手に新規インデックスを割り振り、結合する頂点を追加
									pSubset->indices[i] = newIndex;
									combineVertices.push_back(pDstVertex);
								}
							}
						}
					}

					if (combineVertices.size() > 1)
					{
						// 自分と他のだれかの頂点の結合が要求されている
						MeshVertex** ppCombineVertex = combineVertices.data();

						if (flags & MESH_IMPORT_SMOOSING)
						{
							size_t combineVertexCount = combineVertices.size();

							MeshVertex** ppVertex = ppCombineVertex;
							MeshVertex** ppVertexEnd = ppVertex + combineVertexCount;

							Vector3 combineNormal;
							Vector3 combineTangent;
							Vector3 combineBinormal;

							while (ppVertex != ppVertexEnd)
							{
								combineNormal += (*ppVertex)->normal;
								combineTangent += (*ppVertex)->tangent;
								combineBinormal += (*ppVertex)->binormal;
								ppVertex++;
							}

							(*ppCombineVertex)->normal = combineNormal.ToNormalize();
							(*ppCombineVertex)->tangent = combineTangent.ToNormalize();
							(*ppCombineVertex)->binormal = combineBinormal.ToNormalize();
						}

						// 自分の新規インデックスを割り振る
						pSubset->indices[*pIndex] = newIndex;

						// 結合した頂点を追加
						pSubset->vertices.push_back(**ppCombineVertex);

						combineVertexCount += TO_UI32(combineVertices.size() - 1);
					}

					pIndex++;
				}

				pFace++;
			}

			uint32_t afterVertexCount = TO_UI32(vertexCount) - combineVertexCount;

			stringStream.str(L"");
			stringStream << L"\n           vertexCount[" << afterVertexCount << L"/" << vertexCount << L"]\n";
			OutputDebugString(stringStream.str().c_str());
			subseCount++;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// バーテックス、インデックスのリストを作成
	// ----------------------------------------------------------------------------------------------------

	{
		uint32_t newIndex = 0;

		std::vector<Mesh::OptIndex> indexOpts;

		std::vector<Mesh::OptSubset>::iterator it_subset_begin = optSubsets.begin();
		std::vector<Mesh::OptSubset>::iterator it_subset_end = optSubsets.end();
		std::vector<Mesh::OptSubset>::iterator it_subset;

		uint32_t indexOffset = TO_UI32(dstVertices.size());

		for (it_subset = it_subset_begin; it_subset != it_subset_end; ++it_subset)
		{
			Mesh::OptSubset* pSubset = &(*it_subset);

			indexOpts.clear();
			indexOpts.resize(pSubset->vertices.size());

			Mesh::OptFace* pFace = optFaces.data() + pSubset->firstFace;
			Mesh::OptFace* pFaceEnd = pFace + pSubset->faceCount;

			uint32_t firstIndex = static_cast<uint32_t>(dstIndices.size());

			while (pFace != pFaceEnd)
			{
				uint32_t* pIndex = &pFace->indices[0];
				uint32_t* pIndexEnd = pIndex + 3;

				while (pIndex != pIndexEnd)
				{
					uint32_t oldIndex = pSubset->indices[*pIndex];
					Mesh::OptIndex* pOptIndex = &indexOpts[oldIndex];

					if (pOptIndex->assigned == false)
					{
						const MeshVertex& srcVertex = pSubset->vertices[oldIndex];
						MeshVertex dstVertex;

						dstVertex.pos = srcVertex.pos;
						dstVertex.uv = srcVertex.uv;
						dstVertex.normal = srcVertex.normal;
						dstVertex.tangent = srcVertex.tangent;
						dstVertex.binormal = srcVertex.binormal;

						pOptIndex->index = newIndex;
						pOptIndex->assigned = true;

						dstVertices.push_back(dstVertex);
						dstIndices.push_back(indexOffset + newIndex);

						newIndex++;
					}
					else
					{
						dstIndices.push_back(indexOffset + pOptIndex->index);
					}

					pIndex++;
				}

				pFace++;
			}

			MeshSubset dstSubset;
			dstSubset.material = pSubset->material;
			dstSubset.indexCount = TO_UI32(dstIndices.size()) - firstIndex;
			dstSubset.firstIndex = firstIndex;
			dstSubsets.push_back(dstSubset);
		}
	}

#ifdef _DEBUG
	wchar_t mes[2014];
	::wsprintf(mes, L"vertex[%u] primitive[%u] subset[%u]\n", dstVertices.size(), dstIndices.size() / 3, dstSubsets.size());
	::OutputDebugString(mes);
#endif //_DEBUG

	// ----------------------------------------------------------------------------------------------------
}
