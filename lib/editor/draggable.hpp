#ifndef DRAGGABLE_HPP
#define DRAGGABLE_HPP

/** Draggable Interface
 *
 * Every draggable object in the editor will inherit this class and implement
 * the virtual UpdateDragCache() function. The cached draggable object is
 * checked every frame where this function is called on that object. Within the
 * function, the cache is cleared if the object is no longer being dragged.
 */

class Draggable
{
protected:
	static Draggable* s_ObjectBeingDragged;

public:
	virtual void UpdateDragCache() = 0;

	static bool ObjectBeingDragged()
	{
		if (s_ObjectBeingDragged == nullptr)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	static Draggable* GetObjectBeingDragged()
	{
		return s_ObjectBeingDragged;
	}
};

#endif