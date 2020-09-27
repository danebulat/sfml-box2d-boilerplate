#ifndef EDGE_CHAIN_MANAGER
#define EDGE_CHAIN_MANAGER

#include <SFML/Graphics.hpp>
#include "utils/static_edge_chain.hpp"
#include "utils/box2d_utils.hpp"

class EdgeChainManager
{
private:
	std::vector<StaticEdgeChain>	m_chains;
	std::vector<std::string>		m_guiLabels;
	bool 		 					m_guiEnable;
	bool							m_guiDrawBB;

	int 					m_currSelectedIndex;
	int 					m_prevSelectedIndex;

	b2World* 						m_world;

public:
	EdgeChainManager(b2World* world);
	~EdgeChainManager();

	EdgeChainManager(const EdgeChainManager&) = delete;
	EdgeChainManager& operator== (const EdgeChainManager&) = delete;

	void CheckChainClicked(sf::RenderWindow& window);
	void SelectCurrentChain();

	void PushChain();
	void PopChain();

	void AddVertexToSelectedChain();
	void RemoveVertexFromSelectedChain();

	void HandleInput(const sf::Event& event, sf::RenderWindow& window);
	void Update(sf::RenderWindow& window);
	void Draw(sf::RenderWindow& window);

	/* Accessors */
	int  GetChainCount() const;
	int& GetSelectedChainIndex();

	void SyncEnable();
	void ToggleEnable();

	/* For ImGui */
	std::vector<std::string>& GetChainLabels();
	bool& GetEnableFlag();
	bool& GetDrawBBFlag();
};

#endif