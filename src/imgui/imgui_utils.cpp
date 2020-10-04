#include "imgui/imgui_utils.hpp"

namespace ImGui
{

/*----------------------------------------------------------------------
 Function to create float inputs for a Rect
 ----------------------------------------------------------------------*/
bool InputRect(const char* label,
			utils::Rect* rect,
			int decimal_precision,
			ImGuiInputTextFlags extra_flags) {

	ImGui::PushID(label);	// id for group
	ImGui::BeginGroup();	// start group

	bool value_changed = false;
	std::array<float*, 4> arr = { &rect->x, &rect->y, &rect->w, &rect->h };

	for (auto& elem : arr) {
		ImGui::PushID(elem);			// id for InputFloat
		ImGui::PushItemWidth(64.f);		// width for InputFloat
		value_changed |= ImGui::InputFloat("##arr", elem, 0, 0,
			decimal_precision, extra_flags);
		ImGui::PopID();					// pop id for InputFloat
		ImGui::SameLine();				// same line as previous element
	}

	//ImGui::SameLine(); /* makes no difference */
	ImGui::TextUnformatted(label);

	ImGui::EndGroup();
	ImGui::PopID();

	return value_changed;
}

/*----------------------------------------------------------------------
 Static lambda and overridden functions to enable STL arrays and vectors
 of strings to populate an ImGui::ListBox and ImGui::Combo
 ----------------------------------------------------------------------*/

/** Lambda that sets the out text for the current item in ListBox or Combo
 *  based on data from a vector<string> object.
 *
 * Takes an array of data and index value.
 * Function is required to set `out_text` and return true or false.
 */
static auto vector_getter = [](void* vec, int idx, const char** out_text) {

	// cast `void* vec` parameter back to vector<string*>* and re-reference
	auto& vector = *static_cast<std::vector<std::string>*>(vec);

	// return false if idx is out of bounds
	if (idx < 0 || idx > static_cast<int>(vector.size())) { return false; }

	// set `out_text*` buffer to const char* stored in string at idx in vector
	*out_text = vector.at(idx).c_str();

	return true;
};

/** Overridden Combo function to accept std::vector<std::string>&
 */
bool Combo(const char* label,                   // widget label
		   int* current_item,                   // start index of item to draw
		   std::vector<std::string>& values) {  // data vector

	if (values.empty()) { return false; }		// return false if vector is empty

	// Draw ImGui::Combo widget
	return Combo(label, 						// widget label
				 current_item, 					// start index of item to draw
				 vector_getter,					// getter function bool(*items_getter)(void* data, int idx, const char* out_text)
				 static_cast<void*>(&values),	// &vector array cast to void*
				 values.size());				// number of items to draw to combo
}

/** Overridden ListBox function to accept std::vector<std::string>&
 */
bool ListBox(const char* label,						// widget label
			 int* current_item,					    // start index of item to draw
			 std::vector<std::string>& values) {	// data vector

	if (values.empty()) { return false; }

	return ListBox(label,						// widget label
				   current_item,				// start index of item to draw
				   vector_getter,				// getter function bool(*items_getter)(void* data, int idx, const char* out_text)
				   static_cast<void*>(&values),	// &vector cast to void*
				   values.size());			    // number of items to draw to list box
}

/*----------------------------------------------------------------------
  Wrapper function that lets us pass a lambda with state to and
  InputText callback function
  ----------------------------------------------------------------------*/
template <typename F>
bool InputTextCool(const char* label,			// widget label
			   char* buf,						// InputText buffer
			   size_t buf_size,					// size of buffer
			   ImGuiInputTextFlags flags,   	// InputText flags
			   F callback,          			// callback when data changes
			   void* user_data) {       		// user data passed to callback

	// stateless callback to pass to ImGui::InputText
	auto free_callback = [](ImGuiTextEditCallbackData* data) {
		auto& f = *static_cast<F*>(data->UserData);
		return f(data);		// call lambda with state
	};

	return ImGui::InputText(label,				// widget label
							buf,				// InputText buffer
							buf_size,			// size of buffer
							flags,				// InputText flags
							free_callback,		// stateless callback
							&callback);			// state callback (called in free_callback)
}

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
void HelpMarker(const char* desc)
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
bool HalfWidthLabelCheckoxLeft(const char* label, const char* label_id, const char* help,
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
bool HalfWidthLabelCheckoxRight(const char* label, const char* label_id, const char* help,
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
bool FullWidthLabelCheckox(const char* label, const char* label_id, const char* help,
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
bool StartColorButton(int id, int col, const char* label, float btn_width, float btn_height, bool same_line)
{
	ImGui::PushID(id+10);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(col / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(col / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(col / 7.0f, 0.8f, 0.8f));
	if (same_line)
		ImGui::SameLine();
	return ImGui::Button(label, ImVec2(btn_width, btn_height));
}

void StopColorButton()
{
	ImGui::PopStyleColor(3);
	ImGui::PopID();
}

/* Util: Render a label with a colored integer */
void LabelWithColoredFloat(const char* label, const ImVec4& color,
	int data, bool same_line)
{
	if (same_line)
		ImGui::SameLine();
	ImGui::Text("%s", label);
	ImGui::SameLine();
	ImGui::TextColored(color, "%d", data);
}

/* Util: Render application controls */
void RenderEditorControls()
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

} // namespace ImGui