CC = gcc
SRC = main.c player.c blocks.c editor.c enemy.c 
OBJ = main.exe

INCLUDE_PATH = -Iinclude
LIBRARY_PATH = -Llib

LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

debug:
	$(CC) $(SRC) -o $(OBJ) $(INCLUDE_PATH) $(LIBRARY_PATH) $(LIBS) -g

release:
	$(CC) $(SRC) -o $(OBJ) $(INCLUDE_PATH) $(LIBRARY_PATH) $(LIBS) -s -O2 -mwindows

clean:
	del $(OBJ)