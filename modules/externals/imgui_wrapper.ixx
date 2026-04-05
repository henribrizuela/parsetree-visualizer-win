module;

#include "imgui.h"
#include "backends/imgui_impl_dx11.h"

#ifdef _WIN32
#include "backends/imgui_impl_win32.h"
#endif

export module imgui_wrapper;

export namespace ImGHuiWrapper {
	using namespace ImGui;
}