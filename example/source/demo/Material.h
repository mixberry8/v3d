#pragma once

#include "GraphicsManager.h"
#include "Texture.h"
#include "DynamicOffsetHeap.h"

class TextureManager;
class MaterialManager;

enum RENDER_TYPE
{
	RENDER_TYPE_GEOMETRY = 0,
	RENDER_TYPE_SHADOW = 1,

	RENDER_TYPE_COUNT = 2,
};

class Material
{
public:
	Material();
	~Material();

	const wchar_t* GetName() const;

	uint64_t GetKey() const;

	IV3DPipelineLayout* GetPipelineLayoutPtr(RENDER_TYPE renderType);
	IV3DPipeline* GetPipelinePtr(RENDER_TYPE renderType);
	IV3DDescriptorSet* GetDescriptorSetPtr();
	uint32_t GetDynamicOffset() const;

	V3D_FILTER GetFilter() const;
	void SetFilter(V3D_FILTER filter);

	const V3D_ADDRESS_MODE GetAddressMode() const;
	void SetAddressMode(V3D_ADDRESS_MODE addressMode);

	const BLEND_MODE GetBlendMode() const;
	void SetBlendMode(BLEND_MODE blendMode);

	V3D_POLYGON_MODE GetPolygonMode() const;
	void SetPolygonMode(V3D_POLYGON_MODE polygonMode);

	V3D_CULL_MODE GetCullMode() const;
	void SetCullMode(V3D_CULL_MODE cullMode);

	float GetAmbient() const;
	void SetAmbient(float ambient);

	const Vector4& GetDiffuseColor() const;
	void SetDiffuseColor(const Vector4& color);

	TexturePtr GetDiffuseTexture();
	void SetDiffuseTexture(TexturePtr texture);

	TexturePtr GetMaskTexture();
	void SetMaskTexture(TexturePtr texture);

	TexturePtr GetSpecularTexture();
	void SetSpecularTexture(TexturePtr texture);

	float GetSpecularPower() const;
	void SetSpecularPower(float sepcularPower);

	float GetShininess() const;
	void SetShininess(float shininess);

	TexturePtr GetNormalTexture();
	void SetNormalTexture(TexturePtr texture);

	V3D_RESULT Update();

	void Bind(IV3DCommandBuffer* pCommandBuffer, RENDER_TYPE renderType);

private:
	static constexpr GraphicsPipelienDesc ShadowPipelineDesc =
	{
		V3D_POLYGON_MODE_FILL,
		V3D_CULL_MODE_FRONT,
		true,
		true,
		BLEND_MODE_COPY,
	};

	enum BINDING
	{
		BINDING_UNIFORM = 0,
		BINDING_DIFFUSE_TEXTURE = 1,
		BINDING_SPECULAR_TEXTURE = 2,
		BINDING_MASK_TEXTURE = 3,
		BINDING_NORMAL_TEXTURE = 4,
	};

	struct Uniform
	{
		Vector4 diffuseColor;
		float ambient;
		float specularPower;
		float shininess;
		float reserve;
	};

	struct SerialData
	{
		wchar_t name[256];

		float diffuseColor[4];
		float ambient;
		float specularPower;
		float shininess;
		float reserve1[1];

		uint32_t filter;
		uint32_t addressMode;
		uint32_t reserve2[2];

		wchar_t diffuseTextureFilePath[256];
		wchar_t specularTextureFilePath[256];
		wchar_t maskTextureFilePath[256];
		wchar_t normalTextureFilePath[256];
	};

	GraphicsManager* m_pGraphicsManager;
	MaterialManager* m_pMaterialManager;

	uint64_t m_Key;

	std::wstring m_Name;
	Uniform m_Uniform;

	V3D_FILTER m_Filter;
	V3D_ADDRESS_MODE m_AddressMode;
	TexturePtr m_DiffuseTexture;
	TexturePtr m_SpecularTexture;
	TexturePtr m_MaskTexture;
	TexturePtr m_NormalTexture;

	GraphicsPipelienDesc m_GeometryPipelineDesc;

	Array1<IV3DPipelineLayout*, RENDER_TYPE_COUNT> m_PipelineLayouts;
	Array1<GraphicsPipelineHandle, RENDER_TYPE_COUNT> m_PipelineHandles;

	IV3DBuffer* m_pUniformBuffer;
	DynamicOffsetHeap::Handle m_UniformDynamicOffsetHandle;
	IV3DDescriptorSet* m_pDescriptorSet;

	V3D_RESULT Initialize(GraphicsManager* pGraphicsManager, MaterialManager* pMaterialManager, const wchar_t* pName);
	V3D_RESULT Initialize(GraphicsManager* pGraphicsManager, TextureManager* pTextureManager, MaterialManager* pMaterialManager, const Material::SerialData* pData);
	void Dispose();

	bool Serialize(Material::SerialData* pData);

	friend class Mesh;
	friend class StaticMesh;
	friend class SkeletalMesh;
	friend class MaterialManager;
};

typedef std::shared_ptr<Material> MaterialPtr;
