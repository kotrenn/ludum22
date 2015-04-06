OBJS = main.o common.o geometry.o vector2d.o surface.o\
       datamanager.o resourcemanager.o timer.o\
       level.o camera.o actor.o manager.o terrain.o\
       bulletshape.o bullet.o particle.o actorfactory.o\
       decoration.o goal.o hazard.o hazardfactory.o
#FLAGS = -O0 -g -Wall -Wextra
FLAGS = -O3 -Wall -Wextra
LINKS = -lm -lmingw32 -lSDLmain -lSDL -lSDL_image -lSDL_mixer
#LINKS = -lm -lSDL -lSDL_image -lSDL_mixer

varon: ${OBJS}
	g++ -o varon ${OBJS} ${FLAGS} ${LINKS}

.cpp.o: $<
	-g++ ${FLAGS} -c -o $@ $<