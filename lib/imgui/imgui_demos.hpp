#ifndef IMGUI_DEMOS_HPP
#define IMGUI_DEMOS_HPP

#include "imgui_utils.hpp"

/*
NOTE: Only include this file in one .CPP file.
TODO: Separate implementation in .CPP file
*/

/*----------------------------------------------------------------------
  Demo1 class
  ----------------------------------------------------------------------*/
class Demo1 {
private:
	char      m_windowTitle[255] = { '\0' };
	sf::Color m_bg_color;
	int       m_integer;
	bool      m_open;

	float     m_pos[2] = { 10.f, 20.f };
	float     m_color[3] = { 0.f, 0.f, 0.f };

public:
	Demo1()
		: m_windowTitle("Hello, ImGui")
		, m_bg_color(sf::Color::Black)
		, m_integer(32)
		, m_open(true)
	{}

	sf::Color getBackgroundColor() const {
		return m_bg_color;
	}

	const char* getWindowTitle() const {
		return m_windowTitle;
	}

	void run(sf::RenderWindow& window) {
		/** Begin window **/
		ImGui::Begin("Sample window");

		// Background color edit
		if (ImGui::ColorEdit3("Background color", m_color)) {

			// Output changed color to console
			std::cout << "color[r, g, b] = " << m_color[0] << ' ' << m_color[1] << ' ' << m_color[2] << std::endl;

			// this code gets called if color value changes, so
			// the background color is upgraded automatically
			m_bg_color.r = static_cast<sf::Uint8>(m_color[0] * 255.f);
			m_bg_color.g = static_cast<sf::Uint8>(m_color[1] * 255.f);
			m_bg_color.b = static_cast<sf::Uint8>(m_color[2] * 255.f);
		}

		// Creates a text edit with "Window title" label
		// Pass in windowTitle[] char array as data source for text edit
		if (ImGui::InputText("Window title", m_windowTitle, 255)) {
			std::cout << "new windowTitle[] = " << m_windowTitle << std::endl;
		}

		// Create a button with label "Update window title"
		if (ImGui::Button("Update window title")) {
			// this code gets if user clicks on the button
			// you could have written if(ImGui::InputText(...))
			// but this shows how buttons work
			window.setTitle(m_windowTitle);
		}

		if (ImGui::InputInt("Integer value", &m_integer)) {
			std::cout << "new integer: " << m_integer << std::endl;
		}

		if (ImGui::InputFloat2("position", m_pos)) {
			std::cout << "new pos: " << m_pos[0] << ',' << m_pos[1] << std::endl;
		}

		if (ImGui::ArrowButton("Left Arrow Button", ImGuiDir_Left)) {
			std::cout << "left arrow clicked\n";
		}

		if (ImGui::ArrowButton("Up Arrow Button", ImGuiDir_Up)) {
			std::cout << "up arrow clicked\n";
		}

		ImGui::End();
		/** End window **/

		ImGui::ShowAboutWindow(&m_open);
		ImGui::ShowDemoWindow(&m_open);
		ImGui::ShowMetricsWindow(&m_open);
	}
};

/*----------------------------------------------------------------------
  Demo2 class
  ----------------------------------------------------------------------*/
class Demo2 {
private:
	utils::Point m_point;
	utils::Rect  m_rect;

	int m_combo_index;
	int m_list_index;

	std::array<int, 4> 	 	 m_arr1;
	std::vector<int>   		 m_arr2;
	std::vector<std::string> m_combo_vec;
	std::vector<std::string> m_list_vec;
	std::vector<char>		 m_text_input_buffer;

	std::array<char, 100> m_text_arr;
	const char m_replacement_char = 'D';

private:

	// free function callback for InputText widget
	// makes every inputted character an 'A'
	static int callback(ImGuiTextEditCallbackData* data) {
		data->EventChar = 'A';
		return 0;
	}

