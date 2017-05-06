#include "Mesh.h"
#include "ObjParser.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	Dispose();
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
	m_pMeshManager = nullptr;
	m_pMaterialManager = nullptr;
	m_pTextureManager = nullptr;
	m_pGraphicsManager = nullptr;
}

V3D_RESULT Mesh::Load(
	const wchar_t* pFilePath,
	const MeshLoadDesc& loadDesc,
	std::vector<MeshVertex>& vertices,
	std::vector<Mesh::OptNode>& nodes,
	std::vector<Mesh::OptSubset>& subsets,
	std::vector<Mesh::OptMaterial>& materials)
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
	uint8_t* pBuffer = new uint8_t[bufferSize];

	if (ReadFile(fileHandle, pBuffer, bufferSize, &readSize, nullptr) == FALSE)
	{
		delete[] pBuffer;
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

	{
#ifdef _WIN64
		ObjParser parser(bufferSize, pBuffer);
#else //_WIN64
		ObjParser parser(TO_UI32(bufferSize), pBuffer);
#endif //_WIN64

		std::vector<MeshVertex> tempVertices;
		tempVertices.reserve(4);

		std::vector<Vector3> positions;
		std::vector<Vector2> textures;
		std::vector<Vector3> normals;

		uint32_t objectNodeIndex = ~0U;
		uint32_t currentNodeIndex = ~0U;

		//		subsets.push_back(Mesh::OptSubset{});
		//		Mesh::OptSubset* pSubset = &subsets.back();
		Mesh::OptSubset* pSubset = nullptr;

		while (parser.Read() == true)
		{
			const char* pStr = parser.Pop();

			if ((pStr == nullptr) || (strcmp(pStr, "#") == 0))
			{
			}
			else if (strcmp(pStr, "mtllib") == 0)
			{
				// マテリアルライブラリ
				std::wstring materialLib;

				pStr = parser.Pop();
				if (pStr == nullptr)
				{
					return V3D_ERROR_FAIL;
				}

				ToWideChar(pStr, materialLib);

				std::wstringstream mlStream;
				mlStream << dirPath << materialLib.c_str();

				V3D_RESULT v3dResult = LoadMaterial(dirPath.c_str(), mlStream.str().c_str(), materials);
				if (v3dResult != V3D_OK)
				{
					return v3dResult;
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
				pStr = parser.Pop();
				if (pStr == nullptr)
				{
					return V3D_ERROR_FAIL;
				}

				Mesh::OptNode node{};
				ToWideChar(pStr, node.name);
				node.parentNode = objectNodeIndex;
				node.firstSubset = TO_UI32(subsets.size());
				node.subsetCount = 0;

				objectNodeIndex = currentNodeIndex = TO_UI32(nodes.size());
				nodes.push_back(node);

				std::stringstream stringStream;
				stringStream << "object : " << pStr << "\n";
				OutputDebugStringA(stringStream.str().c_str());
			}
			else if (strcmp(pStr, "g") == 0)
			{
				// グループ
				pStr = parser.Pop();
				if (pStr == nullptr)
				{
					return V3D_ERROR_FAIL;
				}

				if (objectNodeIndex == ~0U)
				{
					Mesh::OptNode node{};
					node.name = L"root";
					node.parentNode = ~0U;
					node.firstSubset = TO_UI32(subsets.size());
					node.subsetCount = 0;

					objectNodeIndex = TO_UI32(nodes.size());
					nodes.push_back(node);

					OutputDebugString(L"object : root\n");
				}

				Mesh::OptNode node{};
				ToWideChar(pStr, node.name);
				node.parentNode = objectNodeIndex;
				node.firstSubset = TO_UI32(subsets.size());
				node.subsetCount = 0;

				currentNodeIndex = TO_UI32(nodes.size());
				nodes.push_back(node);

				std::stringstream stringStream;
				stringStream << "  group : " << pStr << "\n";
				OutputDebugStringA(stringStream.str().c_str());
			}
			else if (strcmp(pStr, "usemtl") == 0)
			{
				// マテリアルを設定
				if (currentNodeIndex == ~0U)
				{
					return V3D_ERROR_FAIL;
				}

				pStr = parser.Pop();
				if (pStr == nullptr)
				{
					return V3D_ERROR_FAIL;
				}

				std::wstring materialName;
				ToWideChar(pStr, materialName);

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
					return V3D_ERROR_FAIL;
				}

				Mesh::OptNode* pNode = &nodes[currentNodeIndex];

				if ((pNode->subsetCount == 0) || (pSubset->material != materialIndex))
				{
					uint32_t firstVertex;

					if (pSubset == nullptr)
					{
						firstVertex = 0;
					}
					else
					{
						firstVertex = pSubset->firstIndex + pSubset->indexCount;
					}

					subsets.push_back(Mesh::OptSubset{});
					pSubset = &subsets.back();

					pSubset->material = materialIndex;
					pSubset->firstIndex = firstVertex;

					pNode->subsetCount++;
				}
				/*
				if ((pSubset->indexCount > 0) && (pSubset->material != materialIndex))
				{
				uint32_t firstVertex = pSubset->firstIndex + pSubset->indexCount;

				subsets.push_back(Mesh::OptSubset{});
				pSubset = &subsets.back();

				pSubset->material = materialIndex;
				pSubset->firstIndex = firstVertex;
				}
				*/
				std::wstringstream stringStream;
				stringStream << L"    usemtl : " << materialName << L"\n";
				OutputDebugString(stringStream.str().c_str());
			}
			else if (strcmp(pStr, "s") == 0)
			{
				// スムージング ( 1 or off )
				parser.Pop();
			}
			else if (strcmp(pStr, "f") == 0)
			{
				// フェイス
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

					MeshVertex vertex;

					if (indices[0] > 0)
					{
						vertex.pos = positions[static_cast<uint32_t>(indices[0] - 1)];
					}
					else if (indices[0] < 0)
					{
						vertex.pos = positions[static_cast<uint32_t>(static_cast<long long>(positions.size()) + indices[0])];
					}

					if (indices[1] > 0)
					{
						vertex.uv = textures[static_cast<uint32_t>(indices[1] - 1)];
					}
					else if (indices[1] < 0)
					{
						vertex.uv = textures[static_cast<uint32_t>(static_cast<long long>(textures.size()) + indices[1])];
					}

					if (indices[2] > 0)
					{
						vertex.normal = normals[static_cast<uint32_t>(indices[2] - 1)];
					}
					else if (indices[2] < 0)
					{
						vertex.normal = normals[static_cast<uint32_t>(static_cast<long long>(normals.size()) + indices[2])];
					}

					if ((indices[0] != 0) || (indices[1] != 0) || (indices[2] != 0))
					{
						tempVertices.push_back(vertex);
					}
				}

				uint32_t vertexCount = static_cast<uint32_t>(tempVertices.size());

				if (vertexCount == 3)
				{
					vertices.push_back(tempVertices[0]);
					vertices.push_back(tempVertices[1]);
					vertices.push_back(tempVertices[2]);

					pSubset->indexCount += 3;
				}
				else if (vertexCount == 4)
				{
					vertices.push_back(tempVertices[0]);
					vertices.push_back(tempVertices[1]);
					vertices.push_back(tempVertices[2]);
					vertices.push_back(tempVertices[2]);
					vertices.push_back(tempVertices[3]);
					vertices.push_back(tempVertices[0]);

					pSubset->indexCount += 6;
				}
				else if (vertexCount > 4)
				{
					uint32_t pointCount = vertexCount - 3;
					uint32_t lastPoint = vertexCount - 1;

					for (uint32_t i = 0; i < pointCount; i++)
					{
						vertices.push_back(tempVertices[i + 0]);
						vertices.push_back(tempVertices[i + 1]);
						vertices.push_back(tempVertices[lastPoint]);
					}

					pSubset->indexCount += pointCount * 3;
				}
				else if (vertexCount == 0)
				{
				}
				else
				{
					return V3D_ERROR_FAIL;
				}
			}
			else
			{
				return V3D_ERROR_FAIL;
			}
		}
	}

	delete[] pBuffer;

	// ----------------------------------------------------------------------------------------------------
	// 回転
	// ----------------------------------------------------------------------------------------------------

	{
		std::vector<MeshVertex>::iterator it_begin = vertices.begin();
		std::vector<MeshVertex>::iterator it_end = vertices.end();
		std::vector<MeshVertex>::iterator it;

		for (it = it_begin; it != it_end; ++it)
		{
			it->pos = loadDesc.matrix * it->pos;

			if (loadDesc.flags & MESH_LOAD_INVERT_TEX_U) { it->uv.x = 1.0f - it->uv.x; }
			if (loadDesc.flags & MESH_LOAD_INVERT_TEX_V) { it->uv.y = 1.0f - it->uv.y; }
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// 法線、接線空間を求める
	// ----------------------------------------------------------------------------------------------------

	{
		size_t primitiveCount = vertices.size() / 3;

		size_t io0;
		size_t io1;
		size_t io2;

		if (loadDesc.flags & MESH_LOAD_FLIP_FACE)
		{
			io0 = 2;
			io1 = 1;
			io2 = 0;
		}
		else
		{
			io0 = 0;
			io1 = 1;
			io2 = 2;
		}

		for (size_t i = 0; i < primitiveCount; i++)
		{
			size_t i0 = i * 3 + io0;
			size_t i1 = i * 3 + io1;
			size_t i2 = i * 3 + io2;

			// 法線
			Vector3 ab;
			Vector3 bc;

			if (loadDesc.flags & MESH_LOAD_INVERT_NORMAL)
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
		}
	}

	return V3D_OK;
}

V3D_RESULT Mesh::LoadMaterial(const wchar_t* pDirPath, const wchar_t* pFilePath, std::vector<Mesh::OptMaterial>& materials)
{
	HANDLE fileHandle = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		return V3D_ERROR_FAIL;
	}

	DWORD readSize = 0;
	DWORD fileSize = ::GetFileSize(fileHandle, nullptr);
	uint8_t* pBuffer = new uint8_t[fileSize];

	if (ReadFile(fileHandle, pBuffer, fileSize, &readSize, nullptr) == FALSE)
	{
		delete[] pBuffer;
		CloseHandle(fileHandle);
		return V3D_ERROR_FAIL;
	}

	CloseHandle(fileHandle);

	V3D_RESULT result = LoadMaterial(pDirPath, fileSize, pBuffer, materials);
	if (result != V3D_OK)
	{
		delete[] pBuffer;
		return V3D_ERROR_FAIL;
	}

	delete[] pBuffer;

	return V3D_OK;
}

