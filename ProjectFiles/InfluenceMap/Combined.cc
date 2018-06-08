#include <sc2api/sc2_api.h>

#include <stdlib.h>

#include <iostream>

#include <unordered_map>

#include <utility>
using namespace sc2;



/*
 * TODO:
 * [x] Add setting influence function
 * [x] Add propagation algorithm (need idea of representation type (2D grid, Area Graphs, etc))
 * [x] Add momentum function (affects rate of propigation)
 * [x] Add decay function (how far will a unit's influence spread?)
 * [] Add update frequency (right now it simply updates on step, but this is too often)
 * [] Make sure enemy influence is negative and works with enemy + ally interactions
 * [] Global variables == bad, pls fix
 * [] Add visualization
 * [] Abstract functions to a non sc2 api dependent format
 * [] Add A*
 * [] Use influence map to alter cost value of A* transitions
 */
// Basic unit structure to be added to hash table

typedef std::vector<std::vector<bool>> boolVector;
typedef std::vector<std::vector<float>> floatVector;

enum InfluenceType { ground = 0, air = 1, general = 3 };

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

int timer = 0;
std::unordered_map<Tag, unit_t> umap;
floatVector general_IM = floatVector(500, std::vector<float>(500, 0.0));

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
    
    //CONVERT TO LOW/MED/HIGH/NEGATIVE INFLUENCE
    
    //low range
    

    floatVector ConvertIM(floatVector IM, float low_greater, float low_less, float med_greater, float med_less, float high_greater, float high_less){
        GameInfo mapinfo = Observation()->GetGameInfo();
        floatVector new_IM = IM;
        for (int i = 0; i < mapinfo.width; ++i) {
            for (int j = 0; j < mapinfo.height; ++j) {
                
                
                
                if(IM[i][j] >= low_greater && IM[i][j] < low_less){
                    new_IM[i][j] = 1;
                }else if(IM[i][j] > med_greater && IM[i][j] < med_less){
                    new_IM[i][j] = 2;
                }else if(IM[i][j] > high_greater && IM[i][j] < high_less){
                    new_IM[i][j] = 3;
                }else if(IM[i][j] <0){
                    new_IM[i][j] = -1;
                }
                
                
            }
            
        }
        
        return new_IM;
    }
    virtual
    
    
    
    
    
    
    
    
    virtual void OnStep() {
        
        // Update every 100 steps, this will eventually be replaced by a function call from main
        if (timer == 1000) {
            boolVector boolMap = CreateMap();
            GameInfo mapinfo = Observation()->GetGameInfo();
            
            PropigateInfluence(general_IM, boolMap, 0.5, 0.3);
            
            for (int i = 0; i < mapinfo.width; ++i) {
                for (int j = 0; j < mapinfo.height; ++j) {
                    floatVector convIM = ConvertIM(general_IM, 0, 0.2, 0.2, 0.7,0.8, 1);
                    std::cout << convIM[i][j];
                    //std::cout << general_IM[i][j];
                }
                std::cout << std::endl;
            }
            timer = 0;
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;
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
    int TotalAllyUnitCount() {
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
    void UpdateVisableUnits(Units units) {
        
        unit_t newUnit;
        // Cycle through units
        for (const auto& unit : units) {
            // maybe check is alive?
            
            newUnit.tag = unit->tag;
            newUnit.type = unit->unit_type;
            newUnit.alliance = unit->alliance;
            newUnit.health = unit->health;
            newUnit.shield = unit->shield;
            newUnit.is_alive = unit->is_alive;
            newUnit.pos_x = unit->pos.x;
            newUnit.pos_y = unit->pos.y;
            ComputeInfluence(newUnit);
            
            // check hash table for matching tag
            // if no matching tag then populate unit structure and add to hash table
            std::unordered_map<Tag, unit_t>::const_iterator element = umap.find(unit->tag);
            if (element == umap.end()) {
                
                // add to hash table
                umap.insert(std::make_pair(newUnit.tag, newUnit));
                
            }
            // else if found repopulate with updated values if previous
            // values are different from new values
            else {
                
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
                else if (element->second.shield != newUnit.shield) {
                    umap.erase(unit->tag);
                    umap.insert(std::make_pair(newUnit.tag, newUnit));
                }
                else if (element->second.is_alive != newUnit.is_alive) {
                    umap.erase(unit->tag);
                    umap.insert(std::make_pair(newUnit.tag, newUnit));
                }
                // if x and y positions of the unit have moved more than 2 tiles away then update position
                else if (element->second.pos_x != newUnit.pos_x && (newUnit.pos_x >= element->second.pos_x + 2 || newUnit.pos_x >= element->second.pos_x - 2)) {
                    umap.erase(unit->tag);
                    umap.insert(std::make_pair(newUnit.tag, newUnit));
                }
                else if (element->second.pos_y != newUnit.pos_y && (newUnit.pos_y >= element->second.pos_y + 2 || newUnit.pos_y >= element->second.pos_y - 2)) {
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
                    // TERRAN
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
                    // PROTOSS
                case UNIT_TYPEID::PROTOSS_PROBE:
                    unit.influence[ground] = (unit.health + unit.shield) / 100;
                    unit.influence[air] = 0;
                    unit.influence[general] = unit.influence[ground] + unit.influence[air];
                    break;
                case UNIT_TYPEID::PROTOSS_ZEALOT:
                    unit.influence[ground] = (unit.health + unit.shield + (8 / 1.2)) / 100;
                                              unit.influence[air] = 0;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::PROTOSS_STALKER:
                                              unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield + (10 / 1.44) * 6) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::PROTOSS_SENTRY:
                                              unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield + (6 / 1) * 5) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
                                              unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
                                              unit.influence[ground] = (unit.health + unit.shield + (45 / 1.69)) / 100;
                                              unit.influence[air] = 0;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::PROTOSS_ARCHON:
                                              unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield + (25 / 1.75) * 3) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::PROTOSS_PHOENIX:
                                              unit.influence[ground] = 0;
                                              unit.influence[air] = (unit.health + unit.shield + (5 / 1.1) * 5) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::PROTOSS_ORACLE:
                                              unit.influence[ground] = (unit.health + unit.shield + (15 / .86) * 4) / 100;
                                              unit.influence[air] = 0;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::PROTOSS_VOIDRAY:
                                              unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield + (6 / .5) * 6) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::PROTOSS_TEMPEST:
                                              unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield + (30 / 3.3) * 15) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::PROTOSS_CARRIER:
                                              unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield + (5 / 1) * 8) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::PROTOSS_OBSERVER:
                                              unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::PROTOSS_WARPPRISM:
                                              unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::PROTOSS_IMMORTAL:
                                              unit.influence[ground] = (unit.health + unit.shield + (20 / 1.45) * 6) / 100;
                                              unit.influence[air] = 0;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::PROTOSS_COLOSSUS:
                                              unit.influence[ground] = (unit.health + unit.shield + (15 / 1.65) * 6) / 100;
                                              unit.influence[air] = 0;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::PROTOSS_MOTHERSHIPCORE:
                                              unit.influence[ground] = (unit.health + unit.shield + (8 / .85) * 5) / 100;
                                              unit.influence[air] = 0;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
                                              unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield + (6 / 2.21) * 7) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              // ZERG
                                              case UNIT_TYPEID::ZERG_DRONE:
                                              unit.influence[ground] = (unit.health + unit.shield) / 100;
                                              unit.influence[air] = 0;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::ZERG_OVERLORD:
                                              unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::ZERG_ZERGLING:
                                              unit.influence[ground] = (unit.health + unit.shield + (5 / .7)) / 100;
                                              unit.influence[air] = 0;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::ZERG_BANELING:
                                              unit.influence[ground] = (unit.health + unit.shield + (20 / .83)) / 100;
                                              unit.influence[air] = 0;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::ZERG_ROACH:
                                              unit.influence[ground] = (unit.health + unit.shield + (16 / 2) * 4) / 100;
                                              unit.influence[air] = 0;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::ZERG_QUEEN:
                                              unit.influence[ground] = (unit.health + unit.shield + (4 / 1) * 5) / 100;
                                              unit.influence[air] = (unit.health + unit.shield + (9 / 1) * 7) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::ZERG_MUTALISK:
                                              unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield + (9 / 1.52) * 3) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::ZERG_CORRUPTOR:
                                              unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield + (14 / 1.9) * 6) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::ZERG_OVERSEER:
                                              unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::ZERG_HYDRALISK:
                                              unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield + (12 / .83) * 5) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::ZERG_SWARMHOSTMP:
                                              unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::ZERG_LOCUSTMP:
                                              unit.influence[ground] = (unit.health + unit.shield + (12 / .8) * 3) / 100;
                                              unit.influence[air] = 0;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::ZERG_INFESTOR:
                                              unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::ZERG_INFESTORTERRAN:
                                              unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield + (8 / .86) * 5) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::ZERG_ULTRALISK:
                                              unit.influence[ground] = (unit.health + unit.shield + (35 / .86) * 1) / 100;
                                              unit.influence[air] = 0;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::ZERG_VIPER:
                                              unit.influence[ground] = unit.influence[air] = (unit.health + unit.shield) / 100;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              case UNIT_TYPEID::ZERG_BROODLORD:
                                              unit.influence[ground] = (unit.health + unit.shield + (20 / 2.5) * 9.5) / 100;
                                              unit.influence[air] = 0;
                                              unit.influence[general] = unit.influence[ground] + unit.influence[air];
                                              break;
                                              default:
                                              unit.influence[ground] = 1.5;
                                              unit.influence[air] = 1.5;
                                              unit.influence[general] = 1.5;
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
                                              
                                              void PropigateInfluence(floatVector &general_IM, const boolVector &map, const float &momentumRate, const float &decayRate) {
                                                  Units selfUnits = Observation()->GetUnits(Unit::Alliance::Self);
                                                  Units enemyUnits = Observation()->GetUnits(Unit::Alliance::Enemy);
                                                  UpdateVisableUnits(selfUnits);
                                                  UpdateVisableUnits(enemyUnits);
                                                  
                                                  int radius = 0;
                                                  for (unsigned i = 0; i < umap.bucket_count(); ++i) {
                                                      for (auto local_it = umap.begin(i); local_it != umap.end(i); ++local_it) {
                                                          if (general_IM[local_it->second.pos_x][local_it->second.pos_y] <= 5 || general_IM[local_it->second.pos_x][local_it->second.pos_y] >= -5) {
                                                              if (map[local_it->second.pos_x][local_it->second.pos_y] == 1) {
                                                                  general_IM[local_it->second.pos_x][local_it->second.pos_y] += local_it->second.influence[general];
                                                                  PropigationDecay(general_IM, decayRate, local_it->second.pos_x, local_it->second.pos_y, radius);
                                                              }
                                                              
                                                              
                                                              
                                                              
                                                          }
                                                          
                                                      }
                                                  }
                                                  
                                                  
                                                  PropigationMomentum(general_IM, map, momentumRate);
                                                  
                                              }
                                              
                                              
                                              
                                              
                                              // Used by PropigateInfluence() to affect rate of propigation
                                              void PropigationMomentum(floatVector &influenceMap, const boolVector &map, const float &rate) {
                                                  // Use a parameter to define a momentum rate from 0 to 1.0
                                                  // This function effects how long a particular "node" on the graph will retain influence from a unit that has moved away from that node
                                                  
                                                  GameInfo info = Observation()->GetGameInfo();
                                                  
                                                  // For every tile that is not of zero influence
                                                  // multiply it by the influence rate
                                                  for (int i = 0; i < info.width; ++i) {
                                                      for (int j = 0; j < info.height; ++j) {
                                                          
                                                          // If you can't walk there, you don't exhibit influence there
                                                          // May need to remove
                                                          if (map[i][j] == 0)
                                                          influenceMap[i][j] = 0;
                                                          
                                                          if ((influenceMap[i][j] != 0)) {
                                                              influenceMap[i][j] *= rate;
                                                              if (absoluteValue(influenceMap[i][j]) >= 0.1) {
                                                                  influenceMap[i][j] = round(influenceMap[i][j]);
                                                              }
                                                              else {
                                                                  influenceMap[i][j] = 0;
                                                              }
                                                          }
                                                      }
                                                  }
                                              }
                                              
                                              
                                              
                                              // Used by PropigateInfluence() to affect how far will a unit's influence spread
                                              void PropigationDecay(floatVector &influenceMap, const float &rate, const int &x, const int &y, int &radius) {
                                                  // Diagonals are minus 0.18
                                                  // Left/Right Up/Down are minus 0.12
                                                  
                                                  
                                                  GameInfo info = Observation()->GetGameInfo();
                                                  float value = influenceMap[x][y];
                                                  float radiusValue = influenceMap[x][y];
                                                  
                                                  radius = 0;
                                                  while (radiusValue - (rate + 0.06) >= 0) {
                                                      radiusValue -= (rate + 0.06);
                                                      radius++;
                                                  }
                                                  
                                                  if (value <= 5 || value >= -5) {
                                                      if (value < 0) {
                                                          
                                                          int i = 1;
                                                          while (value + (rate + 0.06) <= 0) {
                                                              if ((x + i < info.width) && (x - i > 0) && (y + i < info.height) && (y - i > 0)) {
                                                                  // Up/Down Left/Right
                                                                  influenceMap[x + i][y] = round(value + rate);
                                                                  influenceMap[x - i][y] = round(value + rate);
                                                                  influenceMap[x][y + i] = round(value + rate);
                                                                  influenceMap[x][y - i] = round(value + rate);
                                                                  
                                                                  // Diagonals
                                                                  influenceMap[x + i][y - i] = round(value + (rate + 0.06));
                                                                  influenceMap[x + i][y + i] = round(value + (rate + 0.06));
                                                                  influenceMap[x - i][y - i] = round(value + (rate + 0.06));
                                                                  influenceMap[x - i][y + i] = round(value + (rate + 0.06));
                                                                  
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
                                                                  
                                                                  value += (rate + 0.06);
                                                                  i++;
                                                                  
                                                              }
                                                              else {
                                                                  break;
                                                              }
                                                          }
                                                          
                                                      }
                                                      else if (value > 0) {
                                                          int i = 1;
                                                          while (value - (rate + 0.06) >= 0) {
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
                                              }
                                              
                                              
                                              
                                              void FindUnitByTag(const Tag &tag, const Units &units, int &x, int &y) {
                                                  for (const auto& unit : units) {
                                                      if (unit->tag == tag) {
                                                          x = unit->pos.x;
                                                          y = unit->pos.y;
                                                      }
                                                  }
                                                  
                                              }
                                              
                                              float round(float number) {
                                                  if (number >= 0) {
                                                      float value = (int)(number * 100 + 0.5);
                                                      return (float)value / 100;
                                                  }
                                                  else {
                                                      float value = (int)(number * 100 - 0.5);
                                                      return (float)value / 100;
                                                  }
                                                  
                                                  
                                              }
                                              
                                              float absoluteValue(float number) {
                                                  if (number >= 0)
                                                  return number;
                                                  else
                                                  return (number * -1);
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
