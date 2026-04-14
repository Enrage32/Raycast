CC = gcc

TARGET = raycast

INCLUDE_DIRECTORIES = -I"vendors/raylib/include"
LIBRARY = -L"vendors/raylib/lib" -lraylib -lopengl32 -lgdi32 -lwinmm

debug:
	$(CC) $(INCLUDE_DIRECTORIES) -O0 main.c $(LIBRARY) -o $(TARGET)

clean:
	del *.exe