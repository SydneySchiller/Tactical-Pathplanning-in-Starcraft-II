#include <sc2api/sc2_api.h>

#include <iostream>

using namespace sc2;


/*
 * TODO:
 * Add propagation algorithm
 * Add momentum function (affects rate of propigation)
 * Add decay function (how far will a unit's influence spread?)
 * Add update frequency (right now it simply updates on step, but this is too often)
 * Add visualization
*/

// Basic unit structure to be added to hash table
struct unit_t {
    Tag tag;
    UnitTypeID type;
    float health;
    float health_max;
    float shield;
    float shield_max;
    bool is_alive;
    uint32_t last_seen; // the last time this unit was seen
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
        //std::cout << TotalUnitCount()<< std::endl;
        //std::cout << UnitTypeCount(UNIT_TYPEID::TERRAN_SCV) << std::endl;
        UpdateVisableUnits();

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
    void UpdateVisableUnits() {
        Units units = Observation()->GetUnits(Unit::Alliance::Self);
        unit_t newUnit;
        // Cycle through units
        for (const auto& unit : units) {
            // check hash table for matching tag
            // if no matching tag then
            // populate unit structure
            newUnit.tag = unit->tag;
            newUnit.type = unit->unit_type;
            newUnit.health = unit->health;
            newUnit.health_max = unit->health_max;
            newUnit.shield = unit->shield;
            newUnit.shield_max = unit->shield_max;
            newUnit.is_alive = unit->is_alive;
            newUnit.last_seen = unit->last_seen_game_loop;
            // add to hash table
        }
    }

    // Returns count of a particular type of unit
    size_t UnitTypeCount(UNIT_TYPEID unit_type) {
        return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
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