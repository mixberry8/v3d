#pragma once

#include "GraphicsManager.h"
#include "ResourceHeap.h"
#include "Texture.h"

class MaterialManager;

class Material
{
public:
	enum BINDING
	{
		BINDING_UNIFORM = 0,
		BINDING_ALBEDO_TEXTURE = 1,
		BINDING_NORMAL_TEXTURE = 2,
	};

	Material();
	~Material();

	uint64_t GetKey() const;

	const wchar_t* GetName() const;

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

	float GetSpecularPower() const;
	void SetSpecularPower(float sepcularPower);

	float GetShininess() const;
	void SetShininess(float shininess);

	TexturePtr GetNormalTexture();
	void SetNormalTexture(TexturePtr texture);

	V3D_RESULT Update();

	void Bind(IV3DCommandBuffer* pCommandBuffer);

private:
	struct Uniform
	{
		Vector4 diffuseColor;
		float ambient;
		float specularPower;
		float shininess;
		float reserve;
	};

	GraphicsManager* m_pGraphicsManager;
	MaterialManager* m_pMaterialManager;

	uint64_t m_Key;

	std::wstring m_Name;
	Uniform m_Uniform;

	V3D_FILTER m_Filter;
	V3D_ADDRESS_MODE m_AddressMode;
	TexturePtr m_DiffuseTexture;
	TexturePtr m_NormalTexture;

	IV3DPipelineLayout* m_pPipelineLayout;

	GraphicsPipelienDesc m_PipelineDesc;
	GraphicsPipelineHandle m_PipelineHandle;

	IV3DBuffer* m_pUniformBuffer;
	IV3DBufferView* m_pUniformBufferView;
	ResourceHeap::Handle m_UniformBufferHandle;
	IV3DDescriptorSet* m_pDescriptorSet;

	V3D_RESULT Initialize(GraphicsManager* pGraphicsManager, MaterialManager* pMaterialManager, const wchar_t* pName);
	void Dispose();

	friend class Mesh;
	friend class MaterialManager;
};

typedef std::shared_ptr<Material> MaterialPtr;
