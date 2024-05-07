#include "CopyRenderer.h"

#include "base/ImGuiManager.h"

using namespace LWP::Base;

void CopyRenderer::Init() {/* 何もない！ */}

void CopyRenderer::Execute(ID3D12GraphicsCommandList* list) {
	for (std::vector<Target>::iterator it = target_.begin(); it != target_.end(); ++it) {
		// バリアを設定
		it->src->ChangeResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, list);
		it->dst->ChangeResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST, list);
		// コピー
		list->CopyResource(*it->dst, *it->src);
		// バリアを戻す
		it->src->ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);
		it->dst->ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);
	}
}

void CopyRenderer::Reset() {
	target_.clear();
}

