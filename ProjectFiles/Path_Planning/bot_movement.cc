#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"


#include "sc2utils/sc2_manage_process.h"

#include <iostream>

class FooBot : public sc2::Agent {
public:
    uint32_t restarts_;

    FooBot() :
        restarts_(0) {
    }

    virtual void OnGameStart() final {
        std::cout << "Starting a new game (" << restarts_ << " restarts)" << std::endl;
    };
    //*************************************************************************************************
        //pathplanning fn,
    virtual void PathPlanning(sc2::Point2D start_pos, sc2::Point2D end_pos) final {
        //takes in starting position and goes to end position avoiding areas with high/low influence
        
        
        //sc2::Point2D target = sc2::FindRandomLocation(Observation()->GetGameInfo()); //this finds a random location
        //Actions()->UnitCommand(it_unit, sc2::ABILITY_ID::SMART, target);
        
        //find point we want to go to:
        
        //influence_checker = check_influence(pos.x, pos.y); for final
        float influence_checker = -.2;//hard code for testing
        
        
        
        if (influence_checker <= 0) { //change x -1
            //change the direction of the bot
            sc2::Point2D new_start_pos = start_pos;
            new_start_pos.x = new_start_pos.x -1;
            
            Actions()->UnitCommand(it_unit, sc2::ABILITY_ID::SMART,new_start_pos);
            
            PathPlanning(new_start_pos, end_pos);

        }
        /* other cases testing
         if (influence_checker <= 0) { //change x +1
             //change the direction of the bot
             pos.x = pos.x + 1;
             PathPlanning(start_x, start_y, end_x, end_y)
         
         }
         if (influence_checker <= 0) { //change y -1
             //change the direction of the bot
             pos.y = pos.y - 1;
             PathPlanning(start_x, start_y, end_x, end_y)
         
         }
         
         if (influence_checker <= 0) { //change y +1
             //change the direction of the bot
             pos.y = pos.y + 1;
             PathPlanning(start_x, start_y, end_x, end_y)
         
         }
         
         
         */
        
        if (influence_checker >= 0) { //go directly to the point
            Actions()->UnitCommand(it_unit, sc2::ABILITY_ID::SMART, target);
        }
        
    };

        
        
    virtual void OnStep() final {
        uint32_t game_loop = Observation()->GetGameLoop();

        if (game_loop % 100 == 0) {
            sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
            for (auto& it_unit : units) {
               
                sc2::Point2D random_location = sc2::FindRandomLocation(Observation()->GetGameInfo());  //gets random location to go to
                sc2::Point2D unit_pos = unit->pos; //gets the current point of the unit
                
                
                PathPlanning(unit_pos.x, unit_pos.y, random_location.x, random_location.y)//path planning function
                
                //Actions()->UnitCommand(it_unit, sc2::ABILITY_ID::SMART, random_location);
            }
        }
    };

    virtual void OnGameEnd() final {
        ++restarts_;
        std::cout << "Game ended after: " << Observation()->GetGameLoop() << " loops " << std::endl;
    };

private:
};

//*************************************************************************************************
int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return 1;
    }

    // Add the custom bot, it will control the players.
    FooBot bot;

    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, &bot),
        CreateComputer(sc2::Race::Terran)
    });

    // Start the game.
    coordinator.LaunchStarcraft();

    // Step forward the game simulation.
    bool do_break = false;
    while (!do_break) {
        coordinator.StartGame(sc2::kMapBelShirVestigeLE);
        while (coordinator.Update() && !do_break) {
            if (sc2::PollKeyPress()) {
                do_break = true;
            }
        }
    }

    return 0;
}

