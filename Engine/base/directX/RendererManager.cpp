#include "RendererManager.h"

#include "component/Resource.h"
#include "primitive/IPrimitive.h"
#include "primitive/2d/Billboard2D.h"
#include "primitive/2d/Billboard3D.h"
#include "primitive/2d/Sprite.h"
#include "resources/model/ModelData.h"
#include "resources/model/Model.h"
#include "object/core/light/DirectionLight.h"
#include "object/core/light/PointLight.h"

using namespace LWP::Base;

void RendererManager::Init(GPUDevice* device, DXC* dxc, SRV* srv) {
	dxc_ = dxc;
	srv_ = srv;

	// コマンド初期化
	commander_.Init(device);
	// バッファー達を初期化
	buffers_.Init(device, srv_);
	// DXCはデフォルトコンストラクタで初期化済み

	// Viewをセットする関数を用意
	std::function<void()> shadowFunc = [&]() {
		ID3D12GraphicsCommandList* list = commander_.List();
		buffers_.SetVertexView(2, list);
		buffers_.SetTransformView(3, list);
	};
	std::function<void()> normalFunc = [&]() {
		ID3D12GraphicsCommandList* list = commander_.List();
		// RootSignatureを設定
		list->SetGraphicsRootSignature(*buffers_.GetRoot());
		// 各種Viewをセット
		buffers_.SetCommonView(2, list);
		buffers_.SetVertexView(3, list);
		buffers_.SetTransformView(4, list);
		buffers_.SetMaterialView(5, list);
		buffers_.SetDirLightView(6, list);
		buffers_.SetPointLightView(7, list);
		// テクスチャのViewをセット
		list->SetGraphicsRootDescriptorTable(8, srv_->GetFirstTexView());
		// シャドウマップのViewをセット
		list->SetGraphicsRootDescriptorTable(9, srv_->GetFirstDirShadowView());
		list->SetGraphicsRootDescriptorTable(10, srv_->GetFirstPointShadowView());
	};

	// シャドウレンダラー初期化
	shadowRender_.Init(device, srv_, dxc_, shadowFunc);
	// ノーマルレンダラー初期化
	normalRender_.Init(device, srv_, buffers_.GetRoot(), dxc_, normalFunc);
	// ポストプロセスレンダラー初期化
	ppRender_.Init();
	// コピーレンダラー初期化
	copyRenderer_.Init();

	// テクスチャ読み込み
	defaultTexture_ = Resource::LoadTextureLongPath("resources/system/texture/white.png");
}
void RendererManager::DrawCall() {
	// リストをポインタ化
	ID3D12GraphicsCommandList* list = commander_.List();

	// ** 共通の設定を先にしておく ** //

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 描画用のSRVのDescriptorHeapを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srv_->GetHeap() };
	list->SetDescriptorHeaps(1, descriptorHeaps);

	// シャドウ描画
	shadowRender_.DrawCall(list);
	// 通常描画
	normalRender_.DrawCall(list);
	// ポストプロセス描画
	ppRender_.DrawCall(list);
	// リソースをコピー
	copyRenderer_.Execute(list);

	// 実行
	commander_.Execute();

	// 次のフレームのためにリセット
	normalRender_.Reset();
	shadowRender_.Reset();
	ppRender_.Reset();
	copyRenderer_.Reset();
	// リソースもリセット
	buffers_.Reset();
}


void RendererManager::AddPrimitiveData(Primitive::IPrimitive* primitive) {
	// IndexInfo構造体に加工
	IndexInfoStruct info = ProcessIndexInfo(primitive);
	// データを書き込む先を設定
	std::function<void(const IndexInfoStruct&)> sendTo = ProcessSendFunction(primitive);

	// Indexの分だけIndexInfoを求める
	for (int i = 0; i < primitive->GetIndexCount(); i++) {
		IndexInfoStruct indexInfo = info;
		// インデックス分ずらす
		indexInfo.vertex += primitive->indexes[i];

		// 送信
		sendTo(indexInfo);
	}
}
void RendererManager::AddModelData(Resource::ModelData* data, const Resource::Model& modelIndex) {
	// IndexInfo構造体に加工
	IndexInfoStruct info = ProcessIndexInfo(data, modelIndex);
	// データを書き込む先を設定
	std::function<void(const IndexInfoStruct&)> sendTo = ProcessSendFunction(modelIndex);

	// 全メッシュに行う
	for (int m = 0; m < data->GetMeshCount(); m++) {
		// Indexの分だけIndexInfoを求める
		for (int i = 0; i < data->meshes_[m].GetIndexCount(); i++) {
			IndexInfoStruct indexInfo = info;
			// インデックス分ずらす
			indexInfo.vertex += data->meshes_[m].indexes[i];
			indexInfo.material += data->meshes_[m].materialIndex;

			// テクスチャのインデックスを貰う
			if (data->material_[data->meshes_[m].materialIndex].texture.t.GetIndex() != -1) {
				indexInfo.tex2d = data->material_[data->meshes_[m].materialIndex].texture.t.GetIndex();
			}
			else {
				indexInfo.tex2d = defaultTexture_.GetIndex();
			}
			// SRV上のオフセット分戻して考える
			indexInfo.tex2d -= lwpC::Rendering::kMaxBuffer;

			// 送信
			sendTo(indexInfo);
		}
	}
}


