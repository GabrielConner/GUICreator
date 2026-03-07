#include "private/dataTypes.h"

#include "pPack/vector.h"

#include <string>
#include <vector>
#include <map>
#include <numeric>


using namespace ::pPack;
using namespace ::pPack::vector;
using namespace ::pPack::gui_creator_private;


namespace pPack {
namespace gui_creator_private {

	std::pair<Vector2, Vector2> GUINode::GenTransformFor(GUINode* obj) {
		if (obj->noGrid) {
			return { Vector2(), Vector2(1) };
		}

		gridPosition.x += obj->jumpGrid.x;
		gridPosition.y += obj->jumpGrid.y;

		if (gridPosition.x >= horizontalGrid.size() || gridPosition.y >= verticalGrid.size())
			return { Vector2(), Vector2(1) };

		int gridMoveX = std::accumulate(horizontalGrid.begin(), horizontalGrid.begin() + gridPosition.x, 0);
		int gridMoveY = std::accumulate(verticalGrid.begin(), verticalGrid.begin() + gridPosition.y, 0);

		Vector2 pos = Vector2();
		Vector2 scale = Vector2();

		pos.x = (((float)horizontalGrid[gridPosition.x] / 2) + gridMoveX) / (gridSize.x) * 2 - 1;
		pos.y = (((float)verticalGrid[gridPosition.y] / 2) + gridMoveY) / (gridSize.y) * 2 - 1;

		pos.x *= 1 - padding.x;
		pos.y *= 1 - padding.y;

		pos.y *= -1;

		scale.x = (float)horizontalGrid[gridPosition.x] / (gridSize.x + padding.x);
		scale.y = (float)verticalGrid[gridPosition.y] / (gridSize.y + padding.y);

		if (obj->noMove)
			return { pos, scale };

		gridPosition.x++;
		if (gridPosition.x >= horizontalGrid.size()) {
			gridPosition.x = 0;
			gridPosition.y++;
		}

		return { pos, scale };
	}



	GUINode::GUINode() : horizontalGrid(), verticalGrid(), gridPosition(),
		gridSize(3), position(), scale(1), padding(0), jumpGrid(0), noMove(false), noGrid(false), type(),
		data(), attributes(), children(), parent(nullptr) {


		horizontalGrid = { 1,1,1 };
		verticalGrid = { 1,1,1 };
	}


}; // namespace gui_creator_private
}; // namespace pPack