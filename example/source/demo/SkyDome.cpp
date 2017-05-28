#include "SkyDome.h"

SkyDome::SkyDome()
{
	const Vector3 lambda(1.0f / 650e-9f, 1.0f / 570e-9f, 1.0f / 475e-9f);
	const Vector3 lambda2 = lambda * lambda;
	const Vector3 lambda4 = lambda2 * lambda2;

	// ----------------------------------------------------------------------------------------------------
	// Rayleigh
	// ----------------------------------------------------------------------------------------------------

	const float n = 1.0003f;
	const float pn = 0.035f;
	const float N = 2.545e25f;

	float tempRay = FLOAT_PI * FLOAT_PI * (n * n - 1.0f) * (n * n - 1.0f) * (6.0f + 3.0f * pn) / (6.0f - 7.0f * pn) / N;
	float beta = 8.0f * tempRay * FLOAT_PI / 3.0f;
	float betaDash = tempRay / 2.0f;

	m_BetaR = lambda4 * beta;
	m_BetaDashR = lambda4 * betaDash;

	// ----------------------------------------------------------------------------------------------------
	// Mie
	// ----------------------------------------------------------------------------------------------------

	const float T = 2.0f;
	const float c = (6.544f * T - 6.51f) * 1e-17f;
	const Vector3 K(0.685f, 0.679f, 0.670f);

	float tempMie0 = 0.434f * c * FLOAT_PI * (2.0f * FLOAT_PI) * (2.0f * FLOAT_PI);
	float tempMie1 = 0.434f * c * (2.0f * FLOAT_PI) * (2.0f * FLOAT_PI) * 0.5f;

	m_BetaM = K * lambda2 * tempMie0;
	m_BetaDashM = lambda2 * tempMie1;

	// ----------------------------------------------------------------------------------------------------
	// スキャッタリング : パラメータ
	// ----------------------------------------------------------------------------------------------------

	m_SunIntensity = 0.2f;
	m_SunTurbidity = 0.6f;
	m_HenyeyGreenstein = 0.93f;
	m_Rayleigh = 80.0f;
	m_Mie = 0.00060000003f;
	m_Lin = 0.909999967f;
	m_Fex.Set(0.138f, 0.113f, 0.08f, 0.1f);

	// ----------------------------------------------------------------------------------------------------
	// 夜 : パラメータ
	// ----------------------------------------------------------------------------------------------------

	m_NightColor.Set(0.02f, 0.02f, 0.04f, 1.0f);
	m_NightThresholdCos = 0.0f;
	m_NightOffset = 0.45f;

	m_InvNPOffset = 1.0f / m_NightOffset;

	// ----------------------------------------------------------------------------------------------------
}

SkyDome::~SkyDome()
{
	Dispose();
}

