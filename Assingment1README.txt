TicTacToe


Hello, I probably won’t survive long but I’m glad that I made  this game keep to me company in my final days.
To whoever finds this clientside code and wishes to escape from this cruel world for a couple of hours -perhaps minutes- 
feel free. 
I hope this game brings you as much joy as you need to keep on living a couple more days, 
if not then I apologize for wasting your time. 
To those who have stayed to read this I have left instructions for how to use this game below. 
I made this for my dear friend fred, who tragically passed in a freak snowplow accident a few days earlier. 
I miss you dearly old friend…


PREREQUISITES:
* This is a compiled c++ code that must be run on windows OS. make sure your device is windows.


Steps to playing the game:
1. Compile ‘tictactoeClient.cpp’ 
2. Run code with two command line arguments:
* Arg1 = HOST (136.159.5.25)
* Arg2 = PORT (6969)
3. When run, you should see a welcome banner that gives 4 options, “New game”, “Load game”, “Show score” and “EXIT.”




NEW GAME AND LOAD GAME
To start a new game simply type NEWG and the the game will begin.
To load an older game type LOAD:<your file name here.txt> and the game will begin

******************************
There is an error where If you just write LOAD the whole game breaks. 
I did not know how to fix that.
******************************


						******* WARNING *******
		
The LOAD function doesn't work properly. filestuffs isn't recieving anything from a correct game file and I never found out why.

						************************

								
Below is a list of the various commands you can use to communicate with the server: Values are case sensitive 
 


* BORD:X,Y:
   *  X - X coordinate on a 3x3 grid
   * Y - Y coordinate on a 3x3 grid
* CLOS:
   * To close game without saving
* ENDG:
   * To end game with saving.



EXIT
When EXIT is called, the server stops and all games are finished.
You should get a message that says “Thank you for playing !”


Show Score
When show score is called, it should tell you the amount of games played, 
server wins and client wins.

