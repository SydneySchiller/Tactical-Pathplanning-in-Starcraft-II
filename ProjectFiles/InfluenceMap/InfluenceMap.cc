#include <sc2api/sc2_api.h>

#include <stdlib.h>

#include <iostream>

using namespace sc2;


/*
* TODO:
* [x] Add setting influence function
* [] Add propagation algorithm (need idea of representation type (2D grid, Area Graphs, etc))
* [] Add momentum function (affects rate of propigation)
* [] Add decay function (how far will a unit's influence spread?)
* [] Add update frequency (right now it simply updates on step, but this is too often)
* [] Add visualization
* [] Abstract functions to a non sc2 api dependent format
* [] Add A*
* [] Use influence map to alter cost value of A* transitions
*/
// Basic unit structure to be added to hash table

typedef std::vector<std::vector<bool>> boolVector;
typedef std::vector<std::vector<float>> floatVector;

enum InfluenceType { ground = 0, air = 1, general = 3 };

int timer = 0;


struct unit_t {
	Tag tag;
	UNIT_TYPEID type;
	int alliance;
	float health;
	float health_max;
	float shield;
	float shield_max;
	bool is_alive;
	uint32_t last_seen;         // the last time this unit was seen
	float influence[3];         // 0 - ground influence, 1 - air influence, 2 - general influence (could use an enum)
};
class Bot : public Agent {
public:

	// Builds terran SCV when idle 
	virtual void OnUnitIdle(const Unit* unit) final {
		switch (unit->unit_type.ToType()) {
		case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
			break;
		}
		default: {
			break;
		}
		}

	}
	virtual void OnStep() {

		// Update every 100 steps, this will eventually be replaced by a function call from main
		if (timer == 100) {
			boolVector map = CreateMap();
			PropigateInfluence(map, 10, 10);
			timer = 0;
		}
		timer++;



		////std::cout << TotalUnitCount() << std::endl;
		////std::cout << UnitTypeCount(UNIT_TYPEID::TERRAN_SCV) << std::endl;
		////UpdateVisableUnits();
		//std::string testOutput;
		//boolVector test = CreateMap();
		//GameInfo info = Observation()->GetGameInfo();
		//for (int i = 0; i < info.width; ++i) {
		//    for (int j = 0; j < info.height; ++j) {
		//        //testOutput += to_string(test[i][j]);
		//    }
		//    //testOutput += "\n";
		//}

		////std::cout << testOutput;




		//floatVector temp = PropigateInfluence(test);
		//for (int i = 0; i < info.width; ++i) {
		//    for (int j = 0; j < info.height; ++j) {
		//        if (test[i][j] == true) {
		//            std::cout << temp[i][j];
		//        }
		//        else
		//            std::cout << test[i][j];
		//        
		//    }
		//    std::cout << std::endl;
		//}
	}

	// Used to call PropigateInfluence() at a specified frequency
	// Will be called from main to update propigation
	void UpdatePropigation() {
		//PropigateInfluence(CreateMap(), 10, 10);
	}
