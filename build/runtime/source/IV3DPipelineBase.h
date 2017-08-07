#pragma once

class IV3DPipelineBase : public IV3DPipeline
{
public:
	struct Source
	{
		VkPipelineBindPoint pipelineBindPoint;
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;
		uint32_t constantCount;
		const V3DConstantDesc* pConstants;
	};

	virtual const IV3DPipelineBase::Source& GetSource() const = 0;

protected:
	virtual ~IV3DPipelineBase() {}
};
