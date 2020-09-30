#ifndef FONT_STORE_HPP
#define FONT_STORE_HPP

#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <string>

class FontStore {
private:
	// Pointer to the only instance of this class
	static std::shared_ptr<FontStore> m_instance;

	// Private constructor, only the class can instantiate itself
	FontStore();

	static std::string m_fontFolder;

private:
	std::map<std::string, sf::Font> m_map;

public:
	// Public static method to return the pointer to the only instance
	static std::shared_ptr<FontStore> GetInstance();

	// Business logic performed by class
	void AddFont(const std::string& filename);

	sf::Font& GetFont(const std::string& filename);
};

#endif