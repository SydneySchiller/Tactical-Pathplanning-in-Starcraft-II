#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include <stdlib.h>
#include "sc2api/sc2_interfaces.h"
#include <unordered_map>
#include "sc2utils/sc2_manage_process.h"
#include <utility>
#include <queue>
#include <iostream>

typedef std::vector<std::vector<bool>> boolVector;
typedef std::vector<std::vector<float>> floatVector;
floatVector general_IM = floatVector(500, std::vector<float>(500, 0.0));


class node {
public:
    
    int x_index;
    int y_index;
    int val;
    int g_value;
    int f_value;
    int h_value;
    int g() {
        return val;
    }
    int h(int xGoal, int yGoal) {
        return sqrt(pow(xGoal - x_index, 2) + pow(yGoal - y_index, 2));
    }
    int f(int xGoal, int yGoal) {
        return g() + h(xGoal, yGoal);
    }
    bool discovered;
    node* backpointer;

    

    node() {
        x_index, y_index, val, g_value, f_value, h_value = 0;
        discovered = false;

    }

    

    node(int x, int y, int v, int xGoal, int yGoal) {
        
        x_index = x;
        y_index = y;
        val = v;
        discovered = false;
        
        g_value = val;
        f_value = f(xGoal, yGoal);
        h_value = h(xGoal, yGoal);
    }

    node(const node &obj) {
        x_index = obj.x_index;
        y_index = obj.y_index;
        val = obj.val;
        f_value = obj.f_value;
        g_value = obj.g_value;
        h_value = obj.h_value;
        discovered = obj.discovered;
        backpointer = obj.backpointer;
    }

    ~node()
    {

    }

    bool node::operator<(const node& rhs) const {
        return !(f_value < rhs.f_value);
    }
    


};





class graph {
public: 
    floatVector general_IM = floatVector(3, std::vector<float>(3, 0.0));
    node nodeGraph[3][3];

    graph(floatVector g) {
        general_IM = g;
        for (int i = 0; i < general_IM[0].size(); i++) {
            for (int j = 0; j < general_IM.size(); j++) {
                nodeGraph[i][j] = node();
            }
        }
    }

    node at(int index);

    node at(int Xindex, int Yindex) {
        return nodeGraph[Xindex][Yindex];
    }

    void set(int x, int y, node n)
    {
        nodeGraph[x][y].x_index = n.x_index;
        nodeGraph[x][y].y_index = n.y_index;
        nodeGraph[x][y].val = n.val;
        nodeGraph[x][y].f_value = n.f_value;
        nodeGraph[x][y].g_value = n.g_value;
        nodeGraph[x][y].h_value = n.h_value;
        nodeGraph[x][y].discovered = n.discovered;
        nodeGraph[x][y].backpointer = n.backpointer;

    }

};

struct Compare
{
    bool operator() (node lhs, node rhs)
    {
        return !(lhs.f_value < rhs.f_value);
    }
};

std::priority_queue<node> search() {
    
    int x_start_index = 0;
    int y_start_index = 0;
    int x_goal_index = 2;
    int y_goal_index = 2;



    floatVector sample = floatVector(3, std::vector<float>(3, 0.0));
    sample[0][0] = 1;
    sample[0][1] = 2;
    sample[0][2] = 3;
    sample[1][0] = 8;
    sample[1][1] = 1;
    sample[1][2] = 4;
    sample[2][0] = 2;
    sample[2][1] = 5;
    sample[2][2] = 9;
    int index = 0;
    graph Graph(sample);
    node n(x_start_index, y_start_index, Graph.general_IM[x_start_index][y_start_index], x_goal_index, y_goal_index);
    n.g_value = 0;
    n.f_value = n.f(x_goal_index, y_goal_index);
    n.discovered = true;
    n.backpointer = NULL;
    std::priority_queue<node> open;
    std::priority_queue<node> closed;
    
    open.push(n);
    std::make_heap(const_cast<node*>(&open.top()),
        const_cast<node*>(&open.top()) + open.size(),
        Compare());
    Graph.set(x_start_index, y_start_index, n);

    while (!open.empty()) {

        node n = open.top();
        open.pop();
        if (!open.empty())
        std::make_heap(const_cast<node*>(&open.top()),
            const_cast<node*>(&open.top()) + open.size(),
            Compare());
        
        //add to graph

        if (n.x_index == x_goal_index && n.y_index == y_goal_index)
            return closed; // success

                           //checks neighbors, makes nodes of them
        for (int x = -1; x < 2; x++) {
            for (int y = -1; y < 2; y++) {

                if (x + n.x_index < 0 || x + n.x_index > Graph.general_IM[0].size() ||
                    y + n.y_index < 0 || y + n.y_index > Graph.general_IM.size() ||
                    ((x == 0) && (y == 0)))
                    continue;



                if (!Graph.at(n.x_index + x, n.y_index + y).discovered ||
                    Graph.at(n.x_index + x, n.y_index + y).f_value > n.g_value + Graph.at(n.x_index + x, n.y_index + y).f_value) {

                    node n_prime(n.x_index + x, n.y_index + y, Graph.general_IM[n.x_index + x][n.y_index + y], x_goal_index, y_goal_index);
                    n_prime.discovered = true;
                    n_prime.f_value = n.g_value + n_prime.f(x_goal_index, y_goal_index);
                    n_prime.backpointer = &n;
                    open.push(n_prime);
                    std::make_heap(const_cast<node*>(&open.top()),
                        const_cast<node*>(&open.top()) + open.size(),
                        Compare());
                    Graph.set(n.x_index + x, n.y_index + y, n_prime);

                }

                else {
                    continue;
                }

               

            }

        }
        closed.push(n);
        std::make_heap(const_cast<node*>(&closed.top()),
            const_cast<node*>(&closed.top()) + closed.size(),
            Compare());
    }
    std::priority_queue<node> fail;
    return fail;

}




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

        if (general_IM[rand.x][rand.y] == -1) {
            end_pos.x = end_pos.x - 1;
            std::cout << "AVOIDED" << std::endl;
            Path(start_pos, end_pos);
        }
        else {
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
            std::cout << "loop" << std::endl;
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
    search();
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