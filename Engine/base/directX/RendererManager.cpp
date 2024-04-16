#include "RendererManager.h"

#include "primitive/IPrimitive.h"
#include "primitive/2d/Billboard2D.h"
#include "primitive/2d/Billboard3D.h"

using namespace LWP::Base;

void RendererManager::Init(GPUDevice* device, SRV* srv) {
	srv_ = srv;

	// コマンド初期化
	commander_.Init(device);
	// バッファー達を初期化
	buffers_.Init(device, srv_);
	// DXCはデフォルトコンストラクタで初期化済み

	// シャドウレンダラー初期化
	shadowRender_.Init(device, srv_, buffers_.GetRoot(), &dxc_);
	// ノーマルレンダラー初期化
	normalRender_.Init(device, srv_, buffers_.GetRoot(), &dxc_);

	// テクスチャ読み込み
	//defaultTexture_
}
void RendererManager::DrawCall() {
	// リストをポインタ化
	ID3D12GraphicsCommandList* list = commander_.List();

	// ** 共通の設定を先にしておく ** //

	// RootSignatureを設定。PSOに設定してるけど別途設定が必要
	list->SetGraphicsRootSignature(*buffers_.GetRoot());
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 描画用のSRVのDescriptorHeapを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srv_->GetHeap() };
	list->SetDescriptorHeaps(1, descriptorHeaps);

	// Buffer達のViewをセット
	buffers_.SetView(list);

	// シャドウ描画	
	//shadowRender_.DrawCall(list);
	// 通常描画
	normalRender_.DrawCall(list);
	// ポストプロセス描画

	commander_.Execute();

	// リセット
	normalRender_.Reset();
	shadowRender_.Reset();
	// リソースも解放
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

void RendererManager::AddParticleData(Primitive::IPrimitive* primitive, std::vector<Object::ParticleData>& wtf) {
	// IndexInfo構造体に加工
	IndexInfoStruct common = ProcessIndexInfo(primitive);
	// データを書き込む先を設定
	std::function<void(const IndexInfoStruct&)> sendTo = ProcessSendFunction(primitive);

	for (int w = 0; w < wtf.size(); w++) {
		IndexInfoStruct tfInfo = common;
		// ワールドトランスフォームをデータに登録
		WTFStruct wS;
		wS = primitive->transform;
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

	// ワールドトランスフォームをデータに登録
	WTFStruct wtf;
	wtf = primitive->transform;
	result.worldMatrix = buffers_.AddData(wtf);

	// マテリアルをデータに登録
	MaterialStruct m;
	m = primitive->material;
	result.material = buffers_.AddData(m);
	
	// テクスチャのインデックスを貰う
	result.tex2d = primitive->texture.t.GetIndex() != -1 ?
		primitive->texture.t.GetIndex() :
		defaultTexture_.GetIndex();

	// isUiセット
	result.isUI = primitive->isUI;

	return result;
}

std::function<void(const IndexInfoStruct&)> RendererManager::ProcessSendFunction(Primitive::IPrimitive* primitive) {
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
		if (primitive->material.enableLighting) {
			return [this](const IndexInfoStruct& index) { 
				normalRender_.AddIndexData(index); 
				shadowRender_.AddIndexData(index);
			};
		}

		return [this](const IndexInfoStruct& index) { normalRender_.AddIndexData(index); };
	}
}