bool SkyDome::Initialize(GraphicsManager* pGraphicsManager)
{
	m_pGraphicsManager = pGraphicsManager;

	// ----------------------------------------------------------------------------------------------------
	// パイプラインを取得
	// ----------------------------------------------------------------------------------------------------

	GraphicsPipelienDesc pipelineDesc;
	pipelineDesc.blendMode = BLEND_MODE_NORMAL;
	pipelineDesc.polygonMode = V3D_POLYGON_MODE_FILL;
	pipelineDesc.cullMode = V3D_CULL_MODE_NONE;
	pipelineDesc.depthTestEnable = true;
	pipelineDesc.depthWriteEnable = false;

	m_pGraphicsManager->GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_DISTANT_VIEW_SKYDOME, &m_pPipelineLayout);

	V3D_RESULT result = m_pGraphicsManager->GetPipelineHandle(
		GRAPHICS_RENDERPASS_TYPE_DISTANT_VIEW, 0,
		GRAPHICS_PIPELINE_TYPE_DISTANT_VIEW_SKYDOME, pipelineDesc,
		&m_PipelineHandle);

	if (result != V3D_OK)
	{
		return false;
	}

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットを作成
	// ----------------------------------------------------------------------------------------------------

	V3DBufferDesc uniformBufferDesc;
	uniformBufferDesc.usageFlags = V3D_BUFFER_USAGE_UNIFORM;
	uniformBufferDesc.size = sizeof(SkyDomeFragUniform);

	result = m_pGraphicsManager->GetDevicePtr()->CreateBuffer(uniformBufferDesc, &m_pUniformBuffer);
	if (result != V3D_OK)
	{
		return false;
	}

	result = m_pGraphicsManager->GetDevicePtr()->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_HOST_VISIBLE, m_pUniformBuffer);
	if (result != V3D_OK)
	{
		return false;
	}

	result = m_pGraphicsManager->CreateDescriptorSet(GRAPHICS_PIPELINE_TYPE_DISTANT_VIEW_SKYDOME, GRAPHICS_DESCRIPTOR_SET_TYPE_SKYDOME, &m_pDescriptorSet);
	if (result != V3D_OK)
	{
		return false;
	}

	result = m_pDescriptorSet->SetBuffer(0, m_pUniformBuffer, 0, uniformBufferDesc.size);
	if (result != V3D_OK)
	{
		return false;
	}

	m_pDescriptorSet->Update();

	// ----------------------------------------------------------------------------------------------------
	// バーテックス、インデックスを作成
	// ----------------------------------------------------------------------------------------------------

	uint32_t domeDiv = 20;
	float domeRad = FLOAT_DIV(FLOAT_PI * 0.5f, static_cast<float>(domeDiv));
	uint32_t domeHorizSubdiv = static_cast<uint32_t>(FLOAT_DIV(FLOAT_PI * 2.0f, domeRad));
	uint32_t domeVertexCount = 1 + domeDiv * domeHorizSubdiv;
	uint32_t domeIndexCount = (domeHorizSubdiv * 3) + ((domeDiv - 1) * (domeHorizSubdiv * 6));

	std::vector<Vector3> vertices;
	std::vector<uint16_t> indices;

	vertices.reserve(domeVertexCount);
	indices.reserve(domeIndexCount);

	const Vector3 domeBasePos(0.0f, -1.0f, 0.0f);

	Quaternion domeRot;
	Vector3 domePos;

	//先端の頂点
	vertices.push_back(domeBasePos);

	//先端以降の頂点
	for (uint16_t v = 0; v < domeDiv; v++)
	{
		domeRot.SetRotationAxis(Vector3::XAxis(), domeRad * static_cast<float>(v + 1));

		for (uint16_t h = 0; h < domeHorizSubdiv; h++)
		{
			domePos = Matrix4x4(domeRot) * domeBasePos;
			vertices.push_back(domePos);

			domeRot.RotationAxis(Vector3::YAxis(), domeRad);
		}
	}

	//ドームの傘のインデックス
	for (uint16_t h = 0; h < domeHorizSubdiv; h++)
	{
		uint16_t i0 = h + 1;
		uint16_t i1 = (h == (domeHorizSubdiv - 1)) ? 1 : (h + 2);
		uint16_t i2 = 0;

		indices.push_back(i0);
		indices.push_back(i1);
		indices.push_back(i2);
	}

	//ドームの傘以降のインデックス
	for (uint16_t v = 0; v < (domeDiv - 1); v++)
	{
		for (uint16_t h = 0; h < domeHorizSubdiv; h++)
		{
			uint16_t tf = 1 + (v * domeHorizSubdiv);
			uint16_t tl = tf + h;
			uint16_t tr = (h == (domeHorizSubdiv - 1)) ? tf : (tl + 1);

			uint16_t bf = tf + domeHorizSubdiv;
			uint16_t bl = bf + h;
			uint16_t br = (h == (domeHorizSubdiv - 1)) ? bf : (bl + 1);

			indices.push_back(bl);
			indices.push_back(tr);
			indices.push_back(tl);

			indices.push_back(bl);
			indices.push_back(br);
			indices.push_back(tr);
		}
	}

	m_IndexCount = domeIndexCount;

	// ----------------------------------------------------------------------------------------------------
	// バーテックス、インデックスバッファーを作成
	// ----------------------------------------------------------------------------------------------------

	Array1<BufferSubresourceDesc, 2> tempBufferSubresources;
	tempBufferSubresources[0].usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_VERTEX;
	tempBufferSubresources[0].size = sizeof(Vector3) * vertices.size();
	tempBufferSubresources[0].count = 1;
	tempBufferSubresources[1].usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_INDEX;
	tempBufferSubresources[1].size = sizeof(uint16_t) * indices.size();
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

	result = m_pGraphicsManager->GetDevicePtr()->CreateBuffer(tempBufferDesc, &pTempBuffer);
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
	MemCopy(pAddr + tempBufferMemoryLayouts[0].offset, tempBufferMemoryLayouts[0].stride, vertices.data(), tempBufferSubresources[0].size);

	// インデックスをコピー
	MemCopy(pAddr + tempBufferMemoryLayouts[1].offset, tempBufferMemoryLayouts[1].stride, indices.data(), tempBufferSubresources[1].size);

	result = pTempBuffer->Unmap();
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pTempBuffer);
		return false;
	}

	// 描画用バッファを作成
	Array1<BufferSubresourceDesc, 2> bufferSubresources;
	bufferSubresources[0].usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_TRANSFER_DST | V3D_BUFFER_USAGE_VERTEX;
	bufferSubresources[0].size = sizeof(Vector3) * vertices.size();
	bufferSubresources[0].count = 1;
	bufferSubresources[1].usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_TRANSFER_DST | V3D_BUFFER_USAGE_INDEX;
	bufferSubresources[1].size = sizeof(uint16_t) * indices.size();
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

	result = m_pGraphicsManager->GetDevicePtr()->CreateBuffer(bufferDesc, &m_pVertexIndexBuffer);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pTempBuffer);
		return false;
	}

	result = m_pGraphicsManager->GetDevicePtr()->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_DEVICE_LOCAL, m_pVertexIndexBuffer);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pTempBuffer);
		return false;
	}

	// テンポラリバッファの内容を描画用バッファに転送
	IV3DCommandBuffer* pCommandBuffer = m_pGraphicsManager->ResetCommandBuffer();
	if (pCommandBuffer == nullptr)
	{
		SAFE_RELEASE(pTempBuffer);
		return false;
	}

	Array1<V3DCopyBufferRange, 2> copyBufferRanges;
	copyBufferRanges[0].dstOffset = bufferMemoryLayouts[0].offset;
	copyBufferRanges[0].srcOffset = tempBufferMemoryLayouts[0].offset;
	copyBufferRanges[0].size = tempBufferSubresources[0].size;
	copyBufferRanges[1].dstOffset = bufferMemoryLayouts[1].offset;
	copyBufferRanges[1].srcOffset = tempBufferMemoryLayouts[1].offset;
	copyBufferRanges[1].size = tempBufferSubresources[1].size;

	pCommandBuffer->CopyBuffer(m_pVertexIndexBuffer, pTempBuffer, TO_UI32(copyBufferRanges.size()), copyBufferRanges.data());

	if (m_pGraphicsManager->FlushCommandBuffer() == false)
	{
		SAFE_RELEASE(pTempBuffer);
		return false;
	}

	SAFE_RELEASE(pTempBuffer);

	m_VertexOffset = 0;
	m_IndexOffset = bufferMemoryLayouts[1].offset;

	// ----------------------------------------------------------------------------------------------------

	return true;
}

