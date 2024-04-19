#include "Texture.h"
#include "base/directX/resource/rendering/TextureResource.h"
#include "base/directX/resource/rendering/RenderResource.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Resource;

Texture::Texture(const Base::TextureResource& r) {
	*this = r;
}
Texture& Texture::operator=(const TextureResource& r) {
	index_ = r.GetSRVIndex();
	size_ = r.GetTextureSize();
	return *this;
}

Texture::Texture(const Base::RenderResource& r) {
	*this = r;
}
Texture& Texture::operator=(const RenderResource& r) {
	index_ = r.GetSRVIndex();
	size_ = r.GetTextureSize();
	return *this;
}