	// callback examples
	void run_callback_examples() {

		// passing a free callback function to InputText
		ImGui::InputText(
			"Text",										// widget label
			m_text_arr.data(),							// input text buffer
			m_text_arr.size(),							// input text buffer size
			ImGuiInputTextFlags_CallbackCharFilter,  	// char filter flag
			callback);									// callback function

		// you can also pass a stateless lambda instead of a free function
		ImGui::InputText(
			"Text2",									// widget label
			m_text_arr.data(),							// input text buffer
			m_text_arr.size(),							// input text buffer size
			ImGuiInputTextFlags_CallbackCharFilter,	   	// char filter flag
			[](ImGuiTextEditCallbackData* data) {	   	// callback lambda
				data->EventChar = 'A';
				return 0;
			});

		// suppose we want to use a char variable instead of a hard-coded 'A'
		ImGui::InputText(
			"Text3",									// widget label
			m_text_arr.data(),							// input text buffer
			m_text_arr.size(),							// input text buffer size
			ImGuiInputTextFlags_CallbackCharFilter,		// char filter flag
			[](ImGuiInputTextCallbackData* data) {		// callback lambda
				data->EventChar = *static_cast<const char*>(data->UserData);
				return 0;
			},
			const_cast<char*>(&m_replacement_char));	// attached user data

		// passing a lambda with state with custom ImGui::InputTextCool function
		char replacement_char = 'R';

		if (ImGui::InputTextCool(
			"Text4",												// widget label
			m_text_arr.data(),										// input text buffer
			m_text_arr.size(),										// input text buffer size
			ImGuiInputTextFlags_CallbackCharFilter,					// char filter flag
			[&replacement_char](ImGuiTextEditCallbackData* data) {  // lambda with state
				data->EventChar = replacement_char;
				std::cout << "Replaced with: " << replacement_char << "\n";
				return 0;
			})) {
			/* Do stuff when text changes */
		}

		/* --------------------------------------------------------------------------------------
		Callback example 3: Generalising the approach
		(requires boost function_traits / template programming)
		// TODO: https://eliasdaler.github.io/using-imgui-with-sfml-pt2/
		--------------------------------------------------------------------------------------- */
	}

public:
	Demo2() : m_combo_index(0)
			, m_list_index(0) {

		m_arr1 = { 0 };
		m_text_arr = { '\0' };
		m_arr2.resize(4, 0);

		m_combo_vec = { "Camera Animation", "Circle Tween", "Easing Functions" };
		m_list_vec = { "Camera Animation", "Circle Tween", "Easing Functions" };
		m_text_input_buffer = {  't', 'e', 's', 't' };
	}

	void run(sf::RenderWindow& window) {
		ImGui::Begin("Sample Window");

		// using "##" to define unique labels but with same display string
		if (ImGui::Button("Same Label")) {
			std::cout << "Button 1 pushed" << std::endl;
		}

		if (ImGui::Button("Same Label##Second")) {
			std::cout << "Button 2 pushed" << std::endl;
		}

		// Using an array to define input boxes
		std::array<int, 4> arr = { 0 };

		for (int i = 0; i < arr.size(); ++i) {
			ImGui::PushID(i);
			ImGui::InputInt("##", &arr[i]);
			ImGui::PopID();
		}
		ImGui::End();

		// create another window
		ImGui::Begin("Sample Window #2");

		if (ImGui::Button("Button")) {
			std::cout << "button clicked" << std::endl;
		}
		ImGui::End();

		// append to an existing window
		ImGui::Begin("Sample Window");

		if (ImGui::Button("Appended Button")) {
			std::cout << "appended button clicked" << std::endl;
		}

		// using structs with POD - interpreted as array of floats
		if (ImGui::InputFloat2("Point", &m_point.x)) {
			std::cout << "point changed" << std::endl;
		}

		// using structs with POD - interpreted as array of floats
		if (ImGui::InputFloat4("Rect", &m_rect.x)) {
			std::cout << "rect changed" << std::endl;
		}

		// call custom ImGui::InputRect function
		if (ImGui::InputRect("Rect Custom", &m_rect)) {
			std::cout << "rect changed #2" << std::endl;
		}

		// call std::array::data() and std::vector::data() to pass the raw
		// pointer to the internal array to ImGui
		ImGui::InputInt4("STL Array", m_arr1.data());
		ImGui::InputInt4("STL Vector", m_arr2.data());

		// Use custom Combo and Listbox function to pass std::vector to
		// ComboBox and ListBox
		ImGui::BeginGroup();
		ImGui::PushItemWidth(140.f);
		if (ImGui::Combo("Sample Combo", &m_combo_index, m_combo_vec)) {
			std::cout << "> combo change: " << m_combo_vec[m_combo_index] << "\n";
		}
		ImGui::SameLine();

		ImGui::PushItemWidth(140.f);
		if (ImGui::ListBox("Sample List", &m_list_index, m_list_vec)) {
			std::cout << "> list change: " << m_list_vec[m_list_index] << "\n";
		}
		ImGui::SameLine();
		ImGui::EndGroup();

		// InputText and std::string
		// Can pass vector<char> or array<char> to InputText and initialise string to vector.at()
		ImGui::InputText("Sample Input",
			m_text_input_buffer.data(), m_text_input_buffer.size() + 1);

		run_callback_examples();

		ImGui::End();
	}
};

