#include "game_structures.h"

Position get_center(Position const& pos, Size const& Size)
{
	return Position{ pos.x + Size.width / 2, pos.y + Size.height / 2 };
}