private:
	// Returns total number of ally units
	int TotalUnitCount() {
		int numUnits = 0;
		Units units = Observation()->GetUnits(Unit::Alliance::Self);
		// Cycle through ally units and count them all
		for (const auto& unit : units) {
			++numUnits;
		}
		return numUnits; // Currently an off by one error
	}




	// Updates unit data structure with visable units
	// Should probably be a public function
	void UpdateVisableUnits(const Units &units) {
		unit_t newUnit;
		// Cycle through units
		for (const auto& unit : units) {
			// maybe check is alive?
			// check hash table for matching tag
			// if no matching tag then populate unit structure and add to hash table
			// else do nothing
			newUnit.tag = unit->tag;
			newUnit.type = unit->unit_type;
			newUnit.alliance = unit->alliance;
			newUnit.health = unit->health;
			newUnit.health_max = unit->health_max;
			newUnit.shield = unit->shield;
			newUnit.shield_max = unit->shield_max;
			newUnit.is_alive = unit->is_alive;
			newUnit.last_seen = unit->last_seen_game_loop;
			ComputeInfluence(newUnit);
			// add to hash table
		}
	}




	// Returns count of a particular type of unit
	size_t UnitTypeCount(UNIT_TYPEID unit_type) {
		return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
	}




	// Calculate influence of an individual unit
	void ComputeInfluence(unit_t& unit) {
		if (unit.is_alive) {
			// Unit type TERRAN ONLY
			// influence: (health + shields + (damage/attack speed) * range)/100
			// influence types: 0-ground, 1-air, 2-general
			switch (unit.type) {
			case UNIT_TYPEID::TERRAN_SCV:
				unit.influence[ground] = (unit.health + unit.shield) / 100;
				unit.influence[air] = 0;
				unit.influence[general] = unit.influence[ground] + unit.influence[air];
				break;
			case UNIT_TYPEID::TERRAN_MARINE:
				unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield + (6 / 0.86) * 5) / 100;
				unit.influence[general] = unit.influence[ground] + unit.influence[air];
				break;
			case UNIT_TYPEID::TERRAN_REAPER:
				unit.influence[ground] = (unit.health + unit.shield + (4 / 1.1) * 5) / 100;
				unit.influence[air] = 0;
				unit.influence[general] = unit.influence[ground] + unit.influence[air];
				break;
			case UNIT_TYPEID::TERRAN_MARAUDER:
				unit.influence[ground] = (unit.health + unit.shield + (15 / 1.5) * 6) / 100;
				unit.influence[air] = 0;
				unit.influence[general] = unit.influence[ground] + unit.influence[air];
				break;
			case UNIT_TYPEID::TERRAN_GHOST:
				unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield + (15 / 1.5) * 6) / 100;
				unit.influence[general] = unit.influence[ground] + unit.influence[air];
				break;
			case UNIT_TYPEID::TERRAN_HELLION:
				unit.influence[ground] = (unit.health + unit.shield + (13 / 2.5) * 5) / 100;
				unit.influence[air] = 0;
				unit.influence[general] = unit.influence[ground] + unit.influence[air];
				break;
			case UNIT_TYPEID::TERRAN_WIDOWMINE:
				unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield + (125 / 40) * 5) / 100;
				unit.influence[general] = unit.influence[ground] + unit.influence[air];
				break;
			case UNIT_TYPEID::TERRAN_SIEGETANK:
				unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield + (20 / 1.04) * 7) / 100;
				unit.influence[general] = unit.influence[ground] + unit.influence[air];
				break;
			case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
				unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield + (42 / 3) * 13) / 100;
				unit.influence[general] = unit.influence[ground] + unit.influence[air];
				break;
			case UNIT_TYPEID::TERRAN_THOR:
				unit.influence[ground] = (unit.health + unit.shield + (30 / 1.28) * 7) / 100;
				unit.influence[air] = (unit.health + unit.shield + (10 / 2) * 10) / 100;
				unit.influence[general] = unit.influence[ground] + unit.influence[air];
				break;
			case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
				unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield + (10 / 2) * 9) / 100;
				unit.influence[general] = unit.influence[ground] + unit.influence[air];
				break;
			case UNIT_TYPEID::TERRAN_VIKINGASSAULT:
				unit.influence[ground] = (unit.health + unit.shield + (12 / 1) * 6) / 100;
				unit.influence[air] = 0;
				unit.influence[general] = unit.influence[ground] + unit.influence[air];
				break;
			case UNIT_TYPEID::TERRAN_MEDIVAC:
				unit.influence[ground] = (unit.health + unit.shield) / 100;
				unit.influence[air] = (unit.health + unit.shield) / 100;
				unit.influence[general] = unit.influence[ground] + unit.influence[air];
				break;
			case UNIT_TYPEID::TERRAN_RAVEN:
				unit.influence[ground] = (unit.health + unit.shield) / 100;
				unit.influence[air] = (unit.health + unit.shield) / 100;
				unit.influence[general] = unit.influence[ground] + unit.influence[air];
				break;
			case UNIT_TYPEID::TERRAN_BANSHEE:
				unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield + (12 / 1.25) * 6) / 100;
				unit.influence[general] = unit.influence[ground] + unit.influence[air];
				break;
			case UNIT_TYPEID::TERRAN_BATTLECRUISER:
				unit.influence[ground] = (unit.health + unit.shield + (8 / 0.23) * 6) / 100;
				unit.influence[air] = (unit.health + unit.shield + (6 / 0.23) * 6) / 100;
				unit.influence[general] = unit.influence[ground] + unit.influence[air];
				break;
			default:
				unit.influence[ground] = (unit.health + unit.shield + (10 / 0.7) * 5) / 100;
				unit.influence[air] = (unit.health + unit.shield + (5 / 0.7) * 5) / 100;
				unit.influence[general] = unit.influence[ground] + unit.influence[air];
				break;
			}
			// Enemy influence is negative
			if (unit.alliance == 4) {
				unit.influence[ground] *= -1;
				unit.influence[air] *= -1;
				unit.influence[general] *= -1;
			}
		}
		else {
			// If unit is dead influence is 0
			unit.influence[ground] = unit.influence[air] = unit.influence[general] = 0;
		}
	}




	// Largely taken from CommandCenter's boolean grid buildable map info
	// Should probably be a public function
	boolVector CreateMap() {
		GameInfo info = Observation()->GetGameInfo();
		boolVector walkable = boolVector(info.width, std::vector<bool>(info.height, true));
		//Units units = Observation()->GetUnits(Unit::Alliance::Self);

		for (int i = 0; i < info.width; ++i) {
			for (int j = 0; j < info.height; ++j) {
				Point2DI point(i, j);
				if (point.x < 0 || point.x >= info.width || point.y < 0 || point.y >= info.width) {
					walkable[i][j] = false;
				}

				// Cycle through ally units
				//for (const auto& unit : units) {
				//if (point.x == unit->pos.x) {
				// std::cout << "WOOOOO!" << std::endl;
				//}
				//std::cout << "point.x: " << point.x << std::endl;
				//std::cout << "unit->pos.x: " << unit->pos.x << std::endl;
				//}

				//assert(info.pathing_grid.data.size()) == info.width * info.height);
				unsigned char encodedPlacement = info.pathing_grid.data[point.x + ((info.height - 1) - point.y) * info.width];
				bool decodedPlacement = encodedPlacement == 255 ? false : true;
				walkable[i][j] = decodedPlacement;
			}
		}
		return walkable;
	}




	// Use individual unit influence to propogate map influence
	// Pass in unit hash too, unless it's a global
	// boolVector is used to ensure influence doesn't spread onto non-walkable areas

	void PropigateInfluence(floatVector &general_IM, boolVector map, float momentumRate, float decayRate) {

		Units units = Observation()->GetUnits(Unit::Alliance::Self);

		//for (const auto& unit : units) {
		//general_IM[unit->pos.x][unit->pos.y] += unit.influence[general];
		//}

		//UpdateVisableUnits(units);

		//foreach element in the hash table
		//general_IM[unit->pos.x][unit->pos.y] += unit.influence[general];













		// Each node of the graph/grid will take on the value of the influence exhibited by the units nearby
		// For example, if two SCVs are on/near a node then the node will update to reflect the combined influence of both SCVs

		GameInfo info = Observation()->GetGameInfo();
		Units units = Observation()->GetUnits(Unit::Alliance::Self);
		//floatVector influenceMap = floatVector(info.width, std::vector<float>(info.height, 1));




	}




	// Used by PropigateInfluence() to affect rate of propigation
	void PropigationMomentum() {
		// Use a parameter to define a momentum rate from 0 to 1.0
		// This function effects how long a particular "node" on the graph will retain influence from a unit that has moved away from that node
	}




	// Used by PropigateInfluence() to affect how far will a unit's influence spread
	void PropigationDecay() {
	}



	void FindUnitByTag(const Tag &tag, const Units &units, int &x, int &y) {
		for (const auto& unit : units) {
			if (unit->tag == tag) {
				x = unit->pos.x;
				y = unit->pos.y;
			}
		}



	}
};
// Basic game creation stuff
int main(int argc, char* argv[]) {
	Coordinator coordinator; coordinator.LoadSettings(argc, argv);
	Bot bot;
	coordinator.SetParticipants({
		CreateParticipant(Race::Terran, &bot),
		CreateComputer(Race::Zerg)
	});
	coordinator.LaunchStarcraft();
	coordinator.StartGame(sc2::kMapBelShirVestigeLE);
	while (coordinator.Update()) {
	}
	return 0;
}