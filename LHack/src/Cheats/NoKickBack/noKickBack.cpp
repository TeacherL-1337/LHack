#include "../cheatsManager.h"

float scale;

static void hkCreateVector(__int64 pEntity, FLOAT* pVector)
{
    if (pEntity == hookManager->GetLocalPlayer())
    {
        pVector[0] *= scale;
        pVector[1] *= scale;
        pVector[2] *= scale;

        oCreateVector(pEntity, pVector);
    }
    else
    {
        oCreateVector(pEntity, pVector);
    }
}

VOID CheatsManager::NoKickBack(BOOL Enable)
{
    MH_CreateHook((void*)hookManager->GetFunctionAddress(CreateVector), (void*)&hkCreateVector, (void**)&oCreateVector);

    if (Enable)
    {
        ImGui::SliderFloat("KickBackScale", &scale, 0.0f, 1.0f, "%.1f");
        MH_EnableHook((void*)hookManager->GetFunctionAddress(CreateVector));
    }
    else
    {
        MH_DisableHook((void*)hookManager->GetFunctionAddress(CreateVector));
    }
}