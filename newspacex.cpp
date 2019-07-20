/*
author:Sumangala Rao 
december 2018
*/
#include<iostream>
#include<cstdlib>
#include<vector>
#include<unistd.h>
#include<ncurses.h> //it provides,API to write text based UI on terminal
#include<string.h>
#include<cstdint> //for the limits
  using namespace std;

  //structure for co-ordinate points
  typedef struct
  {
     int_fast8_t x;
     int_fast8_t y;
  }vec2d;

  
  typedef struct 
  {
    vec2d offset;
    vec2d bounds;

    int_fast8_t top() 
    { 
	return offset.y; 
    }
    int_fast8_t bot() 
    { 
	return offset.y + bounds.y; 
    }
    int_fast8_t left() 
    { 
	return offset.x; 
    }
    int_fast8_t right() 
    { 
	return offset.x + bounds.x; 
     }

    int_fast8_t width() 
    { 
	return bounds.x; 
    }
    int_fast8_t height() 
    { 
	return bounds.y; 
    }

    bool contains(vec2d a)
    { 
	return (a.x >= offset.x && a.x < right()) && (a.y >= offset.y && a.y < bot()); 
    }
  } rect;


    struct
  {
     vec2d pos;
     char disp_char;
     int energy;
     rect bounds;    
  }player;
  
  
 /*
  struct 
  {
    vec2d pos;
  }enemy;

 
  struct  
  {
    vec2d pos;
  }star;

 */
  class SpaceObject
  {
	public:
	    SpaceObject(int, int);
	    void update();
    	    vec2d getPos() const;
    	    void setPos(vec2d);   
	private:
    	    vec2d pos;
  };


  class ObjectField 
  {
	public:
	    void update();
	    void erase(size_t);
	    std::vector<SpaceObject> getData() const;
	    void setBounds(rect);
	private:
    	    rect field_bounds;
    	    std::vector<SpaceObject> object_set;

  };
  

  void ObjectField::update() 
  {
  	  // update existing objects
  	  for(size_t i = 0; i < object_set.size(); i++)
	  {
  	      if(object_set.at(i).getPos().y > field_bounds.bot())
            		object_set.erase(object_set.begin() + i);

              object_set.at(i).update();
          }  
   
  	  // spawn a new object
  	  SpaceObject s(rand() % field_bounds.width(), 0);
	  object_set.push_back(s);  
   }

  //erases the ith value from the vector Object_set
  void ObjectField::erase(size_t i)
  {
    object_set.erase(object_set.begin() + i);
  }

  std::vector<SpaceObject> ObjectField::getData() const  
  { 
	   return object_set;
   }

  void ObjectField::setBounds(rect a) 
  { 
	   field_bounds=a; 
   }

  SpaceObject::SpaceObject(int nx, int ny) 
  { 
           pos.x = nx; 
           pos.y = ny; 
   }

  void SpaceObject::update() { pos.y += 1; }

  vec2d SpaceObject::getPos() const 
  { 
           return pos; 
  }


  WINDOW* wnd;
  WINDOW* game_wnd;

  ObjectField stars;
  ObjectField asteroids;
  
  rect game_area;
  rect screen_area;
  vec2d cur_size;
  int lifecount=3;
  string msglife="AVOID ASTEROIDS ";
  int init()
  {
	 srand(time(0));
	//initiates
	wnd=initscr();
	//disables buffering of characters
	cbreak();
	//supresses automatic echoing of typed char
	noecho();
	clear();
	//to make changes appear on screen
	refresh();
	//to capture keystrokes
	keypad(wnd,TRUE);
	//makes getch nonblocking one
	nodelay(wnd,TRUE);
	//makes cursor invisible  
	curs_set(0);

	//setting screen area to be of default terminal size
	screen_area = { { 0, 0 }, { 80, 24 } };
	
	int_fast8_t infopanel_height=4;
    	game_wnd = newwin(screen_area.height()-infopanel_height-2,screen_area.width()-2,screen_area.top()+1,screen_area.left()+1);
    	wnd=newwin(screen_area.height(),screen_area.width(),0,0);

    	game_area = { { 0, 0 }, { screen_area.width()-2,screen_area.height()-infopanel_height-4} };

   	 // enable function keys
   	 keypad(wnd, true);
   	 keypad(game_wnd, true);
	
   	 // disable input blocking
   	 nodelay(wnd, true);
   	 nodelay(game_wnd, true);

	return 0;
  }

  
  void run()
  {
	int tick;
	player.disp_char='^';
	player.pos={10,5};
	int in_char;	
	bool exit_requested=false;
	bool game_over = false;
	string msg="GAME_OVER";
	asteroids.setBounds(game_area);
	stars.setBounds(game_area);

	  // initial draw
   	wrefresh(wnd);
    	wrefresh(game_wnd);
	tick = 0;	
	while(1)
	{
		werase(game_wnd);
		//gets user input on the terminal window
		in_char=wgetch(wnd);

		mvaddch(player.pos.y,player.pos.x,' ');
		
		for(auto s : stars.getData()) 
        		mvaddch(s.getPos().y, s.getPos().x, ' ');

		stars.update();

		switch(in_char)
		{
		    case 'q':exit_requested=true;
			     break;
		    case KEY_UP:
		    case 'w':player.pos.y-=1;
			     break;
		    case KEY_DOWN:
		    case 's':player.pos.y+=1;
		             break;
		    case KEY_LEFT:
		    case 'a':player.pos.x-=1;
			     break;
		    case KEY_RIGHT:
		    case 'd':player.pos.x+=1;
			     break;
		    default: break;
		}
		
		// update object fields
        	if(tick % 7 == 0)
            	stars.update();

        	if(tick > 100 && tick % 20 == 0)
            	asteroids.update();

        	// update player bounds 
        	player.bounds = { { player.pos.x-1,player.pos.y },{ 3, 1 } }; // player is 3x1

       		// remove asteroid if collided
        	for(size_t i = 0; i < asteroids.getData().size(); i++) 
		{
            		if(player.bounds.contains(asteroids.getData().at(i).getPos()))
	    		{
	        		asteroids.erase(i);
				lifecount--;
				move(8,30);
				if(lifecount>0)
				{
					for(int i=0;i<msglife.length();i++)
					{
						addch(msglife[i]);
						usleep(100000);
						refresh();
					}
					addch(' ');
					refresh();
				}
				if(lifecount==0)
				{
					for(int i=0;i<msg.length();i++)
					{
					refresh();
					addch(msg[i]);
					addch(' ');
					sleep(1);
					refresh();
					}
				refresh();
				sleep(5);
				game_over=true;
				}
            		}
        	}

        	// draw object fields
        	for(auto s : stars.getData())
	            mvwaddch(game_wnd, s.getPos().y, s.getPos().x, '*');

        	for(auto a : asteroids.getData())
            		mvwaddch(game_wnd, a.getPos().y, a.getPos().x, '!');
			
            		
		//PLAYER OBJECT

        	// draw player body
        	mvwaddch(game_wnd, player.pos.y, player.pos.x, /*player.disp_char*/'8');
        	// draw player sides
       		mvwaddch(game_wnd, player.pos.y, player.pos.x-1, ACS_LARROW);
		mvwaddch(game_wnd, player.pos.y, player.pos.x -2, ACS_LARROW); 
       		mvwaddch(game_wnd, player.pos.y, player.pos.x+1, ACS_RARROW);
		mvwaddch(game_wnd, player.pos.y, player.pos.x+2, ACS_RARROW);
        	// draw engine flame
        	if((tick % 10) / 3)
           		mvwaddch(game_wnd, player.pos.y + 1, player.pos.x, ACS_UARROW);


  	      // refresh all
        	wrefresh(wnd);
        	wrefresh(game_wnd);
        	if(exit_requested || game_over) break;
        	tick++;
        	usleep(10000); // 10 ms
    }
}


  void close()
  {
	delwin(wnd);
	delwin(game_wnd);
	endwin();
  }
  int main(int argc,char* argv[])
  {
	int stat=init();
	if(stat==0)
		run();	
	close();
  return 0;
  }

