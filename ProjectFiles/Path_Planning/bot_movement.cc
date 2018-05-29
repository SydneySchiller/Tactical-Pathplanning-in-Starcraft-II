#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include <stdlib.h>
#include "sc2api/sc2_interfaces.h"
#include <unordered_map>
#include "sc2utils/sc2_manage_process.h"
#include <utility>
#include <iostream>

typedef std::vector<std::vector<bool>> boolVector;
typedef std::vector<std::vector<float>> floatVector;
floatVector general_IM = floatVector(500, std::vector<float>(500, 0.0));



class FooBot : public sc2::Agent {
public:
    uint32_t restarts_;
    
    FooBot() :
    restarts_(0) {
    }
    
    virtual void Path(sc2::Point3D start_pos, sc2::Point3D end_pos) final {
        /*
        MoveTo(start_pos, end_pos);
        
        
        
        sc2::Point3D end_pos_3;
        end_pos_3.x = 0;
        end_pos_3.y = 0;
        MoveTo(start_pos, end_pos_3);
        */
        
        
        
        sc2::Point2D rand = sc2::FindRandomLocation(Observation()->GetGameInfo());
        sc2::Point3D end_pos_2;
        end_pos_2.x = rand.x;
        end_pos_2.y = rand.y;
        
        if(general_IM[rand.x][rand.y] == -1){
            end_pos.x = end_pos.x -1;
            std::cout << "AVOIDED" << std::endl;
            Path(start_pos, end_pos);
        }
        else{
            MoveTo(start_pos, end_pos_2);
        }
        
       
        return;
        
    };
    virtual void MoveTo(sc2::Point3D start_pos, sc2::Point3D end_pos) final {
        sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
        for (auto& it_unit : units) {
            
            //sc2::Point2D target = sc2::FindRandomLocation(Observation()->GetGameInfo()); //get random point
            
            
            
            sc2::GameInfo mapinfo = Observation()->GetGameInfo();
            //height: 160
            //width: 144
            sc2::Rect2D rect_test;
            rect_test.from = start_pos;
            rect_test.to = start_pos;
            
            Actions()->UnitCommand(it_unit, sc2::ABILITY_ID::SMART, end_pos);
            
            
            
            
            /*DEBUG FNS FOR DRAWING LINE ON MAP
            //const sc2::Unit **unit_pos_x = *it_unit;
            //std::cout << unit_pos_x << std::endl;
            //Debug()->sc2::DebugInterface::DebugLineOut(start_pos, end_pos);
            //const sc2::ObservationInterface* obs = FooBot->Observation();
             
            //This should be it but...
            //sc2::DebugInterface* debug;  //works
            //debug->DebugIgnoreMineral(); //this automatically closes the game right when it opens...
            //debug->DebugLineOut(start_pos, end_pos); //this is what we want...
            //debug->SendDebug();
            
            */
            }
        };
    
    
    virtual void OnGameStart() final {
        std::cout << "Starting a new game (" << restarts_ << " restarts)" << std::endl;
        };
        
        sc2::GameInfo mapinfo = Observation()->GetGameInfo();
        bool arrived = 0;
        virtual void OnStep() final {
            
            
            for (int i = 0; i < 144; ++i) {
                for (int j = 0; j < 160; ++j) {
                    general_IM[i][j] = 0;
                }
                
            }
            
            //GENERATE BLOCK OF NEGATIVE NUMBERS
            
            for (int i = 40; i < 110; ++i) {
                for (int j = 45; j < 105; ++j) {
                    general_IM[i][j] = -1;
                }
                
            }
            
            
            
            uint32_t game_loop = Observation()->GetGameLoop();
            //std::cout << arrived<<std::endl;
            if (game_loop % 100 == 0) {
            
                std::cout << mapinfo.width;
                for (int i = 0; i < 144; ++i) {
                    for (int j = 0; j < 160; ++j) {
                        
                        std::cout << general_IM[i][j];
                    }
                    std::cout << std::endl;
                }
                sc2::Point2D rand = sc2::FindRandomLocation(Observation()->GetGameInfo());
                sc2::Point3D target;
                target.x = rand.x;
                target.y = rand.y;
                
                sc2::Point3D start;
                target.x = 100;
                target.y = 100;
                
                
                
                Path(start, target);
                std::cout << "loop"<<std::endl;
                arrived = 1;
                
                
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
