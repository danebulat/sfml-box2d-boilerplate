#ifndef IMGUI_UTILS_HPP
#define IMGUI_UTILS_HPP

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <array>
#include "imgui.h"

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
void HelpMarker(const char* desc);

/* Util: Renders a label & checkbox on left half of window */
bool HalfWidthLabelCheckoxLeft(const char* label, const char* label_id, const char* help,
	bool* ptr_bool);

/* Util: Renders a label & checkbox on right half of window */
bool HalfWidthLabelCheckoxRight(const char* label, const char* label_id, const char* help,
	bool* ptr_bool);

/* Util: Renders a label & checkbox on right half of window */
bool FullWidthLabelCheckox(const char* label, const char* label_id, const char* help,
	bool* ptr_bool);

/* Util: Renders a colors button with custom text label and button width */
bool StartColorButton(int id, int col, const char* label, float btn_width,
	float btn_height, bool same_line);

void StopColorButton();

/* Util: Render a label with a colored integer */
void LabelWithColoredFloat(const char* label, const ImVec4& color,
	int data, bool same_line);

/* Util: Render application controls */
void RenderEditorControls();

/*----------------------------------------------------------------------
 Function to create float inputs for a Rect
 ----------------------------------------------------------------------*/
bool InputRect(const char* label,
			utils::Rect* rect,
			int decimal_precision = -1,
			ImGuiInputTextFlags extra_flags = 0);

/* Overridden Combo function to accept std::vector<std::string>& */
bool Combo(const char* label,                   	// widget label
		   int* current_item,                   	// start index of item to draw
		   std::vector<std::string>& values);   	// data vector

/* Overridden ListBox function to accept std::vector<std::string>& */
bool ListBox(const char* label,						// widget label
			 int* current_item,					    // start index of item to draw
			 std::vector<std::string>& values);  	// data vector

/*----------------------------------------------------------------------
  Wrapper function that lets us pass a lambda with state to and
  InputText callback function
  ----------------------------------------------------------------------*/
template <typename F>
bool InputTextCool(const char* label,				// widget label
			   char* buf,							// InputText buffer
			   size_t buf_size,						// size of buffer
			   ImGuiInputTextFlags flags = 0,		// InputText flags
			   F callback = nullptr,				// callback when data changes
			   void* user_data = nullptr);			// user data passed to callback

}// namespace ImGui

#endif