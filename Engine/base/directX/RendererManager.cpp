#include "RendererManager.h"

#include "component/Resource.h"
#include "primitive/IPrimitive.h"
#include "primitive/2d/Billboard2D.h"
#include "primitive/2d/Billboard3D.h"
#include "primitive/2d/Sprite.h"
#include "resources/model/ModelData.h"
#include "object/core/light/DirectionLight.h"
#include "object/core/light/PointLight.h"

using namespace LWP::Base;

void RendererManager::Init() {
	srv_ = SRV::GetInstance();	// SRVはよく使うので保持

	// コマンド初期化
	commander_.Init(GPUDevice::GetInstance());
	// コマンドをSRVに渡しておく
	SRV::GetInstance()->SetCommand(&commander_);


	// デフォルトテクスチャ読み込み
	defaultTexture_ = Resource::LoadTextureLongPath("resources/system/texture/white.png");
	// バッファー達を初期化
	buffers_.Init();
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
	std::function<void()> meshFunc = [&]() {
		ID3D12GraphicsCommandList* list = commander_.List();
		// 各種Viewをセット
		buffers_.SetCommonView(6, list);
		buffers_.SetDirLightView(9, list);
		buffers_.SetPointLightView(10, list);
	};
	std::function<void()> particleFunc = [&]() {
		ID3D12GraphicsCommandList* list = commander_.List();
		// 各種Viewをセット
		buffers_.SetCommonView(6, list);
		buffers_.SetDirLightView(11, list);
		buffers_.SetPointLightView(12, list);
	};

	// シャドウレンダラー初期化
	shadowRender_.Init(shadowFunc);
	eMapRenderer_.Init(meshFunc);
	eMapRenderer_.SetBufferGroup(&buffers_);
	// ノーマルレンダラー初期化
	normalRender_.Init(buffers_.GetRoot(), normalFunc);
	postRenderer_.Init(buffers_.GetRoot(), normalFunc);
	meshRenderer_.Init(&commander_, meshFunc);
	meshRenderer_.SetBufferGroup(&buffers_);
	particleRenderer_.Init(&commander_, particleFunc);
	particleRenderer_.SetBufferGroup(&buffers_);
	// ポストプロセスレンダラー初期化
	ppRender_.Init();
	// コピーレンダラー初期化
	copyRenderer_.Init();
}
void RendererManager::DrawCall() {
	// リストをポインタ化
	ID3D12GraphicsCommandList6* list = commander_.List();

	// ** 共通の設定を先にしておく ** //

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 描画用のSRVのDescriptorHeapを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srv_->GetHeap() };
	list->SetDescriptorHeaps(1, descriptorHeaps);

	// シャドウ描画
	shadowRender_.DrawCall(list);
	eMapRenderer_.DrawCall(list);
	// 通常描画
	meshRenderer_.DrawCall(list);
	normalRender_.DrawCall(list);
	// パーティクル更新＆描画
	particleRenderer_.DrawCall(list);
	// ポストプロセス描画
	ppRender_.DrawCall(list);
	// スプライト描画
	postRenderer_.DrawCall(list);
	// リソースをコピー
	copyRenderer_.Execute(list);

	// 実行
	commander_.Execute();

	// 次のフレームのためにリセット
	normalRender_.Reset();
	postRenderer_.Reset();
	meshRenderer_.Reset();
	particleRenderer_.Reset();
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
		buffers_.AddData(ver);	// データを追加
	}

	// ワールドトランスフォームをデータに登録（Nodeを考慮するために少し処理を変更）
	WTFStruct wtf;
	wtf = *primitive;
	result.worldMatrix = buffers_.AddData(wtf);

	// マテリアルをデータに登録
	MaterialStruct m;
	m = primitive->material;
	result.material = buffers_.AddData(m);

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
	// Spriteのとき
	else if (dynamic_cast<Primitive::Sprite*>(primitive)) {
		return [this](const IndexInfoStruct& index) { postRenderer_.AddIndexDataSprite(index); };
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