#include "IUtility.h"
#include "UtilityManager.h"

namespace LWP::Utility {
	IUtility::IUtility() { Manager::GetInstance()->SetPtr(this); }
	IUtility::~IUtility() { Manager::GetInstance()->DeletePtr(this); }
}