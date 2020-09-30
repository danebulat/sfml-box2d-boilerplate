#ifndef EDGE_CHAIN_MANAGER
#define EDGE_CHAIN_MANAGER

#include <SFML/Graphics.hpp>
#include "editor/chains/static_edge_chain.hpp"
#include "editor/box2d_utils.hpp"

class EdgeChainManager final : public ChainManagerController
{
private:
	std::vector<StaticEdgeChain>	m_chains;
	std::vector<std::string>		m_guiLabels;
	bool 		 					m_guiEnable;
	bool							m_guiDrawBB;

	int 					m_currSelectedIndex;
	int 					m_prevSelectedIndex;

	b2World* 						m_world;

	unsigned long			m_edgeChainCount;
	unsigned long 			m_edgeChainVertexCount;

public:
	EdgeChainManager(b2World* world);
	virtual ~EdgeChainManager();

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
	bool* GetEnableFlag();
	bool* GetDrawBBFlag();

	/* ChainManagerController Interface Implementaiton */
	virtual void IncrementEdgeChainCount(unsigned long n) override;
	virtual unsigned long* GetEdgeChainCount() override;

	virtual void IncrementEdgeChainVertexCount(unsigned long n) override;
	virtual unsigned long* GetEdgeChainVertexCount() override;
};

#endif