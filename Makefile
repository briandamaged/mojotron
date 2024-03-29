OBJS = main.o World.o ScoreBoard.o Thing.o Globals.o Sound.o Intro.o ObjectList.o\
	Player.o PlayerStats.o InputState.o MiscDisplay.o Bonus.o SkillLevel.o\
	Hazards.o Sprite.o Bullet.o Barricade.o Explosion.o Menu.o\
	Demo.o ConfigFile.o expatcpp.o SDL_CL_ResourceManager.o SDL_CL_Surface.o\
	IniFile.o Ratings.o IRandom.o

REQUIREDLIBS =  `pkg-config --libs sdl2 SDL2_image SDL2_mixer expat gamerzilla`

#CPPFLAGS = -g -pg -Wall $(DEFS)
CPPFLAGS = -Wall -O3 $(DEFS) -DHAVE_GAMERZILLA `pkg-config --cflags sdl2 SDL2_image SDL2_mixer expat gamerzilla`

all: $(OBJS)
	$(CXX) -o mojotron $(OBJS) $(REQUIREDLIBS) $(SOUNDLIBS)

clean:
	-rm -f *.o

