#include "editor/managers/edge_chain_manager.hpp"

EdgeChainManager::EdgeChainManager(b2World* world)
{
	m_edgeChainCount = 0;
	m_edgeChainVertexCount = 0;

	m_guiEnable = true;	// Enable all chains
	m_guiDrawBB = true;	// Draw bounding box on selected chain

	m_world = world;

	// TMP - Add test chains
	m_chains.push_back(StaticEdgeChain(demo_data::coords, Vector2f(0,0), "EC1", m_world, this));
	m_chains.push_back(StaticEdgeChain(demo_data::coordsLeft, Vector2f(0,0), "EC2", m_world, this));
	m_chains.push_back(StaticEdgeChain(demo_data::coordsRight, Vector2f(0,0), "EC3", m_world, this));

	m_edgeChainCount = 3;
	m_edgeChainVertexCount += demo_data::coords.size();
	m_edgeChainVertexCount += demo_data::coordsLeft.size();
	m_edgeChainVertexCount += demo_data::coordsRight.size();
	// END TMP

	m_chains[0].SetEditable(true);
	m_chains[0].DrawBoundingBox(true);
	m_currSelectedIndex = 0;
	m_prevSelectedIndex = 0;

	for (auto& chain : m_chains)
		m_guiLabels.push_back(chain.GetTag());
}

EdgeChainManager::~EdgeChainManager()
{}

void EdgeChainManager::CheckChainClicked(sf::RenderWindow& window)
{
	for (int i = 0; i < m_chains.size(); ++i)
	{
		auto& chain = m_chains[i];
		sf::FloatRect r = chain.GetMoveHandleLabelRect();
		sf::Vector2f  m = GetMousePosition(window);

		if ((m.x > r.left && m.x < r.left + r.width) &&
			(m.y > r.top && m.y < r.top + r.height))
		{
			if (m_currSelectedIndex != i)
			{
				m_currSelectedIndex = i;
				SelectCurrentChain();
			}

			break;
		}
	}
}

/* Select chain at m_currSelectionIndex */
void EdgeChainManager::SelectCurrentChain()
{
	m_chains[m_prevSelectedIndex].SetEditable(false);
	m_chains[m_prevSelectedIndex].DrawBoundingBox(false);

	m_chains[m_currSelectedIndex].SetEditable(true);
	m_chains[m_currSelectedIndex].DrawBoundingBox(true);
	m_prevSelectedIndex = m_currSelectedIndex;
}

/* Create a new StaticEdgeChain object */
void EdgeChainManager::PushChain(const Vector2f& startPos)
{
	// Generate a unique label for the new chain
	unsigned int n = m_chains.size() + 1;
	std::string tag = "EC" + std::to_string(n);
	while (std::find(m_guiLabels.begin(), m_guiLabels.end(), tag) != m_guiLabels.end())
	{
		tag = tag.substr(0,2) + std::to_string(++n);
	}

	m_chains.push_back(StaticEdgeChain(
		demo_data::newChainCoords, startPos, tag, m_world, this));

	m_guiLabels.push_back(tag);
	++m_edgeChainCount;
	m_edgeChainVertexCount += demo_data::newChainCoords.size();

	m_currSelectedIndex = m_chains.size()-1;
	SelectCurrentChain();
}

void EdgeChainManager::PopChain()
{
	if (m_chains.size() > 0)
	{
		// Remove label
		std::string tag = m_chains[m_currSelectedIndex].GetTag();
		m_guiLabels.erase(find(m_guiLabels.begin(), m_guiLabels.end(), tag));

		// Remove chain b2Body and remove chain from m_chains
		auto chain = m_chains.begin() + m_currSelectedIndex;

		// Update vertex count before erasing chain
		m_edgeChainVertexCount -= chain->GetVertexCount();
		--m_edgeChainCount;

		chain->SetEditable(false);
		chain->DeleteBody(m_world);
		m_chains.erase(chain);

		// Update indexes
		if (m_chains.size() > 0)
		{
			m_currSelectedIndex = m_chains.size() - 1;
			m_chains[m_currSelectedIndex].SetEditable(true);
			m_prevSelectedIndex = m_currSelectedIndex;
		}
		else
		{
			// change to -1 to signify no chain selected
			m_currSelectedIndex = 0;
		}
	}
}

void EdgeChainManager::AddVertexToSelectedChain()
{
	if (m_chains.size() > 0)
		m_chains[m_currSelectedIndex].SetAddVertexFlag(true);
}

void EdgeChainManager::RemoveVertexFromSelectedChain()
{
	if (m_chains.size() > 0)
		m_chains[m_currSelectedIndex].SetRemoveVertexFlag(true);
}

void EdgeChainManager::HandleInput(const sf::Event& event, sf::RenderWindow& window)
{
	for (auto& chain : m_chains)
		chain.HandleInput(event, window);
}

void EdgeChainManager::Update(sf::RenderWindow& window)
{
	for (auto& chain : m_chains)
		chain.Update(window, m_world);
}

void EdgeChainManager::Draw(sf::RenderWindow& window)
{
	for (auto& chain : m_chains)
	{
		if (chain.IsEditable())
		{
			chain.DrawBoundingBox(m_guiDrawBB);
		}

		chain.Draw(window);
	}
}

// ----------------------------------------------------------------------
// Accessors
// ----------------------------------------------------------------------

int EdgeChainManager::GetChainCount() const
{
	return m_chains.size();
}

int& EdgeChainManager::GetSelectedChainIndex()
{
	return m_currSelectedIndex;
}

std::vector<std::string>& EdgeChainManager::GetChainLabels()
{
	return m_guiLabels;
}

bool* EdgeChainManager::GetEnableFlag()
{
	return &m_guiEnable;
}

void EdgeChainManager::SyncEnable()
{
	for (auto& chain : m_chains)
	{
		chain.SetEnabled(m_guiEnable);
	}
}

void EdgeChainManager::ToggleEnable()
{
	m_guiEnable = !m_guiEnable;
}

bool* EdgeChainManager::GetDrawBBFlag()
{
	return &m_guiDrawBB;
}



// ----------------------------------------------------------------------
// ChainManagerController Interface Implementaiton
// ----------------------------------------------------------------------

void EdgeChainManager::IncrementEdgeChainCount(unsigned long n)
{
	m_edgeChainCount += n;
}

unsigned long* EdgeChainManager::GetEdgeChainCount()
{
	return &m_edgeChainCount;
}

void EdgeChainManager::IncrementEdgeChainVertexCount(unsigned long n)
{
	m_edgeChainVertexCount += n;
}

unsigned long* EdgeChainManager::GetEdgeChainVertexCount()
{
	return &m_edgeChainVertexCount;
}

const StaticEdgeChain& EdgeChainManager::GetSelectedEdgeChain() const {
	return m_chains[m_currSelectedIndex];
}