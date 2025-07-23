#include "PrimitiveManager.h"
#include "base/ImGuiManager.h"

#include "object/ObjectManager.h"
#include "Config.h"
#include "utility/MyUtility.h"

using namespace LWP::Math;

namespace LWP::Primitive {
	PlaneBuffers::PlaneBuffers() :
		vertices(lwpC::Rendering::Primitive::Sprite::kMaxCount * 4),
		wtf(lwpC::Rendering::Primitive::Sprite::kMaxCount),
		materials(lwpC::Rendering::Primitive::Sprite::kMaxCount) {
		vertices.Init();
		wtf.Init();
		materials.Init();
		count = 0;
	}
	void PlaneBuffers::Reset() {
		vertices.Reset();
		wtf.Reset();
		materials.Reset();
		count = 0;
	}


	Manager::Manager() :
		zSort_(lwpC::Rendering::Primitive::Billboaed::kMaxCount),
		type_(lwpC::Rendering::Primitive::Billboaed::kMaxCount),
		velocities_(lwpC::Rendering::Primitive::Billboaed::kStretchedMaxCount) {
		zSort_.Init();
		type_.Init();
		velocities_.Init();
	}
	void Manager::Init() {
		//for (IPrimitive* p : primitives_.list) {
		//	delete p;
		//}
		//primitives_.list.clear();
		primitiveCount_ = 0;

		for (IPrimitive* p : debugPris) {
			delete p;
		}
		debugPris.clear();
	}

	void Manager::Update() {
		// 全てのバッファーをリセット
		planeBuffers_[PlaneRenderType::Sprite].Reset();
		planeBuffers_[PlaneRenderType::Billboard].Reset();
		zSort_.Reset();
		type_.Reset();
		velocities_.Reset();

		int i = 0;
		struct BillboardIndex {
			int index;
			float distance;
		};
		std::vector<BillboardIndex> indexes;
		Math::Vector3 cameraPos = Object::Manager::GetInstance()->GetMainCamera()->worldTF.GetWorldPosition();

		// 全更新処理
		for (IPlane* p : planes_.list) {
			p->Update();

			// バッファーにデータをセット
			if (p->isActive) {
				IPlane::Type type = p->GetType();
				PlaneBuffers* b = nullptr;

				// スプライト以外の場合（ビルボード系の場合）
				if (type != IPlane::Type::Sprite) {
					// ビルボードのバッファを指定
					b = &planeBuffers_[PlaneRenderType::Billboard];
					type_.Add(static_cast<int>(type));	// タイプを登録
					// Zソート用にデータを登録
					float d = Vector3::Distance(p->GetCenterPosition(), cameraPos);
					indexes.push_back({ i++, d });
				}
				else {
					// スプライトのバッファを指定
					b = &planeBuffers_[PlaneRenderType::Sprite];
				}

				// 指定のタイプのバッファにデータを登録
				for (const Vertex& v : p->vertices) { b->vertices.Add(v); }
				b->materials.Add(p->material);
				b->wtf.Add(p->worldTF);
				b->count++;


				// ↓ タイプ別の特殊な処理 ↓

				// ストレッチビルボードの場合
				if(type == IPlane::Type::StretchedBillboard){
					// ストレッチビルボードの速度を登録
					IStretchedBillboard* sb = dynamic_cast<IStretchedBillboard*>(p);
					velocities_.Add(sb->velocity);
				}
			}
		}

		// Zソート
		std::sort(indexes.begin(), indexes.end(), 
			[](const BillboardIndex& a, const BillboardIndex& b) {
				return a.distance > b.distance;
			}
		);
		// ソート結果をバッファに格納
		for (BillboardIndex& b : indexes) { zSort_.Add(b.index); }
	}

