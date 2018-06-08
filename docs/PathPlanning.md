## How it works
### Avoiding Negative Numbers On the Map

Using an example influence map (see screenshot), there is a defined function MoveTo() and a defined function Path() which when we enter the final destination we want the bot to go to in Path(), the bot checks the influence map at that location. If the influence in that area is negative, we recursively call Path() with to adjust the path.

### Types of Influence
If there is "low influence" in a 1x1 square around end position, we adjust end position to this "low influence" point.
If there is "medium influence" in a 3x3 square around end position, adjust end position to this "medium influence" point.
If there is "high influence" in a 5x5 square around end position, adjust end position to this "high influence" point.

### Images

An example influence map, a block of -1 values, that the bot won't enter:
[https://imgur.com/QyehPfr](https://imgur.com/QyehPfr)

Gif of it running:
In the minimap: We can see units avoiding the lower left corner of the map
In the terminal window: Under the loop "AVOIDED" flashes when the negative influence area is avoided.
[https://imgur.com/3pqBMI5](https://imgur.com/3pqBMI5)

### Side Information
There are functions to draw lines in the StarCraft 2 API (see commented out debug code, bottom three are syntax to properly draw a line), however, we couldn't find any examples of this working and weren't able to get it to work properly.
