
# **Project: Tactical Path Planning Using Influence Maps to Identify the Safest Path**

### Site Navigation
[Project Report](../README.md)<br/>
[Deliverables](deliverables.md)<br/>

### Page Contents
- Where to Find Files
- The Working Product
- How Our Influence Maps Work
- How Our Pathplanning Works With Our Influence Maps


##### Deliverables

#### Where to Find Files
Inside our [git repo](https://github.com/SydneySchiller/ECS170-Project) you can find the following deliverables:
**Executable:** SC2 Project/BotExample.exe<br/>
**Code:** SC2 Project/project/main.cpp<br/>
(Because we use the precompiled lib, there is no building needed)

#### The Working Product

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
In the terminal window: Under the loop "AVOIDED" flashes when the negative influence area is avoided.
[https://imgur.com/3pqBMI5](https://imgur.com/3pqBMI5)

#### Side Information
There are functions to draw lines in the StarCraft 2 API (see commented out debug code, bottom three are syntax to properly draw a line), however, we couldn't find any examples of this working and weren't able to get it to work properly.
