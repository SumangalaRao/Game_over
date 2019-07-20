# ---SpaceX---
 It is a terminal based game developed in c++,using ncurses which is a library in c/c++,that gives us the required API's for writing a text based UI in a terminal independent manner 
# theme/idea:

there are total three types of object on screen
   1 A spacecraft which is in your control------<<8>>
   2 Positive objects(harmless) for your spaceraft,can be considered as stars------'.'
   3 negative objects(harmfull) for your spacecraft,can be considered to be asteroids------'!'
 you gotta avoid the harmfull stuff,a max of 3 chances will be given to you 
 after which the game's over!
 # how to play/run?
 1.use ubuntu(or any other linux distributions)
 2.install ncurses:
    (via terminal:sudo apt-get install libncurses5-dev libncursesw5-dev)
 3.g++ xxxx.cpp -lncurses (or if you want to create a seperate output file:g++ xxxx.cpp -o output -lncurses)
 4./a.out(or the file created:./output)
