#include <sc2api/sc2_api.h>

#include <stdlib.h>

#include <iostream>

#include <unordered_map>

#include <utility>
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
	int pos_x;
	int pos_y;
	float influence[3];         // 0 - ground influence, 1 - air influence, 2 - general influence (could use an enum)
};

std::unordered_map<Tag, unit_t> umap;

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
			//PropigateInfluence(floatVector &general_IM, boolVector map, float momentumRate, float decayRate)
			boolVector boolMap = CreateMap();
			GameInfo info = Observation()->GetGameInfo();
			floatVector general_IM = floatVector(info.width, std::vector<float>(info.height, 0.0));

			PropigateInfluence(general_IM, boolMap, 0.12, 0.3);

			for (int i = 0; i < info.width; ++i) {
				for (int j = 0; j < info.height; ++j) {
					std::cout << general_IM[i][j];
				}
				std::cout << std::endl;
			}

			timer = 0;

			/* std::unordered_map<Tag, unit_t>::iterator itr;
			std::cout << "\nAll Elements : \n";
			for (itr = umap.begin(); itr != umap.end(); itr++)
			{
			std::cout << itr->first << "  "  << std::endl;
			}*/
		}
		timer++;
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
	void UpdateVisableUnits() {
		Units units = Observation()->GetUnits(Unit::Alliance::Self);
		unit_t newUnit;
		// Cycle through units
		for (const auto& unit : units) {
			// maybe check is alive?
			// check hash table for matching tag
			// if no matching tag then populate unit structure and add to hash table

			std::cout << unit->tag << "not found\n\n";
			newUnit.tag = unit->tag;
			newUnit.type = unit->unit_type;
			newUnit.alliance = unit->alliance;
			newUnit.health = unit->health;
			newUnit.health_max = unit->health_max;
			newUnit.shield = unit->shield;
			newUnit.shield_max = unit->shield_max;
			newUnit.is_alive = unit->is_alive;
			newUnit.last_seen = unit->last_seen_game_loop;
			newUnit.pos_x = unit->pos.x;
			newUnit.pos_y = unit->pos.y;
			ComputeInfluence(newUnit);

			std::unordered_map<Tag, unit_t>::const_iterator element = umap.find(unit->tag);
			if (element == umap.end()) {
				// add to hash table
				umap.insert(std::make_pair(newUnit.tag, newUnit));

			}
			// else repopulate with updated values
			else {
				std::cout << unit->tag << "found!!\n\n";
				if (element->second.type != newUnit.type) {
					umap.erase(unit->tag);
					umap.insert(std::make_pair(newUnit.tag, newUnit));
				}
				else if (element->second.alliance != newUnit.alliance) {
					umap.erase(unit->tag);
					umap.insert(std::make_pair(newUnit.tag, newUnit));
				}
				else if (element->second.health != newUnit.health) {
					umap.erase(unit->tag);
					umap.insert(std::make_pair(newUnit.tag, newUnit));
				}
				else if (element->second.health_max != newUnit.health_max) {
					umap.erase(unit->tag);
					umap.insert(std::make_pair(newUnit.tag, newUnit));
				}
				else if (element->second.shield != newUnit.shield) {
					umap.erase(unit->tag);
					umap.insert(std::make_pair(newUnit.tag, newUnit));
				}
				else if (element->second.shield_max != newUnit.shield_max) {
					umap.erase(unit->tag);
					umap.insert(std::make_pair(newUnit.tag, newUnit));
				}
				else if (element->second.is_alive != newUnit.is_alive) {
					umap.erase(unit->tag);
					umap.insert(std::make_pair(newUnit.tag, newUnit));
				}
				else if (element->second.last_seen != newUnit.last_seen) {
					umap.erase(unit->tag);
					umap.insert(std::make_pair(newUnit.tag, newUnit));
				}
				else if (element->second.pos_x != newUnit.pos_x) {
					umap.erase(unit->tag);
					umap.insert(std::make_pair(newUnit.tag, newUnit));
				}
				else if (element->second.pos_y != newUnit.pos_y) {
					umap.erase(unit->tag);
					umap.insert(std::make_pair(newUnit.tag, newUnit));
				}
			}



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
				unit.influence[ground] = 1.5;
				unit.influence[air] = 1.5;
				unit.influence[general] = 3;
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

		UpdateVisableUnits();

		for (unsigned i = 0; i < umap.bucket_count(); ++i) {
			for (auto local_it = umap.begin(i); local_it != umap.end(i); ++local_it) {
				if (general_IM[local_it->second.pos_x][local_it->second.pos_y] <= 5 || general_IM[local_it->second.pos_x][local_it->second.pos_y] >= -5) {
					general_IM[local_it->second.pos_x][local_it->second.pos_y] += local_it->second.influence[general];
					//PropigationMomentum(general_IM, momentumRate);
					PropigationDecay(general_IM, decayRate, local_it->second.pos_x, local_it->second.pos_y);
				}

			}
		}
	}





	// Used by PropigateInfluence() to affect rate of propigation
	void PropigationMomentum(floatVector &influenceMap, float rate) {
		// Use a parameter to define a momentum rate from 0 to 1.0
		// This function effects how long a particular "node" on the graph will retain influence from a unit that has moved away from that node

	}




	// Used by PropigateInfluence() to affect how far will a unit's influence spread
	void PropigationDecay(floatVector &influenceMap, const float &rate, const int &x, const int &y) {
		// Diagonals are minus 0.18
		// Left/Right Up/Down are minus 0.12
		GameInfo info = Observation()->GetGameInfo();
		float value = influenceMap[x][y];
		int i = 1;

		if (value <= 5 || value >= -5) {
			while (value - (rate + 0.06) >= 0) {
				//std::cout << "Value: " << value << std::endl;
				//std::cout << "Value - rate: " << value - rate << std::endl;
				//std::cout << "Value - (rate + 0.06): " << value - (rate + 0.06) << std::endl;
				std::cout << "i: " << i << std::endl;
				std::cout << "x + i: " << x + i << std::endl;
				std::cout << "x - i: " << x - i << std::endl;
				std::cout << "y + i: " << y + i << std::endl;
				std::cout << "y - i: " << y - i << std::endl;
				std::cout << std::endl;
				std::cout << std::endl;
				std::cout << std::endl;


				if ((x + i < info.width) && (x - i > 0) && (y + i < info.height) && (y - i > 0)) {
					// Up/Down Left/Right
					influenceMap[x + i][y] = round(value - rate);
					influenceMap[x - i][y] = round(value - rate);
					influenceMap[x][y + i] = round(value - rate);
					influenceMap[x][y - i] = round(value - rate);

					// Diagonals
					influenceMap[x + i][y - i] = round(value - (rate + 0.06));
					influenceMap[x + i][y + i] = round(value - (rate + 0.06));
					influenceMap[x - i][y - i] = round(value - (rate + 0.06));
					influenceMap[x - i][y + i] = round(value - (rate + 0.06));

					for (int _x = x - i; _x <= x + i; _x++) {
						for (int _y = y - i; _y <= y + i; _y++) {
							if (influenceMap[_x][_y] == 0) {

								influenceMap[_x][_y] = influenceMap[_x + 1][_y] +
									influenceMap[_x - 1][_y] +
									influenceMap[_x][_y + 1] +
									influenceMap[_x][_y - 1] +
									influenceMap[_x + 1][_y - 1] +
									influenceMap[_x + 1][_y + 1] +
									influenceMap[_x - 1][_y - 1] +
									influenceMap[_x - 1][_y + 1];
								influenceMap[_x][_y] /= 8;
								influenceMap[_x][_y] = round(influenceMap[_x][_y]);
							}
						}

					}

					value -= (rate + 0.06);
					i++;

				}
				else {
					break;
				}
			}
		}
	}

	void DrawInfluenceLine(floatVector &influenceMap, const float &rate, int x_begin, int y_begin, int x_end, int y_end) {
		float value = influenceMap[x_begin][y_begin];
		GameInfo info = Observation()->GetGameInfo();
		int i = 1;
		std::cout << "x_begin => x_end: " << x_begin << " => " << x_end << std::endl;
		std::cout << "y_begin => y_end: " << y_begin << " => " << y_end << std::endl;
		if (x_begin != x_end) {
			for (int x = x_begin; x < x_end; x++) {
				for (int y = y_begin; y < y_end; y++) {
					while (value - (rate + 0.06) >= 0) {
						if ((x + i <= info.width) && (x - i >= 0) && (y_begin + i <= info.height) && (y_begin - i >= 0)) {
							// Up/Down Left/Right
							influenceMap[x + i][y] = 1;
							influenceMap[x - i][y] = 1;
							influenceMap[x][y + i] = 1;
							influenceMap[x][y - i] = 1;

							// Diagonals
							influenceMap[x + i][y - i] = 1;
							influenceMap[x + i][y + i] = 1;
							influenceMap[x - i][y - i] = 1;
							influenceMap[x - i][y + i] = 1;

							value -= (rate + 0.06);
							i++;
						}
						else {
							break;
						}
					}
				}

			}
		}
	}

	//if (y_begin != y_end) {
	//    for (int y = y_begin; y < y_end; y++) {
	//        influenceMap[x_begin][y] = 1;
	//    }
	//}




	//for (int i = x_begin; i <= x_end; i++) {
	//    for (int j = y_begin; j <= y_end; j++) {
	//        influenceMap[i][j] = k;
	//        //k++;
	//        //if (value - rate >= 0) {
	//            //influenceMap[i][j] = value - rate;
	//        //}
	//        //else {
	//            //break;
	//        //}
	//        
	//    }
	//    
	//}



	void FindUnitByTag(const Tag &tag, const Units &units, int &x, int &y) {
		for (const auto& unit : units) {
			if (unit->tag == tag) {
				x = unit->pos.x;
				y = unit->pos.y;
			}
		}



	}

	float round(float number) {
		float value = (int)(number * 100 + 0.5);
		return (float)value / 100;

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