V3D_RESULT Mesh::LoadMaterial(const wchar_t* pDirPath, uint64_t bufferSize, void* pBuffer, std::vector<Mesh::OptMaterial>& materials)
{
#ifdef _WIN64
	ObjParser parser(bufferSize, pBuffer);
#else //_WIN64
	ObjParser parser(TO_UI32(bufferSize), pBuffer);
#endif //_WIN64

	Mesh::OptMaterial* pMaterial = nullptr;

	while (parser.Read() == true)
	{
		const char* pStr = parser.Pop();

		if ((pStr == nullptr) || (strcmp(pStr, "#") == 0))
		{
		}
		else if (strcmp(pStr, "newmtl") == 0)
		{
			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return V3D_ERROR_FAIL;
			}

			materials.push_back(Mesh::OptMaterial{});
			pMaterial = &materials.back();

			ToWideChar(pStr, pMaterial->name);
			pMaterial->ambientColor.Set(0.0f, 0.0f, 0.0f);
			pMaterial->diffuseColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
			pMaterial->specularColor.Set(1.0f, 1.0f, 1.0f);
			pMaterial->emissiveColor.Set(1.0f, 1.0f, 1.0f);
			pMaterial->specularPower = 1.0f;
			pMaterial->shininess = 0.0f;
		}
		else if (strcmp(pStr, "Ns") == 0)
		{
			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return V3D_ERROR_FAIL;
			}

			pMaterial->specularPower = static_cast<float>(atof(pStr));
		}
		else if (strcmp(pStr, "Ni") == 0)
		{
			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return V3D_ERROR_FAIL;
			}

			pMaterial->shininess = static_cast<float>(atof(pStr));
		}
		else if (strcmp(pStr, "d") == 0)
		{
			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return V3D_ERROR_FAIL;
			}

			pMaterial->diffuseColor.a = static_cast<float>(atof(pStr));
		}
		else if (strcmp(pStr, "Tr") == 0)
		{
			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return V3D_ERROR_FAIL;
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
				return V3D_ERROR_FAIL;
			}

			pMaterial->ambientColor.r = static_cast<float>(atof(pStr));

			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return V3D_ERROR_FAIL;
			}

			pMaterial->ambientColor.g = static_cast<float>(atof(pStr));

			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return V3D_ERROR_FAIL;
			}

			pMaterial->ambientColor.b = static_cast<float>(atof(pStr));
		}
		else if (strcmp(pStr, "Kd") == 0)
		{
			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return V3D_ERROR_FAIL;
			}

			pMaterial->diffuseColor.r = static_cast<float>(atof(pStr));

			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return V3D_ERROR_FAIL;
			}

			pMaterial->diffuseColor.g = static_cast<float>(atof(pStr));

			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return V3D_ERROR_FAIL;
			}

			pMaterial->diffuseColor.b = static_cast<float>(atof(pStr));
		}
		else if (strcmp(pStr, "Ks") == 0)
		{
			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return V3D_ERROR_FAIL;
			}

			pMaterial->specularColor.r = static_cast<float>(atof(pStr));

			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return V3D_ERROR_FAIL;
			}

			pMaterial->specularColor.g = static_cast<float>(atof(pStr));

			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return V3D_ERROR_FAIL;
			}

			pMaterial->specularColor.b = static_cast<float>(atof(pStr));
		}
		else if (strcmp(pStr, "Ke") == 0)
		{
			parser.Pop();
			parser.Pop();
			parser.Pop();
		}
		else if (strcmp(pStr, "map_Ka") == 0)
		{
			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return V3D_ERROR_FAIL;
			}

			ToWideChar(pStr, pMaterial->diffuseTexture);
		}
		else if (strcmp(pStr, "map_Kd") == 0)
		{
			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return V3D_ERROR_FAIL;
			}

			ToWideChar(pStr, pMaterial->diffuseTexture);
		}
		else if (strcmp(pStr, "map_d") == 0)
		{
			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return V3D_ERROR_FAIL;
			}

			ToWideChar(pStr, pMaterial->maskTexture);
		}
		else if (strcmp(pStr, "map_bump") == 0)
		{
			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return V3D_ERROR_FAIL;
			}

			ToWideChar(pStr, pMaterial->bumpTexture);
		}
		else if (strcmp(pStr, "bump") == 0)
		{
			pStr = parser.Pop();
			if (pStr == nullptr)
			{
				return V3D_ERROR_FAIL;
			}

			ToWideChar(pStr, pMaterial->bumpTexture);
		}
		else
		{
			return V3D_ERROR_FAIL;
		}
	}

	return V3D_OK;
}
