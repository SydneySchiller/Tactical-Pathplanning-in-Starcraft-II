Made with bot_simple.cc example file structure.
Replace tutorial.cc with this file's code.

Update: Now it is compatible with an example general_IM matrix.

It currently AVOIDS NEGATIVE NUMBERS in the influence map.

___________________ How it works
generates an example influence map (see screenshot)
defines function MoveTo
defines function Path
We enter the final destination we want the bot to go to in Path
the bot checks the influence map at that location
if it is negative, recursively call Path with an adjusted the path.
else: go to that path



___________________ Todo
Postive influence pathplanning
find path to maxamize influence
A*



___________________ PICS

My example influence map.  It is a block of -1s that the bot won't enter:
https://imgur.com/QyehPfr

Gif of it running:
see minimap: bots avoid lower left corner of the map (I guess the coordinate system is flipped?)
see terminal window: under loop "AVOIDED" flashes when the negative influence area is avoided.
https://imgur.com/3pqBMI5






___________________ Side info
I found the documenation for how to draw on the map.  It may be valuable for us to explore this for visualization.  I was trying to get it to work.  There are functions to draw lines in the API.  See commented out debug code, bottom three are syntax to properly draw a line.  I couldn't find any example code online tho, only documentation.



