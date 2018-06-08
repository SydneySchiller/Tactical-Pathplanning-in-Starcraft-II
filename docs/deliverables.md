
# **Project: Tactical Path Planning Using Influence Maps to Identify the Safest Path**

### Site Navigation
[Project Report](../README.md)<br/>
[Deliverables](deliverables.md)<br/>
[About Out Influence Maps](im.md)<br/>


### Deliverables

<p align="center">
  <img src="https://media.giphy.com/media/5zdtUxDPWngbS09hCl/giphy.gif"><br/>
Gif 1: Displays units avoiding the negative influence of the enemies by skirting around them rather than directly charging in.
</p>

<p align="center">
  <img src="https://media.giphy.com/media/8L1KiMGY3l2cexdJ5U/giphy.gif"><br/>
Gif 2: A crude representation of the influence map updating to show units moving across the map, we can see influence propagation (the circular-ish areas of numbers), influence decay (the numbers gradually get smaller as they spread out, and influence momentum (as units move away from a location their influence dissipates over time).
</p>

<p align="center">
  <img src="https://i.imgur.com/CVS4TmP.png"><br/>
Image 1: A closer look at units (pictured as the center mass) moving away from the command center (pictured left of the center mass) and an additional single unit moving away from both (pictured as the lonely 0.13 value).
</p>

<p align="center">
  <img src="https://i.imgur.com/MefINzG.png"><br/>
Image 2: A small group of SCVs encountering the Zerg base. We can see the SCVs have a positive influence on the left and the enemy Zerg have a negative influence on the right. The smaller numbers surrounding the positive values are remnant influence left over by the SCVs moving--a sort of "trail."
</p>
