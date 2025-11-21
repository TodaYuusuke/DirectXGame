#include "PrimitiveManager.h"
#include "base/ImGuiManager.h"

#include "object/ObjectManager.h"
#include "Config.h"

using namespace LWP::Math;

namespace LWP::Primitive {
	PlaneBuffers::PlaneBuffers(int bufferSize) :
		vertices(bufferSize * 4),
		wtf(bufferSize),
		materials(bufferSize),
		zSort(bufferSize) {
		vertices.Init();
		wtf.Init();
		materials.Init();
		zSort.Init();
		count = 0;
	}
	void PlaneBuffers::Reset() {
		vertices.Reset();
		wtf.Reset();
		materials.Reset();
		zSort.Reset();
		count = 0;
	}


	Manager::Manager() :
		planeBuffers_{
			PlaneBuffers(lwpC::Rendering::Primitive::Sprite::kMaxCount),
			PlaneBuffers(lwpC::Rendering::Primitive::Billboaed::kMaxCount)
		}, 
		type_(lwpC::Rendering::Primitive::Billboaed::kMaxCount),
		velocities_(lwpC::Rendering::Primitive::Billboaed::kMaxCount) {
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
		type_.Reset();
		velocities_.Reset();

		int sI = 0;
		int bI = 0;
		struct SortIndex {
			int index;
			float distance;
		};
		std::vector<SortIndex> spriteIndex;
		std::vector<SortIndex> billIndex;
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
					billIndex.push_back({ bI++, d });
				}
				else {
					// スプライトのバッファを指定
					b = &planeBuffers_[PlaneRenderType::Sprite];
					// Zソート用にデータを登録
					spriteIndex.push_back({ sI++, p->worldTF.GetWorldPosition().z });	// 奥行だけを考慮する
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
				// ストレッチビルボードでない　かつ　スプライトでない場合
				else if(type != IPlane::Type::Sprite){
					// 速度に空のベクトルを登録
					velocities_.Add({ 0.0f,0.0f,0.0f });
				}
			}
		}

		// Zソート
		std::sort(spriteIndex.begin(), spriteIndex.end(),
			[](const SortIndex& a, const SortIndex& b) { return a.distance > b.distance; }
		);
		std::sort(billIndex.begin(), billIndex.end(),
			[](const SortIndex& a, const SortIndex& b) { return a.distance > b.distance; }
		);
		// ソート結果をバッファに格納
		for (SortIndex& b : spriteIndex) { planeBuffers_[PlaneRenderType::Sprite].zSort.Add(b.index); }
		for (SortIndex& b : billIndex) { planeBuffers_[PlaneRenderType::Billboard].zSort.Add(b.index); }
	}

	void Manager::DebugGUI() {
		static int mode = 0;

		if (ImGui::BeginTabItem("Primitive")) {
			ImGui::RadioButton("Plane", &mode, 0);
			ImGui::SameLine();
			ImGui::RadioButton("3DPrimitive", &mode, 1);
			ImGui::Text("---------- Create New Instance ----------");

			if (mode == 0) {
				DebugGUIPlane();
			}
			else {
				DebugGUI3DPrimitive();
			}

			ImGui::EndTabItem();
		}
	}

	void Manager::DebugGUIPlane() {
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
		if (ImGui::Button("Create")) { functions[typeID][policyID](); }
		ImGui::Text("---------- Instance List ----------");
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
	}
	void Manager::DebugGUI3DPrimitive() {
		// 指定の種類のPrimitiveを絞り込む選択肢
		static std::vector<const char*> filterText = {
			"All", "Cube", "Sphere", "Cylinder", "Capsule"
		};
		static int filterID = 0;
		// 選択肢の変数
		static std::vector<const char*> typeText = {
			"Cube", "Sphere", "Cylinder", "Capsule"
		};
		static int typeID = 0;

		// 新規生成用のインスタンスを作成する関数のリスト
		static std::vector<std::function<void()>> functions = {
			[this]() { debug3DPris.push_back(new Cube()); },
			[this]() { debug3DPris.push_back(new Sphere()); },
			[this]() { debug3DPris.push_back(new Cylinder()); },
			[this]() { debug3DPris.push_back(new Capsule()); },
		};
		static int currentItem = 0;

		// インスタンス生成用のUI
		if (ImGui::BeginTable("ComboTable", 1, ImGuiTableFlags_SizingFixedFit)) {
			ImGui::TableSetupColumn("Type Select", ImGuiTableColumnFlags_WidthStretch);   // ラベル列

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Type Select");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::SetNextItemWidth(-FLT_MIN); // セルの左端から右端まで広げる
			ImGui::Combo("##TypeCombo", &typeID, typeText.data(), static_cast<int>(typeText.size()));

			ImGui::EndTable();
		}
		if (ImGui::Button("Create")) { functions[typeID](); }
		ImGui::Text("---------- Instance List ----------");
		ImGui::Combo("Filter by Type", &filterID, filterText.data(), static_cast<int>(filterText.size()));
		// インスタンス一覧
		if (!pris3D_.list.empty()) {
			std::vector<const char*> itemText;
			std::vector<IPrimitive3D*> ptr;
			for (IPrimitive3D* p : pris3D_.list) {
				if (filterID != 0) {	// All以外の時
					// 選択されたタイプ以外は表示しない
					if (p->GetType() != static_cast<IPrimitive3D::Type>(filterID - 1)) { continue; }
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
	}

	void Manager::SetPlanePtr(IPlane* ptr) {
		ptr->name += "_" + std::to_string(primitiveCount_++);	// 数字だけ渡す
		planes_.SetPtr(ptr);
	}
	void Manager::SetPri3DPtr(IPrimitive3D* ptr) {
		ptr->name += "_" + std::to_string(primitiveCount_++);	// 数字だけ渡す
		pris3D_.SetPtr(ptr);
	}
}