#ifndef DRAG_CACHE_MANAGER_HPP
#define DRAG_CACHE_MANAGER_HPP

#include "editor/callbacks/trigger_zone.hpp"
#include "editor/debug/demo_distance_joint.hpp"
#include "editor/chains/static_edge_chain.hpp"


/** DragCacheManager
 * Simple class to update the cached object that is being dragged.
 */

class DragCacheManager
{
public:
	DragCacheManager()
	{}

	DragCacheManager(const DragCacheManager&) = delete;
	DragCacheManager& operator= (const DragCacheManager&) = delete;

	static void UpdateCache()
	{
		Draggable* dragCache = Draggable::GetObjectBeingDragged();

		if (TriggerZone* zone = dynamic_cast<TriggerZone*>(dragCache))
		{
			zone->UpdateDragCache();
		}
		else if (DemoDistanceJoint* demo = dynamic_cast<DemoDistanceJoint*>(dragCache))
		{
			demo->UpdateDragCache();
		}
		else if (StaticEdgeChain* chain = dynamic_cast<StaticEdgeChain*>(dragCache))
		{
			chain->UpdateDragCache();
		}
	}

};

#endif