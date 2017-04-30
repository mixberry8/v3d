#pragma once

class IV3DPipelineBase : public IV3DPipeline
{
public:
	struct Source
	{
		VkPipelineBindPoint pipelineBindPoint;
		VkPipeline pipeline;
	};

	virtual const IV3DPipelineBase::Source& GetSource() const = 0;
	virtual void AfterBind(VkCommandBuffer commandBuffer) = 0;

protected:
	virtual ~IV3DPipelineBase() {}
};
