#include "Material.h"
#include "TextureManager.h"
#include "MaterialManager.h"

Material::Material() :
	m_pGraphicsManager(nullptr),
	m_pMaterialManager(nullptr),
	m_Key(0),
	m_Uniform({}),
	m_GeometryPipelineDesc({}),
	m_pUniformBuffer(nullptr),
	m_pDescriptorSet(nullptr),
	m_Filter(V3D_FILTER_LINEAR),
	m_AddressMode(V3D_ADDRESS_MODE_REPEAT)
{
	m_GeometryPipelineDesc.polygonMode = V3D_POLYGON_MODE_FILL;
	m_GeometryPipelineDesc.cullMode = V3D_CULL_MODE_BACK;
	m_GeometryPipelineDesc.depthTestEnable = true;
	m_GeometryPipelineDesc.depthWriteEnable = true;
	m_GeometryPipelineDesc.blendMode = BLEND_MODE_COPY;

	m_Uniform.diffuseColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_Uniform.ambient = 0.0f;
	m_Uniform.specularPower = 60.0f;
	m_Uniform.shininess = 1.0f;
}

Material::~Material()
{
	Dispose();
}

const wchar_t* Material::GetName() const
{
	return m_Name.c_str();
}

uint64_t Material::GetKey() const
{
	return m_Key;
}

IV3DPipelineLayout* Material::GetPipelineLayoutPtr(RENDER_TYPE renderType)
{
	return m_PipelineLayouts[renderType];
}

IV3DPipeline* Material::GetPipelinePtr(RENDER_TYPE renderType)
{
	return m_PipelineHandles[renderType]->GetPtr();
}

IV3DDescriptorSet* Material::GetDescriptorSetPtr()
{
	return m_pDescriptorSet;
}

uint32_t Material::GetDynamicOffset() const
{
	return m_UniformDynamicOffsetHandle.GetOffset();
}

V3D_FILTER Material::GetFilter() const
{
	return m_Filter;
}

void Material::SetFilter(V3D_FILTER filter)
{
	m_Filter = filter;
}

const V3D_ADDRESS_MODE Material::GetAddressMode() const
{
	return m_AddressMode;
}

void Material::SetAddressMode(V3D_ADDRESS_MODE addressMode)
{
	m_AddressMode = addressMode;
}

const BLEND_MODE Material::GetBlendMode() const
{
	return m_GeometryPipelineDesc.blendMode;
}

void Material::SetBlendMode(BLEND_MODE blendMode)
{
	m_GeometryPipelineDesc.blendMode = blendMode;
}

V3D_POLYGON_MODE Material::GetPolygonMode() const
{
	return m_GeometryPipelineDesc.polygonMode;
}

void Material::SetPolygonMode(V3D_POLYGON_MODE polygonMode)
{
	m_GeometryPipelineDesc.polygonMode = polygonMode;
}

V3D_CULL_MODE Material::GetCullMode() const
{
	return m_GeometryPipelineDesc.cullMode;
}

void Material::SetCullMode(V3D_CULL_MODE cullMode)
{
	m_GeometryPipelineDesc.cullMode = cullMode;
}

float Material::GetAmbient() const
{
	return m_Uniform.ambient;
}

void Material::SetAmbient(float ambient)
{
	m_Uniform.ambient = CLAMP(ambient, 0.0f, 1.0f);
}

const Vector4& Material::GetDiffuseColor() const
{
	return m_Uniform.diffuseColor;
}

void Material::SetDiffuseColor(const Vector4& albedo)
{
	m_Uniform.diffuseColor = albedo;
}

TexturePtr Material::GetDiffuseTexture()
{
	return m_DiffuseTexture;
}

void Material::SetDiffuseTexture(TexturePtr texture)
{
	if (texture == nullptr)
	{
		return;
	}

	m_DiffuseTexture = texture;

	if (m_DiffuseTexture != nullptr)
	{
		m_pDescriptorSet->SetImageViewAndSampler(
			Material::BINDING_DIFFUSE_TEXTURE,
			m_DiffuseTexture->GetImageViewPtr(),
			V3D_IMAGE_LAYOUT_SHADER_READ_ONLY,
			m_pMaterialManager->GetSamplerPtr(m_Filter, m_AddressMode));
	}
	else
	{
		m_pDescriptorSet->SetImageViewAndSampler(
			Material::BINDING_DIFFUSE_TEXTURE,
			m_pMaterialManager->GetDummyImageViewPtr(MaterialManager::DUMMY_IMAGE_TYPE_DIFFUSE),
			V3D_IMAGE_LAYOUT_SHADER_READ_ONLY,
			m_pMaterialManager->GetSamplerPtr(m_Filter, m_AddressMode));
	}
}

