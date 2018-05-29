Made with bot_simple.cc example file structure.
Replace tutorial.cc with this file's code.

Update: Now it is compatible with an example general_IM matrix.

It currently AVOIDS NEGATIVE NUMBERS in the influence map.

How it works:
generates an example influence map (see screenshot)
defines function MoveTo
defines function Path
We enter the final destination we want the bot to go to in Path
the bot checks the influence map at that location
if it is negative, adjust the path.
else: go to that path

TODO:
Postive influence pathplanning
find path to maxamize influence



[Imgur](https://i.imgur.com/3pqBMI5.gifv)




