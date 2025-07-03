#include "gui.h"

Vector3 worldPos{ 10, 4, 10 };
Vector2 screenPos{};

typedef HRESULT(__stdcall* ResizeBuffers)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
ResizeBuffers oResizeBuffers = nullptr;
ResizeBuffers pResizeBuffersTarget;

typedef HRESULT(__stdcall* Present)(IDXGISwapChain*, UINT, UINT);
Present oPresent = nullptr;
Present pPresentTarget;

static bool GetVTablePointer()
{
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = GetForegroundWindow();
	sd.SampleDesc.Count = 1;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	IDXGISwapChain* swapChain;
	ID3D11Device* device;

	const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (
		D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			0,
			featureLevels,
			2,
			D3D11_SDK_VERSION,
			&sd,
			&swapChain,
			&device,
			nullptr,
			nullptr) == S_OK
		)
	{
		void** pVTable = *reinterpret_cast<void***>(swapChain);
		swapChain->Release();
		device->Release();
		pPresentTarget = (Present)pVTable[8];
		pResizeBuffersTarget = (ResizeBuffers)pVTable[13];

		return true;
	}

	return false;
}

WNDPROC oWndProc;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
	{
		return true;
	}

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool bInit = false;
HWND window = NULL;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView = NULL;
static void CleanupRenderTarget()
{
	if (mainRenderTargetView)
	{
		mainRenderTargetView->Release();
		mainRenderTargetView = nullptr;
	}
}

static void CreateRenderTarget(IDXGISwapChain* pSwapChain)
{
	CleanupRenderTarget();

	ID3D11Texture2D* pBackBuffer = nullptr;
	pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

	if (pBackBuffer)
	{
		ID3D11Device* pDevice = nullptr;
		pSwapChain->GetDevice(IID_PPV_ARGS(&pDevice));

		if (pDevice)
		{
			pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView);
			pBackBuffer->Release();

			ImGui_ImplDX11_InvalidateDeviceObjects();
			ImGui_ImplDX11_CreateDeviceObjects();
		}
	}
}

bool guiEnable = false;
bool getPlayerList = false;

static void RenderUI()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	ImGui::Begin("LHack");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::Text("LocalPlayer: %p", hookManger->GetLocalPlayer());
	ImGui::Checkbox("GetPlayerList", &getPlayerList);
	if (getPlayerList)
	{
		ImGui::Text("PlayerList:");

		int i = 1;
		for (auto& ptr : hookManger->GetPlayerList())
		{
			ImGui::Text("	%d. %p", i, ptr);
			i++;
		}
	}

	ImGui::End();

	ImGui::EndFrame();
	ImGui::Render();
}

static HRESULT __stdcall hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	CleanupRenderTarget();

	HRESULT hr = oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	if (SUCCEEDED(hr))
	{
		CreateRenderTarget(pSwapChain);
	}

	return hr;
}

static HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags)
{
	if (!bInit)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
			ImGui_ImplWin32_Init(window);
			ImGui_ImplDX11_Init(pDevice, pContext);
			bInit = true;
		}
		else
		{
			return oPresent(pSwapChain, syncInterval, flags);
		}
	}

	RenderUI();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	if (mainRenderTargetView == nullptr)
	{
		CreateRenderTarget(pSwapChain);
	}

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return oPresent(pSwapChain, syncInterval, flags);
}

Gui::Gui()
{
	if (!GetVTablePointer())
	{
		throw std::runtime_error("Failed to get VTable");
	}

	MH_STATUS status = MH_Initialize();
	if (status != MH_OK)
	{
		if (status != MH_ERROR_ALREADY_INITIALIZED)
		{
			throw std::runtime_error("Failed to initialize MinHook");
		}
	}

	if (MH_CreateHook(reinterpret_cast<void**>(pPresentTarget), &hkPresent, reinterpret_cast<void**>(&oPresent)) != MH_OK)
	{
		throw std::runtime_error("Failed to create Hook.");
	}

	if (MH_CreateHook(reinterpret_cast<void**>(pResizeBuffersTarget), &hkResizeBuffers, reinterpret_cast<void**>(&oResizeBuffers)) != MH_OK)
	{
		throw std::runtime_error("Failed to create Hook.");
	}

	if (MH_EnableHook(pPresentTarget) != MH_OK)
	{
		throw std::runtime_error("Failed to enable hook.");
	}

	if (MH_EnableHook(pResizeBuffersTarget) != MH_OK)
	{
		throw std::runtime_error("Failed to enable hook.");
	}
}

BOOL Gui::Show(VOID)
{
	guiEnable = !guiEnable;

	return guiEnable;
}

VOID Gui::DisableAll(VOID)
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();


	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (mainRenderTargetView) { mainRenderTargetView->Release(); mainRenderTargetView = NULL; }
	if (pContext) { pContext->Release(); pContext = NULL; }
	if (pDevice) { pDevice->Release(); pDevice = NULL; }
	SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)(oWndProc));

}