/*----------------------------------------------------------------------
  Demo3 class
  ----------------------------------------------------------------------*/
class Demo3 {
private:
	bool  m_show_window;
	float m_float;
	bool  m_tool_active;

	char  m_buf[100]  = { '\0' };
	float m_color[4]  = { 0.f };
	float m_floats[2] = { 0.f };

public:
	Demo3() : m_show_window(true)
			, m_float(.0f)
			, m_tool_active(true)
	{}

	void run(sf::RenderWindow& window) {
		// Hello, World Sample window
		if(m_show_window)
		{
			if (!ImGui::Begin("Sample Window", &m_show_window)) {
				ImGui::End();
			}
			else {
				ImGui::Text("Hello, world %d", 123);
				if (ImGui::Button("Save")) {
					std::cout << "Call MySaveFunction()\n";
				}
				ImGui::InputText("string", m_buf, IM_ARRAYSIZE(m_buf));
				ImGui::SliderFloat("float", &m_float, 0.f, 1.f);
				ImGui::SliderFloat2("float2", m_floats, 0.f, 1.f);

				ImGui::End();
			}
		}

		// Create a window called "My First Tool" with a menu bar
		ImGui::Begin("My First Tool", &m_tool_active, ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Open..", "Ctrl+O")) {  std::cout << "> Open()\n"; }
				if (ImGui::MenuItem("Save", "Ctrl+S")) { std::cout << "> Save()\n"; }
				if (ImGui::MenuItem("Close", "Ctrl+W")) { std::cout << "> Close()\n"; }
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Button to show other sample window
		if (ImGui::Button("Toggle Sample Window")) {
			m_show_window = !m_show_window;
		}

		// Edit a color (stored as ~4 floats)
		ImGui::ColorEdit4("Color", m_color);

		// Plot some values
		const float my_values[] = { .2f, .1f, 1.f, .5f, .9f, 2.2f };
		ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

		// Display contents in a scrolling region
		ImGui::TextColored(ImVec4(1,1,0,1), "Important Stuff");
		ImGui::BeginChild("Scrolling");
		for (int n = 0; n < 50; ++n) {
			ImGui::Text("%04d: Some text", n);
		}
		ImGui::EndChild();
		ImGui::End();
	}
};

/*----------------------------------------------------------------------
  DemoManager class
  ----------------------------------------------------------------------*/
class DemoManager {
private:
	Demo1 m_demo1;
	Demo2 m_demo2;
	Demo3 m_demo3;

	unsigned int m_current_demo;

	std::vector<std::string> m_demo_names;

private:
	void draw_demo_selector() {
		ImGui::Begin("Select Demo");

		int current_item = (m_current_demo - 1);

		if (ImGui::Combo("Demos", &current_item, m_demo_names)) {
			if (m_demo_names[current_item] == "Demo 1") {
				set_current_demo(1);
			}
			else if (m_demo_names[current_item] == "Demo 2") {
				set_current_demo(2);
			}
			else if (m_demo_names[current_item] == "Demo 3") {
				set_current_demo(3);
			}
		}
		ImGui::End();
	}

public:
	DemoManager() : m_current_demo(1) {
		m_demo_names = { "Demo 1", "Demo 2", "Demo 3" };
	}

	void initialise(sf::RenderWindow& window) {
		window.setTitle(m_demo1.getWindowTitle());
	}

	void update(sf::RenderWindow& window) {
		draw_demo_selector();

		switch (m_current_demo) {
			case 1:  m_demo1.run(window); break;
			case 2:  m_demo2.run(window); break;
			case 3:  m_demo3.run(window); break;
			default: m_demo1.run(window); break;
		}

		if (m_current_demo == 1)
			window.clear(m_demo1.getBackgroundColor());
		else
			window.clear();
	}

	const uint get_current_demo() const {
		return m_current_demo;
	}

	void set_current_demo(const uint demo) {
		m_current_demo = demo;
	}
};

#endif