void SkyDome::Dispose()
{
	SAFE_RELEASE(m_pVertexIndexBuffer);
	SAFE_RELEASE(m_pDescriptorSet);
	SAFE_RELEASE(m_pUniformBuffer);
	SAFE_RELEASE(m_pPipelineLayout);
}

void SkyDome::SetHemisphere(float radius, float height)
{
	Vector2 p0(0.0f, height);
	Vector2 p1(-radius, 0.0f);
	Vector2 p2(+radius, 0.0f);

	float d1 = p0.GetLengthSqr();
	float d2 = p1.GetLengthSqr();
	float d3 = p2.GetLengthSqr();
	float u = FLOAT_DIV(0.5f, (p0.x * p1.y - p1.x * p0.y + p1.x * p2.y - p2.x * p1.y + p2.x * p0.y - p0.x * p2.y));
	float a = u * (d1 * p1.y - d2 * p0.y + d2 * p2.y - d3 * p1.y + d3 * p0.y - d1 * p2.y);
	float b = u * (p0.x * d2 - p1.x * d1 + p1.x * d3 - p2.x * d2 + p2.x * d1 - p0.x * d3);

	m_OuterRadius = ::sqrtf((a - p0.x) * (a - p0.x) + (b - p0.y) * (b - p0.y));
	m_InnerRadius = m_OuterRadius - height;
	m_CoreOffset.Set(0.0f, m_InnerRadius, 0.0f);
}

const Vector3& SkyDome::GetSunPosition() const
{
	return m_SunPos;
}

void SkyDome::SetSunPosition(const Vector3& pos)
{
	Vector3 normSunPos = pos.ToNormalize();
	Vector3 sunPos = normSunPos * m_OuterRadius;
	Vector3 habitatPos(0.0f, -m_InnerRadius, 0.0f);

	m_SunPos = pos;

	m_ActualSunPos = normSunPos * m_OuterRadius;
	m_SunDir = (habitatPos - sunPos).ToNormalize();
}

const Vector3& SkyDome::GetSunDirection() const
{
	return m_SunDir;
}

const Vector4& SkyDome::GetSunColor() const
{
	return m_SunLightColor;
}

