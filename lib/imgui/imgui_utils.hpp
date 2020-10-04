#ifndef IMGUI_UTILS_HPP
#define IMGUI_UTILS_HPP

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <array>
#include "imgui.h"

/**
 * NOTE: Static ImGui helper functions marked inline to prevent GCC unused warnings.
 * The unused warning is displayed even though the functions are being called in
 * imgui_manager.cpp.
*/

namespace utils {

	struct Rect {
		float x;
		float y;
		float w;
		float h;
	};

	struct Point {
		float x;
		float y;
	};
}

namespace ImGui
{

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

/* Util: Renders a label & checkbox on left half of window */
static inline bool HalfWidthLabelCheckoxLeft(const char* label, const char* label_id, const char* help,
	bool* ptr_bool)
{
	float windowWidth = ImGui::GetWindowContentRegionWidth();

	ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", label);

	ImGui::SameLine();
	ImGui::HelpMarker(help);

	ImGui::SameLine((windowWidth/2) * 0.8f);
	return ImGui::Checkbox(label_id, ptr_bool);
}

/* Util: Renders a label & checkbox on right half of window */
static inline bool HalfWidthLabelCheckoxRight(const char* label, const char* label_id, const char* help,
	bool* ptr_bool)
{
	float windowWidth = ImGui::GetWindowContentRegionWidth();

	ImGui::AlignTextToFramePadding();
	ImGui::SameLine(windowWidth/2);
    ImGui::Text("%s", label);

	ImGui::SameLine();
	ImGui::HelpMarker(help);

	ImGui::SetNextItemWidth(-1);
	ImGui::SameLine(windowWidth - 20.f);
	return ImGui::Checkbox(label_id, ptr_bool);
}

/* Util: Renders a label & checkbox on right half of window */
static inline bool FullWidthLabelCheckox(const char* label, const char* label_id, const char* help,
	bool* ptr_bool)
{
	float windowWidth = ImGui::GetWindowContentRegionWidth();

	ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", label);

	ImGui::SameLine();
	ImGui::HelpMarker(help);

	ImGui::SetNextItemWidth(-1);
	ImGui::SameLine(windowWidth - 20.f);
	return ImGui::Checkbox(label_id, ptr_bool);
}

/* Util: Renders a colors button with custom text label and button width */
static inline bool StartColorButton(int id, int col, const char* label, float btn_width, float btn_height, bool same_line)
{
	ImGui::PushID(id+10);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(col / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(col / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(col / 7.0f, 0.8f, 0.8f));
	if (same_line)
		ImGui::SameLine();
	return ImGui::Button(label, ImVec2(btn_width, btn_height));
}

static inline void StopColorButton()
{
	ImGui::PopStyleColor(3);
	ImGui::PopID();
}

/* Util: Render a label with a colored integer */
static inline void LabelWithColoredFloat(const char* label, const ImVec4& color,
	int data, bool same_line)
{
	if (same_line)
		ImGui::SameLine();
	ImGui::Text("%s", label);
	ImGui::SameLine();
	ImGui::TextColored(color, "%d", data);
}

/* Util: Render application controls */
static inline void RenderEditorControls()
{
	std::vector<std::pair<std::string, std::string>> controls =
	{
		std::make_pair("Enter", "Spawn custom polygon"),
		std::make_pair("Space", "Cycle RMB modes"),
		std::make_pair("E", 	"Toggle edge chain active state"),
		std::make_pair("W", 	"Toggle wireframe rendering mode"),
		std::make_pair("Esc", 	"Close window"),
		std::make_pair("\nMMB", "\nSpawn circle rigid body"),
		std::make_pair("RMB", 	"Perform selected RMB mode"),
	};
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
		ImGui::BeginChild("ControlsChild", ImVec2(0, 145.f), true, ImGuiWindowFlags_None);

		for (auto& control : controls)
		{
			ImVec4 itemColor(1.f, 1.f, .4f, 1.f);
			ImGui::TextColored(itemColor, "%s", control.first.data());
			ImGui::SameLine(80);
			ImGui::Text("%s", control.second.data());
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();
	}
}

/*----------------------------------------------------------------------
 Function to create float inputs for a Rect
 ----------------------------------------------------------------------*/
bool InputRect(const char* label,
			utils::Rect* rect,
			int decimal_precision = -1,
			ImGuiInputTextFlags extra_flags = 0);


/** Overridden Combo function to accept std::vector<std::string>&
 */

bool Combo(const char* label,                   // widget label
		   int* current_item,                   // start index of item to draw
		   std::vector<std::string>& values);  // data vector

/** Overridden ListBox function to accept std::vector<std::string>&
 */
bool ListBox(const char* label,						// widget label
			 int* current_item,					    // start index of item to draw
			 std::vector<std::string>& values);  	// data vector

/*----------------------------------------------------------------------
  Wrapper function that lets us pass a lambda with state to and
  InputText callback function
  ----------------------------------------------------------------------*/
template <typename F>
bool InputTextCool(const char* label,			// widget label
			   char* buf,						// InputText buffer
			   size_t buf_size,					// size of buffer
			   ImGuiInputTextFlags flags = 0,	// InputText flags
			   F callback = nullptr,			// callback when data changes
			   void* user_data = nullptr);		// user data passed to callback

}// namespace ImGui

#endif