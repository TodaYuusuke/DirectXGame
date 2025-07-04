#pragma once
#include "Copy.h"

#include "object/ObjectManager.h"
#include "resources/ResourceManager.h"

using namespace LWP::Resource;

namespace LWP::Base {
	void Copy::PushCommand(IRenderingResource* src, IRenderingResource* dst, ID3D12GraphicsCommandList6* list) {
		assert(src != dst && "Copying to the same resource is prohibited.");	// 同じリソースへのコピーは禁止
		// 解像度が同一でない場合はエラー
		assert((src->width == dst->width) && (src->height == dst->height) && "Source and destination resources must have the same resolution.");	// ソースとデスティネーションの解像度が同じでなければならない

		// バリアを設定
		src->ChangeResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, list);
		dst->ChangeResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST, list);

		// Mip数が同じならば
		if (src->GetMipLevels() == dst->GetMipLevels()) {
			list->CopyResource(*dst, *src);	// 通常コピー
		}
		// Mip数が異なるならば
		else {
			// CopyTextureRegion でコピー

			// src側の設定
			D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
			srcLocation.pResource = *src;
			srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			srcLocation.SubresourceIndex = 0;
			// dst側の設定
			D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
			dstLocation.pResource = *dst;
			dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			dstLocation.SubresourceIndex = 0;

			list->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);
		}

		// バリアを戻す
		//src->RevertResourceBarrier(list);
		//dst->RevertResourceBarrier(list);
	}
}