void RendererManager::AddParticleData(Primitive::IPrimitive* primitive,const std::vector<Object::ParticleData>& wtf) {
	// IndexInfo構造体に加工
	IndexInfoStruct common = ProcessIndexInfo(primitive);
	// データを書き込む先を設定
	std::function<void(const IndexInfoStruct&)> sendTo = ProcessSendFunction(primitive);

	for (int w = 0; w < wtf.size(); w++) {
		IndexInfoStruct tfInfo = common;
		// ワールドトランスフォームをデータに登録
		WTFStruct wS;
		wS = wtf[w].wtf;
		tfInfo.worldMatrix = buffers_.AddData(wS);
		// Indexの分だけIndexInfoを求める
		for (int i = 0; i < primitive->GetIndexCount(); i++) {
			IndexInfoStruct indexInfo = tfInfo;
			// インデックス分ずらす
			indexInfo.vertex += primitive->indexes[i];

			// 送信
			sendTo(indexInfo);
		}
	}
}

void RendererManager::AddLightData(Object::DirectionLight* light) { buffers_.AddData(*light); }

void RendererManager::AddLightData(Object::PointLight* light) { buffers_.AddData(*light); }

IndexInfoStruct RendererManager::ProcessIndexInfo(Primitive::IPrimitive* primitive) {
	IndexInfoStruct result;

	// あとでインデックス分ずらすので初期値
	result.vertex = buffers_.GetVertexCount();

	// 頂点データを登録
	for (int i = 0; i < primitive->GetVertexCount(); i++) {
		VertexStruct ver;
		ver = primitive->vertices[i];
		if (primitive->commonColor != nullptr)	// 共通の色があるときはcommonColorを適応
			ver.color_ = primitive->commonColor->GetVector4();
		buffers_.AddData(ver);	// データを追加
	}

	// ワールドトランスフォームをデータに登録（Nodeを考慮するために少し処理を変更）
	WTFStruct wtf;
	wtf = *primitive;
	result.worldMatrix = buffers_.AddData(wtf);

	// マテリアルをデータに登録
	MaterialStruct m;
	m = primitive->material;
	m.enableLighting = primitive->enableLighting;
	result.material = buffers_.AddData(m);
	
	// テクスチャのインデックスを貰う
	result.tex2d = primitive->texture.t.GetIndex() != -1 ?
		primitive->texture.t.GetIndex() :
		defaultTexture_.GetIndex();
	// SRV上のオフセット分戻して考える
	result.tex2d -= lwpC::Rendering::kMaxBuffer;

	// isUiセット
	result.isUI = primitive->isUI;

	return result;
}

IndexInfoStruct RendererManager::ProcessIndexInfo(Resource::ModelData* data, const Resource::Model& modelIndex) {
	IndexInfoStruct result;

	// あとでインデックス分ずらすので初期値
	result.vertex = buffers_.GetVertexCount();
	result.material = buffers_.GetMaterialCount();

	// 全メッシュに行う
	for (int m = 0; m < data->GetMeshCount(); m++) {
		// 頂点データを登録
		for (int i = 0; i < data->meshes_[m].GetVertexCount(); i++) {
			VertexStruct ver;
			ver = data->meshes_[m].vertices[i];
			buffers_.AddData(ver);	// データを追加
		}
	}

	// マテリアルを登録
	for (int i = 0; i < data->GetMaterialCount(); i++) {
		MaterialStruct m;
		m = data->material_[i];
		m.enableLighting = modelIndex.enableLighting;
		buffers_.AddData(m);	// データを追加
	}

	// ワールドトランスフォームをデータに登録
	WTFStruct wtf;
	wtf = modelIndex.worldTF;
	result.worldMatrix = buffers_.AddData(wtf);

	
	// isUiセット
	result.isUI = false;

	return result;
}

std::function<void(const IndexInfoStruct&)> RendererManager::ProcessSendFunction(Primitive::IPrimitive* primitive) {
	// Spriteのとき
	if (dynamic_cast<Primitive::Sprite*>(primitive)) {
		return [this](const IndexInfoStruct& index) { normalRender_.AddIndexDataSprite(index); };
	}
	// ビルボード2Dのとき
	if (dynamic_cast<Primitive::Billboard2D*>(primitive)) {
		return [this](const IndexInfoStruct& index) { normalRender_.AddIndexDataBillboard2D(index); };
	}
	// ビルボード3Dのとき
	else if (dynamic_cast<Primitive::Billboard3D*>(primitive)) {
		return [this](const IndexInfoStruct& index) { normalRender_.AddIndexDataBillboard3D(index); };
	}
	// ワイヤーフレームのとき
	else if (primitive->isWireFrame) {
		return [this](const IndexInfoStruct& index) { normalRender_.AddIndexDataWire(index); };
	}
	// どれでもないとき
	else {
		// シャドウマップにも描画するか確認
		if (primitive->enableLighting) {
			return [this](const IndexInfoStruct& index) { 
				normalRender_.AddIndexData(index); 
				shadowRender_.AddIndexData(index);
			};
		}

		return [this](const IndexInfoStruct& index) { normalRender_.AddIndexData(index); };
	}
}

std::function<void(const IndexInfoStruct&)> RendererManager::ProcessSendFunction(const Resource::Model& model) {
	// シャドウマップにも描画するか確認
	if (model.enableLighting) {
		return [this](const IndexInfoStruct& index) {
			normalRender_.AddIndexData(index);
			shadowRender_.AddIndexData(index);
		};
	}

	return [this](const IndexInfoStruct& index) { normalRender_.AddIndexData(index); };
}