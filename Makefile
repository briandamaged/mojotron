OBJS = main.o World.o ScoreBoard.o Thing.o Globals.o Sound.o Intro.o ObjectList.o\
	Player.o PlayerStats.o InputState.o MiscDisplay.o Bonus.o SkillLevel.o\
	Hazards.o Sprite.o Bullet.o Barricade.o Explosion.o Menu.o\
	Demo.o ConfigFile.o

KILLTESTOBJS = main.o killtest.o ScoreBoard.o Thing.o Globals.o Intro.o ObjectList.o\
	Player.o PlayerStats.o InputState.o MiscDisplay.o Bonus.o\
	Hazards.o Sprite.o Bullet.o Barricade.o Explosion.o Menu.o

GLOBTESTOBJS = globtest.o Sprite.o Globals.o

SNDTESTOBJS = sndtest.o Sound.o

WORLDTESTOBJS = worldloadtest.o World.o ScoreBoard.o Thing.o Globals.o Sound.o Intro.o ObjectList.o\
	Player.o PlayerStats.o InputState.o MiscDisplay.o Bonus.o SkillLevel.o\
	Hazards.o Sprite.o Bullet.o Barricade.o Explosion.o Menu.o

LEVLOADTESTOBJS = levloadtest.o World.o ScoreBoard.o Thing.o Globals.o Sound.o Intro.o ObjectList.o\
	Player.o PlayerStats.o InputState.o MiscDisplay.o Bonus.o SkillLevel.o\
	Hazards.o Sprite.o Bullet.o Barricade.o Explosion.o Menu.o Demo.o

REQUIREDLIBS = -lclanCore -lclanDisplay -lclanApp

#CPPFLAGS = -g -pg -Wall $(DEFS)
CPPFLAGS = -Wall -O3 $(DEFS)

# Compile with clanMikMod
DEFS = -DHAVEMIKMOD
SOUNDLIBS = -lclanSound -lclanMikMod

# Compile without clanMikMod
#DEFS = -UHAVEMIKMOD
#SOUNDLIBS = -lclanSound

all: $(OBJS)
	$(CXX) -o mojotron $(OBJS) $(REQUIREDLIBS) $(SOUNDLIBS)


#An old benchmark
#killtest: $(KILLTESTOBJS)
#	$(CXX) -o test -pg -g $(REQUIREDLIBS) $(KILLTESTOBJS)

# Unit test for class Globals
globtest: $(GLOBTESTOBJS)
	$(CXX) -o test $(GLOBTESTOBJS) $(REQUIREDLIBS) $(SOUNDLIBS)

# Unit test for sound
sndtest: $(SNDTESTOBJS)
	$(CXX) -o test $(SNDTESTOBJS) -lclanCore -lclanSound -lclanApp -lclanMikMod -DHAVEMIKMOD

# Crude unit test for class World
worldtest: $(WORLDTESTOBJS)
	$(CXX) -o test $(WORLDTESTOBJS) $(REQUIREDLIBS) $(SOUNDLIBS)

levloadtest: $(LEVLOADTESTOBJS)
	$(CXX) -o test $(LEVLOADTESTOBJS) $(REQUIREDLIBS) $(SOUNDLIBS)

profiled: $(OBJS)
	$(CXX) -o mojotron -pg -g $(OBJS) $(REQUIREDLIBS) $(SOUNDLIBS)

static: $(OBJS)
	#g++ -o mojotron $(OBJS) -Wl,-Bstatic ../miscmojo/libclohfive.a /usr/lib/libmikmod.a -Wl,-Bdynamic /usr/X11R6/lib/libX11.so /usr/X11R6/lib/libXext.so /usr/X11R6/lib/libXxf86vm.a -lz -lpthread
	# completely static
	$(CXX) -o mojotron $(OBJS) -Wl,-Bstatic ../miscmojo/mylib.a ../miscmojo/libherm.a /usr/lib/libmikmod.a /usr/X11R6/lib/libX11.a /usr/X11R6/lib/libXext.a /usr/X11R6/lib/libXxf86vm.a -lz -lpthread

clean:
	-rm -f *.o