TexturePtr Material::GetMaskTexture()
{
	return m_DiffuseTexture;
}

void Material::SetMaskTexture(TexturePtr texture)
{
	if (texture == nullptr)
	{
		return;
	}

	m_MaskTexture = texture;

	if (m_MaskTexture != nullptr)
	{
		m_pDescriptorSet->SetImageViewAndSampler(
			Material::BINDING_MASK_TEXTURE,
			m_MaskTexture->GetImageViewPtr(),
			V3D_IMAGE_LAYOUT_SHADER_READ_ONLY,
			m_pMaterialManager->GetSamplerPtr(m_Filter, m_AddressMode));
	}
	else
	{
		m_pDescriptorSet->SetImageViewAndSampler(
			Material::BINDING_MASK_TEXTURE,
			m_pMaterialManager->GetDummyImageViewPtr(MaterialManager::DUMMY_IMAGE_TYPE_MASK),
			V3D_IMAGE_LAYOUT_SHADER_READ_ONLY,
			m_pMaterialManager->GetSamplerPtr(m_Filter, m_AddressMode));
	}
}

TexturePtr Material::GetSpecularTexture()
{
	return m_SpecularTexture;
}

void Material::SetSpecularTexture(TexturePtr texture)
{
	if (texture == nullptr)
	{
		return;
	}

	m_SpecularTexture = texture;

	if (m_SpecularTexture != nullptr)
	{
		m_pDescriptorSet->SetImageViewAndSampler(
			Material::BINDING_SPECULAR_TEXTURE,
			m_SpecularTexture->GetImageViewPtr(),
			V3D_IMAGE_LAYOUT_SHADER_READ_ONLY,
			m_pMaterialManager->GetSamplerPtr(m_Filter, m_AddressMode));
	}
	else
	{
		m_pDescriptorSet->SetImageViewAndSampler(
			Material::BINDING_SPECULAR_TEXTURE,
			m_pMaterialManager->GetDummyImageViewPtr(MaterialManager::DUMMY_IMAGE_TYPE_SPECULAR),
			V3D_IMAGE_LAYOUT_SHADER_READ_ONLY,
			m_pMaterialManager->GetSamplerPtr(m_Filter, m_AddressMode));
	}
}

float Material::GetSpecularPower() const
{
	return m_Uniform.specularPower;
}

void Material::SetSpecularPower(float sepcularPower)
{
	m_Uniform.specularPower = MAXIMUM(1.0f, sepcularPower);
}

float Material::GetShininess() const
{
	return m_Uniform.shininess;
}

void Material::SetShininess(float shininess)
{
	m_Uniform.shininess = MAXIMUM(0.0f, shininess);
}

TexturePtr Material::GetNormalTexture()
{
	return m_NormalTexture;
}

void Material::SetNormalTexture(TexturePtr texture)
{
	if (texture == nullptr)
	{
		return;
	}

	m_NormalTexture = texture;

	if (m_NormalTexture != nullptr)
	{
		m_pDescriptorSet->SetImageViewAndSampler(
			Material::BINDING_NORMAL_TEXTURE,
			m_NormalTexture->GetImageViewPtr(),
			V3D_IMAGE_LAYOUT_SHADER_READ_ONLY,
			m_pMaterialManager->GetSamplerPtr(V3D_FILTER_NEAREST, m_AddressMode));
	}
	else
	{
		m_pDescriptorSet->SetImageViewAndSampler(
			Material::BINDING_NORMAL_TEXTURE,
			m_pMaterialManager->GetDummyImageViewPtr(MaterialManager::DUMMY_IMAGE_TYPE_NORMAL),
			V3D_IMAGE_LAYOUT_SHADER_READ_ONLY,
			m_pMaterialManager->GetSamplerPtr(V3D_FILTER_NEAREST, m_AddressMode));
	}
}

