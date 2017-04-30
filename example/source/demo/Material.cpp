#include "Material.h"
#include "MaterialManager.h"

Material::Material() :
	m_pGraphicsManager(nullptr),
	m_pMaterialManager(nullptr),
	m_Key(0),
	m_pPipelineLayout(nullptr),
	m_PipelineDesc({}),
	m_PipelineHandle(nullptr),
	m_pUniformBuffer(nullptr),
	m_pUniformBufferView(nullptr),
	m_UniformBufferHandle(nullptr),
	m_pDescriptorSet(nullptr),
	m_Filter(V3D_FILTER_LINEAR),
	m_AddressMode(V3D_ADDRESS_MODE_REPEAT)
{
	m_PipelineDesc.polygonMode = V3D_POLYGON_MODE_FILL;
	m_PipelineDesc.cullMode = V3D_CULL_MODE_BACK;
	m_PipelineDesc.depthTestEnable = true;
	m_PipelineDesc.depthWriteEnable = true;
	m_PipelineDesc.blendMode = BLEND_MODE_COPY;

	m_Uniform.diffuseColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_Uniform.ambient = 0.0f;
	m_Uniform.specularPower = 60.0f;
	m_Uniform.shininess = 1.0f;
}

Material::~Material()
{
	Dispose();
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

	m_pGraphicsManager->GetPipelineLayout(GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY, GRAPHICS_PIPELINE_TYPE_GEOMETRY, &m_pPipelineLayout);

	V3D_RESULT result = m_pGraphicsManager->GetPipelineHandle(GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY, GRAPHICS_PIPELINE_TYPE_GEOMETRY, m_PipelineDesc, &m_PipelineHandle);
	if (result != V3D_OK)
	{
		return result;
	}

	// ----------------------------------------------------------------------------------------------------
	// ユニフォームバッファを作成
	// ----------------------------------------------------------------------------------------------------

	result = m_pMaterialManager->CreateUniformBuffer(&m_pUniformBuffer, &m_pUniformBufferView, &m_UniformBufferHandle);
	if (result != V3D_OK)
	{
		return result;
	}

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットを作成
	// ----------------------------------------------------------------------------------------------------

	result = m_pGraphicsManager->CreateDescriptorSet(GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY, GRAPHICS_PIPELINE_TYPE_GEOMETRY, GRAPHICS_DESCRIPTOR_SET_TYPE_MATERIAL, &m_pDescriptorSet);
	if (result != V3D_OK)
	{
		return result;
	}

	result = m_pDescriptorSet->SetBufferView(Material::BINDING_UNIFORM, m_pUniformBufferView);
	if (result != V3D_OK)
	{
		return result;
	}

	result = m_pDescriptorSet->SetImageViewAndSampler(
		Material::BINDING_ALBEDO_TEXTURE,
		m_pMaterialManager->GetDummyImageViewPtr(MaterialManager::DUMMY_IMAGE_TYPE_DIFFUSE),
		V3D_IMAGE_LAYOUT_SHADER_READ_ONLY,
		m_pMaterialManager->GetSamplerPtr(m_Filter, m_AddressMode));
	if (result != V3D_OK)
	{
		return result;
	}

	result = m_pDescriptorSet->SetImageViewAndSampler(
		Material::BINDING_NORMAL_TEXTURE,
		m_pMaterialManager->GetDummyImageViewPtr(MaterialManager::DUMMY_IMAGE_TYPE_NORMAL),
		V3D_IMAGE_LAYOUT_SHADER_READ_ONLY,
		m_pMaterialManager->GetSamplerPtr(V3D_FILTER_NEAREST, m_AddressMode));
	if (result != V3D_OK)
	{
		return result;
	}

	m_pDescriptorSet->Update();

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

void Material::Dispose()
{
	m_DiffuseTexture = nullptr;
	m_NormalTexture = nullptr;

	SAFE_RELEASE(m_pDescriptorSet);
	SAFE_RELEASE(m_pUniformBufferView);
	SAFE_RELEASE(m_pUniformBuffer);

	if ((m_pMaterialManager != nullptr) && (m_UniformBufferHandle != nullptr))
	{
		m_pMaterialManager->ReleaseUniformBuffer(m_UniformBufferHandle);
	}

	m_PipelineHandle = nullptr;
	SAFE_RELEASE(m_pPipelineLayout);

	m_pMaterialManager = nullptr;
	m_pGraphicsManager = nullptr;
}

uint64_t Material::GetKey() const
{
	return m_Key;
}

const wchar_t* Material::GetName() const
{
	return m_Name.c_str();
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
	return m_PipelineDesc.blendMode;
}

void Material::SetBlendMode(BLEND_MODE blendMode)
{
	m_PipelineDesc.blendMode = blendMode;
}

V3D_POLYGON_MODE Material::GetPolygonMode() const
{
	return m_PipelineDesc.polygonMode;
}

void Material::SetPolygonMode(V3D_POLYGON_MODE polygonMode)
{
	m_PipelineDesc.polygonMode = polygonMode;
}

V3D_CULL_MODE Material::GetCullMode() const
{
	return m_PipelineDesc.cullMode;
}

void Material::SetCullMode(V3D_CULL_MODE cullMode)
{
	m_PipelineDesc.cullMode = cullMode;
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
			Material::BINDING_ALBEDO_TEXTURE,
			m_DiffuseTexture->GetImageViewPtr(),
			V3D_IMAGE_LAYOUT_SHADER_READ_ONLY,
			m_pMaterialManager->GetSamplerPtr(m_Filter, m_AddressMode));
	}
	else
	{
		m_pDescriptorSet->SetImageViewAndSampler(
			Material::BINDING_ALBEDO_TEXTURE,
			m_pMaterialManager->GetDummyImageViewPtr(MaterialManager::DUMMY_IMAGE_TYPE_DIFFUSE),
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
	m_Uniform.shininess = MAXIMUM(1.0f, shininess);
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
	V3D_RESULT result = m_pGraphicsManager->GetPipelineHandle(GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY, GRAPHICS_PIPELINE_TYPE_GEOMETRY, m_PipelineDesc, &m_PipelineHandle);
	if (result != V3D_OK)
	{
		return result;
	}

	const V3DBufferSubresourceLayout& subresourceLayout = m_pUniformBuffer->GetSubresourceLayout(0);
	void* pMemory;

	result = m_pUniformBuffer->Map(subresourceLayout.offset, subresourceLayout.size, &pMemory);
	if (result != V3D_OK)
	{
		return result;
	}

	memcpy_s(pMemory, subresourceLayout.size, &m_Uniform, sizeof(Material::Uniform));

	result = m_pUniformBuffer->Unmap();
	if (result != V3D_OK)
	{
		return result;
	}

	m_pDescriptorSet->Update();

	return V3D_OK;
}

void Material::Bind(IV3DCommandBuffer* pCommandBuffer)
{
	pCommandBuffer->BindPipeline(m_PipelineHandle->GetPtr());
	pCommandBuffer->BindDescriptorSets(V3D_PIPELINE_TYPE_GRAPHICS, m_pPipelineLayout, 1, 1, &m_pDescriptorSet);
}
