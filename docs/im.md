# **Influence Map Information**

### Site Navigation
[Project Report](../README.md)<br/>
[Deliverables](deliverables.md)<br/>
[About Out Influence Maps](im.md)<br/>

### General Information

##### Influence Maps

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

##### You can see momentum working below on a single unit being moved across the map:
<p align="center">
  <img src="https://i.imgur.com/gx9VRYi.jpg"><br/>
Figure 2 - Trail of a single SCV moving across the map
</p>
<p align="center">
  <img src="https://i.imgur.com/Uf1lY0U.jpg"><br/>
Figure 3 - When the SCV comes to a stop, it spreads its influence again.
</p>

##### On all units being moved across the map:
<p align="center">
  <img src="https://i.imgur.com/gwrIfqc.jpg"><br/>
Figure 4 - Trail of all SCVs moving across the map
</p>
<p align="center">
  <img src="https://i.imgur.com/tgAk2Ks.jpg"><br/>
Figure 5 - When the SCVs come to a stop, they spread their influence again.
</p>