void SkyDome::Update(const Matrix4x4& viewProjMatrix, const Vector3& eyePos)
{
	static const Vector3 LAMDA(0.65f, 0.57f, 0.475f);
	static const float ALPHA = 1.3f;

	float beta = 0.04608365822050f * m_SunTurbidity - 0.04586025928522f;
	float baseTheta = Vector3::Dot(-m_SunDir, -Vector3::YAxis());

	float theta;
	float deg;
	float m;

	// ----------------------------------------------------------------------------------------------------
	// 太陽の色
	// ----------------------------------------------------------------------------------------------------

	if (baseTheta >= 0.0f)
	{
		theta = baseTheta;
		deg = ::acos(theta) / FLOAT_PI * 180.0f;
	}
	else
	{
		theta = 0.0f;
		deg = 90.0f;
	}

	m = 1.0f / (theta + 0.15f * ::powf(93.885f - deg, -1.253f));

	for (uint32_t i = 0; i < 3; ++i)
	{
		float tauR = ::expf(-m * 0.008735f * ::powf(LAMDA.data[i], -4.08f));
		float tauA = ::expf(-m * beta * ::powf(LAMDA.data[i], -ALPHA));

		m_SunColor.data[i] = tauR * tauA;
	}

	// ----------------------------------------------------------------------------------------------------
	// 夜への移行
	// ----------------------------------------------------------------------------------------------------

	if (m_NightThresholdCos < baseTheta)
	{
		//昼
		m_SunColor = m_SunColor;
		m_ActualNightColor.Set(0.0f, 0.0f, 0.0f, 0.0f);
	}
	else
	{
		float diffCos = m_NightThresholdCos - baseTheta;

		m_ActualNightColor.r = m_NightColor.r * m_NightColor.a;
		m_ActualNightColor.g = m_NightColor.g * m_NightColor.a;
		m_ActualNightColor.b = m_NightColor.b * m_NightColor.a;

		if (diffCos < m_NightOffset)
		{
			//昼→夜へ移行中
			float ratio = CLAMP(diffCos * m_InvNPOffset, 0.0f, 1.0f);

			m_SunColor = Vector4::Lerp(m_SunColor, Vector4(0.0f, 0.0f, 0.0f, 0.0f), ratio);
			m_ActualNightColor.a = ratio;
		}
		else
		{
			//夜
			m_SunColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
			m_ActualNightColor.a = 1.0f;
		}
	}

	m_SunLightColor = m_SunColor;
	m_SunLightColor.a = CLAMP(m_SunColor.GetLength(), 0.0f, 1.0f);

	// ----------------------------------------------------------------------------------------------------
	// 定数の設定
	// ----------------------------------------------------------------------------------------------------

	Matrix4x4 transMat = Matrix4x4::Translation(m_CoreOffset);
	Matrix4x4 scaleMat = Matrix4x4::Scale(Vector3(m_OuterRadius, m_OuterRadius, m_OuterRadius));

	m_VertConstant.worldMatrix = transMat * scaleMat;
	m_VertConstant.worldViewProjMatrix = viewProjMatrix * m_VertConstant.worldMatrix;

	const float& hg = m_HenyeyGreenstein;

	m_FragUniform.eyePos = eyePos;

	m_FragUniform.sunDir = m_SunDir;

	m_FragUniform.sunColor = m_SunColor * 100.0f * m_SunIntensity;
	m_FragUniform.sunColor.a = 1.0f;

	m_FragUniform.nightColor = m_ActualNightColor;

	m_FragUniform.henyeyGreenstein.Set(1.0f - hg * hg, 1.0f + hg * hg, 2.0f * hg, 1.0f);

	m_FragUniform.betaDashR = m_BetaDashR * m_Rayleigh;
	m_FragUniform.betaDashM = m_BetaDashM * m_Mie;
	m_FragUniform.betaRM = (m_BetaR * m_Rayleigh) + (m_BetaM * m_Mie);

	m_FragUniform.oneOverBetaRM.Set(FLOAT_RECIPROCAL(m_FragUniform.betaRM.x),
		FLOAT_RECIPROCAL(m_FragUniform.betaRM.y),
		FLOAT_RECIPROCAL(m_FragUniform.betaRM.z),
		1.0f);

	m_FragUniform.mulitpliers.Set(m_Lin, m_Fex.x * m_Fex.w, m_Fex.y * m_Fex.w, m_Fex.z * m_Fex.w);

	void* pMemory;
	if (m_pUniformBuffer->Map(0, V3D_WHOLE_SIZE, &pMemory) == V3D_OK)
	{
		::memcpy_s(pMemory, m_pUniformBuffer->GetDesc().size, &m_FragUniform, sizeof(SkyDomeFragUniform));
		m_pUniformBuffer->Unmap();
	}
}

void SkyDome::Draw(IV3DCommandBuffer* pCommandBuffer)
{
	pCommandBuffer->BindPipeline(m_PipelineHandle->GetPtr());
	pCommandBuffer->PushConstant(m_pPipelineLayout, 0, &m_VertConstant);
	pCommandBuffer->BindDescriptorSets(V3D_PIPELINE_TYPE_GRAPHICS, m_pPipelineLayout, 0, 1, &m_pDescriptorSet, 0, nullptr);
	pCommandBuffer->BindVertexBuffers(0, 1, &m_pVertexIndexBuffer, &m_VertexOffset);
	pCommandBuffer->BindIndexBuffer(m_pVertexIndexBuffer, m_IndexOffset, V3D_INDEX_TYPE_UINT16);
	pCommandBuffer->DrawIndexed(m_IndexCount, 1, 0, 0, 0);
}
