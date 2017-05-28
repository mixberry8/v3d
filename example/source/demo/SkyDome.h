#pragma once

#include "GraphicsManager.h"

class SkyDome
{
public:
	SkyDome();
	~SkyDome();

	bool Initialize(GraphicsManager* pGraphicsManager);
	void Dispose();

	void SetHemisphere(float radius, float height);

	const Vector3& GetSunPosition() const;
	void SetSunPosition(const Vector3& pos);

	const Vector3& GetSunDirection() const;
	const Vector4& GetSunColor() const;

	void Update(const Matrix4x4& viewProjMatrix, const Vector3& eyePos);

	void Draw(IV3DCommandBuffer* pCommandBuffer);

private:
	GraphicsManager* m_pGraphicsManager = nullptr;

	IV3DPipelineLayout* m_pPipelineLayout = nullptr;
	GraphicsPipelineHandle m_PipelineHandle = nullptr;

	IV3DBuffer* m_pUniformBuffer = nullptr;
	IV3DDescriptorSet* m_pDescriptorSet = nullptr;

	uint64_t m_VertexOffset = 0;
	uint64_t m_IndexOffset = 0;
	uint32_t m_IndexCount = 0;
	IV3DBuffer* m_pVertexIndexBuffer = nullptr;

	SkyDomeVertConstant m_VertConstant = {};
	SkyDomeFragUniform m_FragUniform = {};

	float m_OuterRadius = 0.0f;
	float m_InnerRadius = 0.0f;
	Vector3 m_CoreOffset;

	float m_SunIntensity = 0.0f;
	float m_SunTurbidity = 0.0f;
	float m_HenyeyGreenstein = 0.0f;
	float m_Rayleigh = 0.0f;
	float m_Mie = 0.0f;
	float m_Lin = 0.0f;
	Vector4 m_Fex;

	Vector4 m_NightColor;
	float m_NightThresholdCos = 0.0f;
	float m_NightOffset = 0.0f;

	Vector3 m_BetaR;
	Vector3 m_BetaDashR;
	Vector3 m_BetaM;
	Vector3 m_BetaDashM;

	float m_InvNPOffset = 0.0f;

	Vector3 m_SunPos;
	Vector3 m_ActualSunPos;
	Vector3 m_SunDir;
	Vector4 m_SunColor;
	Vector4 m_SunLightColor;

	Vector4 m_ActualNightColor;
};
