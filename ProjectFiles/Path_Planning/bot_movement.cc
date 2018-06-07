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
//variables: types of influence
int do_not_enter = 9;
int high_influence = 3;
int low_influence = 1;
int medium_influence = 2;



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
        
        
        
        
        //If the point is do_not_enter.
        if(general_IM[rand.x][rand.y] == do_not_enter){
            end_pos.x = end_pos.x - 1; //scan left until we find something that doesn't have do_not_enter
            std::cout << "Avoided Negative influence" << std::endl;
            Path(start_pos, end_pos);
            }
            
            //Todo: make these into seperate functions
            ////Scan area around path -- scans a 1x1 square for low influence
            int check_box = 1;
            for(int i = 0; i < check_box; ++i){
                for(int j = 0; j < check_box; ++j){
                    if(general_IM[rand.x + i][rand.y + j] == low_influence && general_IM[rand.x][rand.y] != low_influence){ //need to fix, probably not just + i +j
                        std::cout << "Adjusted path to low influence" << std::endl;
                        end_pos_2.x = rand.x + i; //location of point
                        end_pos_2.y = rand.y + j; //location of point
                        //Path(start_pos, end_pos); //new end point
                        
                    }
                    
                }
                
            }
            //Scan area around path -- scans a 3x3 square for medium medium
            check_box = 3;
            for(int i = 0; i < check_box; ++i){
                for(int j = 0; j< check_box;++j){
                    if(general_IM[rand.x + i][rand.y + j] == medium_influence && general_IM[rand.x][rand.y] != medium_influence){ //need to fix, probably not just + i +j
                        std::cout << "Adjusted path to medium influence" << std::endl;
                        end_pos_2.x = rand.x + i; //location of point
                        end_pos_2.y = rand.y + j; //location of point
                        //Path(start_pos, end_pos); //new end point
                        
                    }
                    
                }
                
            }
            
            
            //CHECK FOR HIGH INFLUENCE 5x5 square
            check_box = 5;
            for(int i = 0; i < check_box; ++i){
                for(int j = 0; j < check_box; ++j){
                    if(general_IM[rand.x + i][rand.y + j] == high_influence  && general_IM[rand.x][rand.y] != high_influence){ //need to fix, probably not just + i +j
                        std::cout << "Adjusted path to high influence" << std::endl;
                        end_pos_2.x = rand.x + i; //location of point
                        end_pos_2.y = rand.y + j; //location of point
                        Path(start_pos, end_pos); //new end point
                        
                    }
                    
                }
                
            }
            
            
            MoveTo(start_pos, end_pos_2); //Go to the new point
            
            
            
            return; // ends the recursion
            
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
                        general_IM[i][j] = 0.0;
                    }
                    
                }
                
                //Generate EX. Influence map.
                
                //Variables
                //int do_not_enter = -1;
                //GENERATE BLOCK OF NEGATIVE NUMBERS
                
                for (int i = 40; i < 70; ++i) {
                    for (int j = 45; j < 75; ++j) {
                        general_IM[i][j] = do_not_enter;
                    }
                    
                }
                
                for (int i = 70; i < 80; ++i) {
                    for (int j = 45; j < 65; ++j) {
                        general_IM[i][j] = low_influence;
                    }
                    
                }
                
                for (int i = 80; i < 90; ++i) {
                    for (int j = 45; j < 65; ++j) {
                        general_IM[i][j] = medium_influence;
                    }
                    
                }
                
                for (int i = 30; i < 40; ++i) {
                    for (int j = 40; j < 50; ++j) {
                        general_IM[i][j] = high_influence;
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