V3D_RESULT Material::Update()
{
	V3D_RESULT result = m_pGraphicsManager->GetPipelineHandle(
		GRAPHICS_RENDERPASS_TYPE_GEOMETRY, 0,
		GRAPHICS_PIPELINE_TYPE_GEOMETRY, m_GeometryPipelineDesc,
		&m_PipelineHandles[RENDER_TYPE_GEOMETRY]);
	if (result != V3D_OK)
	{
		return result;
	}

	result = m_pGraphicsManager->GetPipelineHandle(
		GRAPHICS_RENDERPASS_TYPE_SHADOW, 0,
		GRAPHICS_PIPELINE_TYPE_SHADOW, Material::ShadowPipelineDesc,
		&m_PipelineHandles[RENDER_TYPE_SHADOW]);

	if (result != V3D_OK)
	{
		return result;
	}

	void* pMemory;

	result = m_pUniformBuffer->Map(m_UniformDynamicOffsetHandle.GetOffset(), sizeof(Material::Uniform), &pMemory);
	if (result != V3D_OK)
	{
		return result;
	}

	MemCopy(pMemory, sizeof(Material::Uniform), &m_Uniform, sizeof(Material::Uniform));

	result = m_pUniformBuffer->Unmap();
	if (result != V3D_OK)
	{
		return result;
	}

	m_pDescriptorSet->Update();

	return V3D_OK;
}

void Material::Bind(IV3DCommandBuffer* pCommandBuffer, RENDER_TYPE renderType)
{
	uint32_t dynamicOffset = m_UniformDynamicOffsetHandle.GetOffset();

	pCommandBuffer->BindPipeline(m_PipelineHandles[renderType]->GetPtr());
	pCommandBuffer->BindDescriptorSets(V3D_PIPELINE_TYPE_GRAPHICS, m_PipelineLayouts[renderType], 1, 1, &m_pDescriptorSet, 1, &dynamicOffset);
}

V3D_RESULT Material::Initialize(GraphicsManager* pGraphicsManager, MaterialManager* pMaterialManager, const wchar_t* pName)
{
	m_pGraphicsManager = pGraphicsManager;
	m_pMaterialManager = pMaterialManager;

	m_Key = reinterpret_cast<uint64_t>(this);
	m_Name = pName;

	// ----------------------------------------------------------------------------------------------------
	// グラフィックスパイプラインを取得
	// ----------------------------------------------------------------------------------------------------

	m_pGraphicsManager->GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_GEOMETRY, &m_PipelineLayouts[RENDER_TYPE_GEOMETRY]);

	V3D_RESULT result = m_pGraphicsManager->GetPipelineHandle(
		GRAPHICS_RENDERPASS_TYPE_GEOMETRY, 0,
		GRAPHICS_PIPELINE_TYPE_GEOMETRY, m_GeometryPipelineDesc,
		&m_PipelineHandles[RENDER_TYPE_GEOMETRY]);
	if (result != V3D_OK)
	{
		return result;
	}

	m_pGraphicsManager->GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_SHADOW, &m_PipelineLayouts[RENDER_TYPE_SHADOW]);

	result = m_pGraphicsManager->GetPipelineHandle(
		GRAPHICS_RENDERPASS_TYPE_SHADOW, 0,
		GRAPHICS_PIPELINE_TYPE_SHADOW, Material::ShadowPipelineDesc,
		&m_PipelineHandles[RENDER_TYPE_SHADOW]);

	if (result != V3D_OK)
	{
		return result;
	}

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットを作成
	// ----------------------------------------------------------------------------------------------------

	result = m_pMaterialManager->CreateDescriptorSet(m_Filter, m_AddressMode, &m_pDescriptorSet, &m_UniformDynamicOffsetHandle);
	if (result != V3D_OK)
	{
		return result;
	}

	result = m_pDescriptorSet->GetBuffer(0, &m_pUniformBuffer);
	if (result != V3D_OK)
	{
		return result;
	}

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

