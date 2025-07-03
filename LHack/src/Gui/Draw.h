#pragma once

#include"../../ext/ImGui/imgui.h"
#include"../../ext/ImGui/imgui_impl_dx11.h"
#include"../../ext/ImGui/imgui_impl_win32.h"
#include <stdio.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

inline ImFont* g_font = NULL;

namespace draw
{
	inline VOID FilledRect(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, ImVec4 Color)
	{
		ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(X, Y), ImVec2(X + W, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(Color)), 0, 0);
	}

	inline VOID Line(FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2, ImVec4 Color, FLOAT Width)
	{
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(X1, Y1), ImVec2(X2, Y2), ImGui::ColorConvertFloat4ToU32(ImVec4(Color)), Width);
	}

	inline VOID FilledBox(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, ImVec4 Color)
	{
		Line(X, Y, (X + W), Y, Color, 0.1f);
		Line(X, (Y + H), (X + W), (Y + H), Color, 0.1f);
		Line(X, Y, X, (Y + H), Color, 0.1f);
		Line((X + W), Y, (X + W), (Y + H), Color, 0.1f);
	}

	inline VOID CircleFilled(FLOAT X, FLOAT Y, FLOAT Radius, ImVec4 Color, INT Segments)
	{
		ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(X, Y), Radius, ImGui::ColorConvertFloat4ToU32(ImVec4(Color)), Segments);
	}

	inline VOID Text(FLOAT X, FLOAT Y, ImVec4 Color, FLOAT Size, CONST CHAR* Str, ...)
	{
		CHAR Buffer[1024] = { 0 };
		va_list va_alist;
		va_start(va_alist, Str);
		vsprintf_s(Buffer, Str, va_alist);
		va_end(va_alist);
		ImGui::GetForegroundDrawList()->AddText(g_font, Size, ImVec2(X, Y), ImGui::ColorConvertFloat4ToU32(ImVec4(Color)), Buffer);
	}

	inline FLOAT TextCenter(FLOAT X, FLOAT Y, ImVec4 Color, FLOAT Size, CONST CHAR* Str, ...)
	{
		CHAR Buffer[1024] = { 0 };
		va_list va_alist;
		va_start(va_alist, Str);
		vsprintf_s(Buffer, Str, va_alist);
		va_end(va_alist);
		ImVec2 TextSize = g_font->CalcTextSizeA(g_font->FontSize, D3D11_FLOAT32_MAX, 0.0f, Buffer);
		ImGui::GetForegroundDrawList()->AddText(NULL, Size, ImVec2(X - TextSize.x / 2, Y), ImGui::ColorConvertFloat4ToU32(ImVec4(Color)), Buffer);
		return X - TextSize.x / 2;
	}

}