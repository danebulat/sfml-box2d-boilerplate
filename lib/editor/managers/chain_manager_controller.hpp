#ifndef CHAIN_COUNTER_HPP
#define CHAIN_COUNTER_HPP

class ChainManagerController
{
public:
	virtual void IncrementEdgeChainCount(unsigned long n) = 0;
	virtual unsigned long* GetEdgeChainCount() = 0;

	virtual void IncrementEdgeChainVertexCount(unsigned long n) = 0;
	virtual unsigned long* GetEdgeChainVertexCount() = 0;
};

#endif