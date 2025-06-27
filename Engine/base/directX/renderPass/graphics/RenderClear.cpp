#pragma once
#include "RenderClear.h"

#include "object/ObjectManager.h"
#include "../../resource/rendering/IRenderingResource.h"

using namespace LWP;

namespace LWP::Base {
	void RenderClear::PushCommand(BackBuffer* resource, ID3D12GraphicsCommandList6* list) {
		resource->ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);
		resource->Clear(list);
		resource->RevertResourceBarrier(list);
	}
	void RenderClear::PushCommand(RenderResource* resource, ID3D12GraphicsCommandList6* list) {
		resource->ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);
		resource->Clear(list);
		resource->RevertResourceBarrier(list);
	}
	void RenderClear::PushCommand(DepthStencil* resource, ID3D12GraphicsCommandList6* list) {
		resource->ChangeResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE, list);
		resource->Clear(list);
		resource->RevertResourceBarrier(list);
	}

	void RenderClear::ClearAllCamera(ID3D12GraphicsCommandList6* list) {
		auto& cameras = Object::Manager::GetInstance()->GetCameras();
		for (auto& camera : cameras) {
			PushCommand(camera->GetRenderResource(), list);
			PushCommand(camera->GetDepthStencil(), list);
		}
	}
}