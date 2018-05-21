#include <sc2api/sc2_api.h>
#include <iostream>
using namespace sc2;
typedef std::vector<std::vector<bool>> boolVector;
/*
* TODO:
* [x] Add setting influence function
* [] Add propagation algorithm (need idea of representation type (2D grid, Area Graphs, etc))
* [] Add momentum function (affects rate of propigation)
* [] Add decay function (how far will a unit's influence spread?)
* [] Add update frequency (right now it simply updates on step, but this is too often)
* [] Add visualization
*/
// Basic unit structure to be added to hash table
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
    //std::cout << TotalUnitCount() << std::endl;
    //std::cout << UnitTypeCount(UNIT_TYPEID::TERRAN_SCV) << std::endl;
    //UpdateVisableUnits();
    boolVector test = CreateMap();
    GameInfo info = Observation()->GetGameInfo();
    for (int i = 0; i < info.width; ++i) {
      for (int j = 0; j < info.height; ++j) {
        std::cout << test[i][j];
      }
      std::cout << std::endl;
    }
  }
private:
  /*
  * NOTE:
  * THE FUNCTIONS DEFINED BY THIS PRIVATE SECTION SHOULD EVENTUALLY BE ABSTRACTED TO BE USED OUTSIDE THIS PROJECT
  */
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
  void UpdateVisableUnits() {
    Units units = Observation()->GetUnits(Unit::Alliance::Self);
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
        unit.influence[0] = (unit.health + unit.shield) / 100;
        unit.influence[1] = 0;
        unit.influence[2] = unit.influence[0] + unit.influence[1];
        break;
      case UNIT_TYPEID::TERRAN_MARINE:
        unit.influence[0] = unit.influence[1] = (unit.health + unit.shield + (6 / 0.86) * 5) / 100;
        unit.influence[2] = unit.influence[0] + unit.influence[1];
        break;
      case UNIT_TYPEID::TERRAN_REAPER:
        unit.influence[0] = (unit.health + unit.shield + (4 / 1.1) * 5) / 100;
        unit.influence[1] = 0;
        unit.influence[2] = unit.influence[0] + unit.influence[1];
        break;
      case UNIT_TYPEID::TERRAN_MARAUDER:
        unit.influence[0] = (unit.health + unit.shield + (15 / 1.5) * 6) / 100;
        unit.influence[1] = 0;
        unit.influence[2] = unit.influence[0] + unit.influence[1];
        break;
      case UNIT_TYPEID::TERRAN_GHOST:
        unit.influence[0] = unit.influence[1] = (unit.health + unit.shield + (15 / 1.5) * 6) / 100;
        unit.influence[2] = unit.influence[0] + unit.influence[1];
        break;
      case UNIT_TYPEID::TERRAN_HELLION:
        unit.influence[0] = (unit.health + unit.shield + (13 / 2.5) * 5) / 100;
        unit.influence[1] = 0;
        unit.influence[2] = unit.influence[0] + unit.influence[1];
        break;
      case UNIT_TYPEID::TERRAN_WIDOWMINE:
        unit.influence[0] = unit.influence[1] = (unit.health + unit.shield + (125 / 40) * 5) / 100;
        unit.influence[2] = unit.influence[0] + unit.influence[1];
        break;
      case UNIT_TYPEID::TERRAN_SIEGETANK:
        unit.influence[0] = unit.influence[1] = (unit.health + unit.shield + (20 / 1.04) * 7) / 100;
        unit.influence[2] = unit.influence[0] + unit.influence[1];
        break;
      case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:
        unit.influence[0] = unit.influence[1] = (unit.health + unit.shield + (42 / 3) * 13) / 100;
        unit.influence[2] = unit.influence[0] + unit.influence[1];
        break;
      case UNIT_TYPEID::TERRAN_THOR:
        unit.influence[0] = (unit.health + unit.shield + (30 / 1.28) * 7) / 100;
        unit.influence[1] = (unit.health + unit.shield + (10 / 2) * 10) / 100;
        unit.influence[2] = unit.influence[0] + unit.influence[1];
        break;
      case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
        unit.influence[0] = unit.influence[1] = (unit.health + unit.shield + (10 / 2) * 9) / 100;
        unit.influence[2] = unit.influence[0] + unit.influence[1];
        break;
      case UNIT_TYPEID::TERRAN_VIKINGASSAULT:
        unit.influence[0] = (unit.health + unit.shield + (12 / 1) * 6) / 100;
        unit.influence[1] = 0;
        unit.influence[2] = unit.influence[0] + unit.influence[1];
        break;
      case UNIT_TYPEID::TERRAN_MEDIVAC:
        unit.influence[0] = (unit.health + unit.shield) / 100;
        unit.influence[1] = (unit.health + unit.shield) / 100;
        unit.influence[2] = unit.influence[0] + unit.influence[1];
        break;
      case UNIT_TYPEID::TERRAN_RAVEN:
        unit.influence[0] = (unit.health + unit.shield) / 100;
        unit.influence[1] = (unit.health + unit.shield) / 100;
        unit.influence[2] = unit.influence[0] + unit.influence[1];
        break;
      case UNIT_TYPEID::TERRAN_BANSHEE:
        unit.influence[0] = unit.influence[1] = (unit.health + unit.shield + (12 / 1.25) * 6) / 100;
        unit.influence[2] = unit.influence[0] + unit.influence[1];
        break;
      case UNIT_TYPEID::TERRAN_BATTLECRUISER:
        unit.influence[0] = (unit.health + unit.shield + (8 / 0.23) * 6) / 100;
        unit.influence[1] = (unit.health + unit.shield + (6 / 0.23) * 6) / 100;
        unit.influence[2] = unit.influence[0] + unit.influence[1];
        break;
      default:
        unit.influence[0] = (unit.health + unit.shield + (10 / 0.7) * 5) / 100;
        unit.influence[1] = (unit.health + unit.shield + (5 / 0.7) * 5) / 100;
        unit.influence[2] = unit.influence[0] + unit.influence[1];
        break;
      }
      // Enemy influence is negative
      if (unit.alliance == 4) {
        unit.influence[0] *= -1;
        unit.influence[1] *= -1;
        unit.influence[2] *= -1;
      }
    }
    else {
      // If unit is dead influence is 0
      unit.influence[0] = unit.influence[1] = unit.influence[2] = 0;
    }
  }
  // Largely taken from CommandCenter's boolean grid buildable map info
  boolVector CreateMap() {
    GameInfo info = Observation()->GetGameInfo();
    boolVector walkable = boolVector(info.width, std::vector<bool>(info.height, true));
    for (int i = 0; i < info.width; ++i) {
      for (int j = 0; j < info.height; ++j) {
        Point2DI point(i, j);
        if (point.x < 0 || point.x >= info.width || point.y < 0 || point.y >= info.width) {
          walkable[i][j] = false;
        }
        //assert(info.pathing_grid.data.size()) == info.width * info.height);
        unsigned char encodedPlacement = info.pathing_grid.data[point.x + ((info.height - 1) - point.y) * info.width];
        bool decodedPlacement = encodedPlacement == 255 ? false : true;
        walkable[i][j] = decodedPlacement;
      }
    }
    return walkable;
  }
  // Use individual unit influence to propogate map influence
  void PropigateInfluence() {
    // Each node of the graph/grid will take on the value of the influence exhibited by the units nearby
    // For example, if two SCVs are on/near a node then the node will update to reflect the combined influence of both SCVs
    // To do this we need to:
    // For each node on the graph
    // Check to see if there is one or more units on it
    // If so
  }
  // Used by PropigateInfluence() to affect rate of propigation
  void PropigationMomentum() {
    // Use a parameter to define a momentum rate from 0 to 1.0
    // This function effects how long a particular "node" on the graph will retain influence from a unit that has 
  }
  // Used by PropigateInfluence() to affect how far will a unit's influence spread
  void PropigationDecay() {
  }
  // Used to call PropigateInfluence() at a specified frequency
  void UpdatePropigation() {
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