	void Manager::DebugGUI() {
		// 指定の種類のPrimitiveを絞り込む選択肢
		static std::vector<const char*> filterText = {
			"All", "Sprite", "Surface", "Billboard2D", "HorizontalBillboard", "VerticalBillboard", "StretchedBillboard"
		};
		static int filterID = 0;
		// 選択肢の変数
		static std::vector<const char*> typeText = {
			"Sprite", "Surface", "Billboard2D", "HorizontalBillboard", "VerticalBillboard", "StretchedBillboard",
		};
		static int typeID = 0;
		static std::vector<const char*> policyText = {
			"Normal","Sequence","Clip",
		};
		static int policyID = 0;

		// 新規生成用のインスタンスを作成する関数のリスト
		static std::vector<std::vector<std::function<void()>>> functions = {
			{
				[this]() { debugPris.push_back(new NormalSprite()); },
				[this]() { debugPris.push_back(new SequenceSprite()); },
				[this]() { debugPris.push_back(new ClipSprite()); },
			},
			{
				[this]() { debugPris.push_back(new NormalSurface()); },
				[this]() { debugPris.push_back(new SequenceSurface()); },
				[this]() { debugPris.push_back(new ClipSurface()); },
			},
			{
				[this]() { debugPris.push_back(new NormalBillboard2D()); },
				[this]() { debugPris.push_back(new SequenceBillboard2D()); },
				[this]() { debugPris.push_back(new ClipBillboard2D()); },
			},
			{
				[this]() { debugPris.push_back(new NormalHorizontalBillboard()); },
				[this]() { debugPris.push_back(new SequenceHorizontalBillboard()); },
				[this]() { debugPris.push_back(new ClipHorizontalBillboard()); },
			},
			{
				[this]() { debugPris.push_back(new NormalVerticalBillboard()); },
				[this]() { debugPris.push_back(new SequenceVerticalBillboard()); },
				[this]() { debugPris.push_back(new ClipVerticalBillboard()); },
			},
			{
				[this]() { debugPris.push_back(new NormalStretchedBillboard()); },
				[this]() { debugPris.push_back(new SequenceStretchedBillboard()); },
				[this]() { debugPris.push_back(new ClipStretchedBillboard()); },
			},
		};
		static int currentItem = 0;
		
		if (ImGui::BeginTabItem("Primitive")) {
			// インスタンス生成用のUI
			if (ImGui::BeginTable("ComboTable", 2, ImGuiTableFlags_SizingFixedFit)) {
				ImGui::TableSetupColumn("Type Select", ImGuiTableColumnFlags_WidthStretch);   // ラベル列
				ImGui::TableSetupColumn("Policy Select", ImGuiTableColumnFlags_WidthStretch);        // Combo列 (Stretch or Fixed with wider width)

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Type Select");
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("Policy Select");

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::SetNextItemWidth(-FLT_MIN); // セルの左端から右端まで広げる
				ImGui::Combo("##TypeCombo", &typeID, typeText.data(), static_cast<int>(typeText.size()));
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(-FLT_MIN); // セルの左端から右端まで広げる
				ImGui::Combo("##PolicyCombo", &policyID, policyText.data(), static_cast<int>(policyText.size()));

				ImGui::EndTable();
			}
			if (ImGui::Button("Create New Instance")) { functions[typeID][policyID](); }
			ImGui::Text("-------------------------");
			ImGui::Combo("Filter by Type", &filterID, filterText.data(), static_cast<int>(filterText.size()));
			// インスタンス一覧
			if (!planes_.list.empty()) {
				std::vector<const char*> itemText;
				std::vector<IPlane*> ptr;
				for (IPlane* p : planes_.list) {
					if (filterID != 0) {	// All以外の時
						// 選択されたタイプ以外は表示しない
						if (p->GetType() != static_cast<IPlane::Type>(filterID - 1)) { continue; }
					}
					itemText.push_back(p->name.c_str());
					ptr.push_back(p);
				}
				ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);
				
				if (!ptr.empty() && 0 <= currentItem && currentItem < ptr.size()) {
					ptr[currentItem]->DebugGUI();
				}
				else {
					ImGui::Text("No Primitive selected.");
				}
			}

			ImGui::EndTabItem();
		}
	}

	void Manager::SetPlanePtr(IPlane* ptr) {
		ptr->name += "_" + std::to_string(primitiveCount_++);	// 数字だけ渡す
		planes_.SetPtr(ptr);
	}
}