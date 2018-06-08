Made with bot_simple.cc example file structure.
//Replace tutorial.cc with this file's code. 
Run executable to see it in action.

Update: Now it is compatible with an example general_IM matrix.
Update: Now it is combatible with low, medium, and high influence.  

//It currently AVOIDS NEGATIVE NUMBERS in the influence map.
//It currently AVOIDS NEGATIVE NUMBERS and ADJUSTS PATH TO HIGH NUMBERS in the influence map.
Update: Fully functional and combined with influencemap.cc
___________________ How it works
Update: See codeTour.pdf for more details

generates an example influence map (see screenshot)
defines function MoveTo
defines function Path
We enter the final destination we want the bot to go to in Path
the bot checks the influence map at that location
if it is negative, recursively call Path with an adjusted the path.

*update june 4:
check the end position called

if there is "low influence" in a 1x1 square around end pos, adjust endpos to this "low influence" point.
if there is "medium influence" in a 3x3 square around end pos, adjust endpos to this "medium influence" point.
if there is "high influence" in a 5x5 square around end pos, adjust endpos to this "high influence" point.

go to the updated path path



___________________ Todo
Postive influence pathplanning - DONE
find path to maxamize influence - DONE via a square checking system
A* - may not be necessary due to SC2's built in pathplanning.  Will investigate 
combine with influence map - DONE



___________________ PICS

Update: project page of gifs: https://imgur.com/a/mLViLfj

My example influence map.  It is a block of -1s that the bot won't enter:
https://imgur.com/QyehPfr

Gif of it running:
see minimap: bots avoid lower left corner of the map (I guess the coordinate system is flipped?)
see terminal window: under loop "AVOIDED" flashes when the negative influence area is avoided.
https://imgur.com/3pqBMI5






___________________ Side info
//future project:

I found the documenation for how to draw on the map.  It may be valuable for us to explore this for visualization.  I was trying to get it to work.  There are functions to draw lines in the API.  See commented out debug code, bottom three are syntax to properly draw a line.  I couldn't find any example code online tho, only documentation.



