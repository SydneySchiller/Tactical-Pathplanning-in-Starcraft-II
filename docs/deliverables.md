
# **Project: Tactical Path Planning Using Influence Maps to Identify the Safest Path**

### Site Navigation
[Project Report](../README.md)<br/>
[Deliverables](deliverables.md)<br/>

### Page Contents
- Where to Find Files
- The Working Product
- How Our Influence Maps Work
- How Our Pathplanning Works With Our Influence Maps


## Deliverables

### Where to Find Files
Inside our [git repo](https://github.com/SydneySchiller/ECS170-Project) you can find the following deliverables:<br/>
**Executable:** SC2 Project/BotExample.exe<br/>
**Code:** SC2 Project/project/main.cpp<br/>
(Because we use the precompiled lib, there is no building needed)<br/>

### The Working Product

<p align="center">
  <img src="https://media.giphy.com/media/5zdtUxDPWngbS09hCl/giphy.gif"><br/>
Gif 1: Displays units avoiding the negative influence of the enemies by skirting around them rather than directly charging in.
</p>
<br/>

<p align="center">
  <img src="https://media.giphy.com/media/8L1KiMGY3l2cexdJ5U/giphy.gif"><br/>
Gif 2: A crude representation of the influence map updating to show units moving across the map, we can see influence propagation (the circular-ish areas of numbers), influence decay (the numbers gradually get smaller as they spread out, and influence momentum (as units move away from a location their influence dissipates over time).
</p>
<br/>

<p align="center">
  <img src="https://i.imgur.com/CVS4TmP.png"><br/>
Image 1: A closer look at units (pictured as the center mass) moving away from the command center (pictured left of the center mass) and an additional single unit moving away from both (pictured as the lonely 0.13 value).
</p>
<br/>

<p align="center">
  <img src="https://i.imgur.com/MefINzG.png"><br/>
Image 2: A small group of SCVs encountering the Zerg base. We can see the SCVs have a positive influence on the left and the enemy Zerg have a negative influence on the right. The smaller numbers surrounding the positive values are remnant influence left over by the SCVs moving--a sort of "trail."
</p>
<br/>

### How Our Influence Maps Work

    enum InfluenceType { ground = 0, air = 1, general = 3 };
There are 3 Influence map types: ground, air, and general.
The General Influence map is currently the most functional. It is defined as:

    floatVector general_IM = floatVector(500, std::vector<float>(500, 0.0));

We currently store information about each active unit in an unordered map, called umap. Each unit in umap can be found by its key, which we chose to be the unit tag because it is unique to each unit. By looking up a unit by its tag, you can find out this information:

    struct unit_t {
      Tag tag;
      UNIT_TYPEID type;
      int alliance;
      float health;
      float health_max;
      float shield;
      float shield_max;
      bool is_alive;
      uint32_t last_seen;         
      int pos_x;
      int pos_y;
      float influence[3];        
    };

#### Decay
<p align="center">
  <img src="https://i.imgur.com/SNHLusc.jpg"><br/>
Figure 1 - Influence decay around a Terran base populated by SCVs
</p>
The farther the decay spreads from the center point of influence, the influence value decays to zero.

#### Momentum
Momentum determines how much history you choose to represent on an influence map. A momentum rate closer to 1 will keep most of the information on the map, while a momentum rate closer to 0 will get rid of influence faster. However, you do want to keep the momentum rate above the decay rate, or the momentum will destroy almost all information on the map.

#### You can see momentum working below on a single unit being moved across the map:
<p align="center">
  <img src="https://i.imgur.com/gx9VRYi.jpg"><br/>
Figure 2 - Trail of a single SCV moving across the map
</p>
<p align="center">
  <img src="https://i.imgur.com/Uf1lY0U.jpg"><br/>
Figure 3 - When the SCV comes to a stop, it spreads its influence again.
</p>

#### On all units being moved across the map:
<p align="center">
  <img src="https://i.imgur.com/gwrIfqc.jpg"><br/>
Figure 4 - Trail of all SCVs moving across the map
</p>
<p align="center">
  <img src="https://i.imgur.com/tgAk2Ks.jpg"><br/>
Figure 5 - When the SCVs come to a stop, they spread their influence again.
</p>


### How Our Pathplanning Works With Our Influence Maps
#### Code Tour

```
std::cout << mapinfo.width;
for(int i = 0; i < 144; ++i){
  for(int j = 0; j < 160; ++j){
    std::cout << general_IM[i][j];
  }
   std::cout << std::endl;
}
```
#### Path Function:
First we check to see if the path entered is negative.  If it is, it is labled do_not_enter. If the point entered is negative, we adjust the path and call the Path function again recursively with the updated point.

```
//If the point is do_not_enter.
if(general_IM[rand.x][rand.y] == do_not_enter){
  end_pos.x = end_pos.x -1; // scan left until we find something that doesn't have do_not_enter
  std::cout << "Avoided Negative influence" << "\n\n\n\n\n" << std::endl;
  Path(start_pos, end_pos);
 }
 ```
Next we scan around the area to see if there are low, medium, or high areas of influence. We define a variable check_box, which is the radius around the path that we will check.  We check farther out for high influence than medium and low influence. Here is an example of the loop for medium influence:
```
// Scan area arount path -- scans a 3x3 square for medium medium
check_box = 3;
for (int i = 0; i < check_box; ++i){
  for(int j = 0; j < check_box; ++j){
    if(general_IM[rand.x + i][rand.y + j] == medium_influence && general_IM[rand.x][rand.y] != medium_influence){
      std::cout << "Adjusted path to medium influence\n\n\n\n\n" << std::endl;
      end_pos_2.x = rand.x + i; // location of point
      end_pos_2.y = rand.y + j; // location of point
      //Path(start_pos, end_pos); // new end point
    }
  }
}
```
Finally, at the end of Path, we call MoveTo to move to the new adjusted points.

```
MoveTo(start_pos, end_pos_2); // Go to the new point
```
#### MoveTo
In this function, we call the S2API’s unit movement based on a given point. Debug fns are commented out because one day I would like to implement drawing on the game to show it working.
```
virtual void MoveTo(sc2::Point3d start_pos, sc2::Point3d end_pos) final {
  sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
  for (auto& it_unit : units) {
    //sc2::Point2D target = sc2::FindRandomLocation(Oberservation()->GetGameInfor()); // get random point
    
    sc2::GameInfo mapinfo = Observation()->GetGameInfo();
    //height:160
    //width: 144
    sc2::Rect2D rect_test;
    rect_test.from = start_pos;
    rect_test.to = start_pos;
    
    Actions()->UnitCommand(it_unit, sc2::ABILITY_ID::SMART, end_pos);
    
    /*DEBUG FNS FOR DRAWING LINE ON MAP
      //const sc2::Unit **unit_pos_x = *it_unit;
      //std::cout << unit_pos_x << std::endl;
      //Debug()->sc2::DebugInterface* obs = FooBot->Observation();
      
      //This should be it but...
      //sc2::DebugInterface* debug; //works
      //debug->DebugIgnoreMineral(); //this automatically closes the game right when it opens...
      //debug->DebugLineOut(start_pos, end_pos); //this is what we want...
      //debug->sendDebug();
      
    */
  }
};
```

#### Generate Example Influence Map:
Here I generate an example influence map with all the available variables.
```
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
```
Here is where my path planning function is actually called.  Other bots can use our code in this manner to enhance their path planning.  For now, we send it to random locations to exhibit its functionality.
```
sc2::Point2D rand = sc2::FindRandomLocation(Observation()->GetGameInfo());
sc2::Point3D target;
target.x = rand.x;
target.y = rand.y;

sc2::Point3D start;
target.x = 100;
target.y = 100;

Path(start,target);
std::cout << "loop" << std::endl;
arrived = 1;
```

#### Avoiding Negative Numbers On the Map
Using an example influence map (see screenshot), there is a defined function MoveTo() and a defined function Path() which when we enter the final destination we want the bot to go to in Path(), the bot checks the influence map at that location. If the influence in that area is negative, we recursively call Path() with to adjust the path.

#### Types of Influence
If there is "low influence" in a 1x1 square around end position, we adjust end position to this "low influence" point.
If there is "medium influence" in a 3x3 square around end position, adjust end position to this "medium influence" point.
If there is "high influence" in a 5x5 square around end position, adjust end position to this "high influence" point.

#### Image
<p align="center">
  <img src="https://i.imgur.com/QyehPfr.png"><br/>
An example influence map, a block of -1 values, that the bot won't enter
</p>

#### Gif of it running:
In the minimap: We can see units avoiding the lower left corner of the map
In the terminal window: Under the loop "AVOIDED" flashes when the negative influence area is avoided.<br/>
[https://imgur.com/3pqBMI5](https://imgur.com/3pqBMI5)

#### Side Information
There are functions to draw lines in the StarCraft 2 API (see commented out debug code, bottom three are syntax to properly draw a line), however, we couldn't find any examples of this working and weren't able to get it to work properly.