V3D_RESULT Material::Initialize(GraphicsManager* pGraphicsManager, TextureManager* pTextureManager, MaterialManager* pMaterialManager, const Material::SerialData* pData)
{
	V3D_RESULT result = Initialize(pGraphicsManager, pMaterialManager, pData->name);
	if(result != V3D_OK)
	{
		return result;
	}

	m_Uniform.diffuseColor.r = pData->diffuseColor[0];
	m_Uniform.diffuseColor.g = pData->diffuseColor[1];
	m_Uniform.diffuseColor.b = pData->diffuseColor[2];
	m_Uniform.diffuseColor.a = pData->diffuseColor[3];
	m_Uniform.ambient = pData->ambient;
	m_Uniform.specularPower = pData->specularPower;
	m_Uniform.shininess = pData->shininess;

	m_Filter = static_cast<V3D_FILTER>(pData->filter);
	m_AddressMode = static_cast<V3D_ADDRESS_MODE>(pData->addressMode);

	if (wcslen(pData->diffuseTextureFilePath) > 0)
	{
		SetDiffuseTexture(pTextureManager->Load(pData->diffuseTextureFilePath));
	}

	if (wcslen(pData->specularTextureFilePath) > 0)
	{
		SetSpecularTexture(pTextureManager->Load(pData->specularTextureFilePath));
	}

	if (wcslen(pData->maskTextureFilePath) > 0)
	{
		SetMaskTexture(pTextureManager->Load(pData->maskTextureFilePath));
	}

	if (wcslen(pData->normalTextureFilePath) > 0)
	{
		SetNormalTexture(pTextureManager->Load(pData->normalTextureFilePath));
	}

	result = Update();
	if (result != V3D_OK)
	{
		return result;
	}

	return V3D_OK;
}

void Material::Dispose()
{
	m_DiffuseTexture = nullptr;
	m_NormalTexture = nullptr;

	SAFE_RELEASE(m_pDescriptorSet);
	SAFE_RELEASE(m_pUniformBuffer);

	if (m_pMaterialManager != nullptr)
	{
		m_pMaterialManager->ReleaseDescriptorSet(&m_UniformDynamicOffsetHandle);
	}

	for (auto i = 0; i < RENDER_TYPE_COUNT; i++)
	{
		m_PipelineHandles[i] = nullptr;
		SAFE_RELEASE(m_PipelineLayouts[i]);
	}

	m_pMaterialManager = nullptr;
	m_pGraphicsManager = nullptr;
}

bool Material::Serialize(Material::SerialData* pData)
{
	if ((sizeof(pData->name) >> 1) > m_Name.size())
	{
		wcscpy_s(pData->name, m_Name.data());
	}
	else
	{
		return false;
	}

	pData->diffuseColor[0] = m_Uniform.diffuseColor.r;
	pData->diffuseColor[1] = m_Uniform.diffuseColor.g;
	pData->diffuseColor[2] = m_Uniform.diffuseColor.b;
	pData->diffuseColor[3] = m_Uniform.diffuseColor.a;
	pData->ambient = m_Uniform.ambient;
	pData->specularPower = m_Uniform.specularPower;
	pData->shininess = m_Uniform.shininess;
	pData->filter = m_Filter;
	pData->addressMode = m_AddressMode;

	if (m_DiffuseTexture != nullptr)
	{
		if ((sizeof(pData->diffuseTextureFilePath) >> 1) > wcslen(m_DiffuseTexture->GetFilePath()))
		{
			wcscpy_s(pData->diffuseTextureFilePath, m_DiffuseTexture->GetFilePath());
		}
		else
		{
			return false;
		}
	}

	if (m_SpecularTexture != nullptr)
	{
		if ((sizeof(pData->specularTextureFilePath) >> 1) > wcslen(m_SpecularTexture->GetFilePath()))
		{
			wcscpy_s(pData->specularTextureFilePath, m_SpecularTexture->GetFilePath());
		}
		else
		{
			return false;
		}
	}

	if (m_MaskTexture != nullptr)
	{
		if ((sizeof(pData->maskTextureFilePath) >> 1) > wcslen(m_MaskTexture->GetFilePath()))
		{
			wcscpy_s(pData->maskTextureFilePath, m_MaskTexture->GetFilePath());
		}
		else
		{
			return false;
		}
	}

	if (m_NormalTexture != nullptr)
	{
		if ((sizeof(pData->normalTextureFilePath) >> 1) > wcslen(m_NormalTexture->GetFilePath()))
		{
			wcscpy_s(pData->normalTextureFilePath, m_NormalTexture->GetFilePath());
		}
		else
		{
			return false;
		}
	}

	return true;
}
