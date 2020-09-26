#include "utils/font_store.hpp"
#include <memory>

using std::map;
using std::shared_ptr;
using std::string;
using sf::Font;

shared_ptr<FontStore> FontStore::m_instance;
string FontStore::m_fontFolder = "content/";

FontStore::FontStore()
{}

shared_ptr<FontStore> FontStore::GetInstance()
{
	if (m_instance.get() == nullptr)
		m_instance.reset(new FontStore);

	return m_instance;
}

void FontStore::AddFont(const string& filename)
{
	auto pos = m_map.find(filename);

	if (pos == m_map.end())
	{
		Font font;
		font.loadFromFile(m_fontFolder + filename);
		m_map[filename] = font;
	}
}

Font& FontStore::GetFont(const string& filename)
{
	auto pos = m_map.find(filename);

	if (pos == m_map.end())
	{
		AddFont(filename);
		return GetFont(filename);
	}
	else
	{
		return pos->second;
	}
}