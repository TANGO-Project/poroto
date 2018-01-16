# Fichier makefile trame
#--------------------------

CC=gcc 
FLAG= -o
TARGET=parsefile
FILES= parseFile.c main.c
OBJECTS=$(FILES:.c=.o)
HFILE=$(FILES:.c=.h)
CLEANING=rm -f $(OBJECTS) $(TARGET) core a.out && reset && ls
EDIT=gedit
#-------------------------------------------------------------

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(FLAG) $(TARGET) -lm

#-------------------------------------------------------------

main.o: parseFile.h main.c
	$(CC) -c main.c $(FLAG) main.o

parseFile.o: parseFile.h parseFile.c
	$(CC) -c parseFile.c $(FLAG) parseFile.o

#-------------------------------------------------------------

.c.o :
	$(CC) -c $< -o $*.o

clean:
	$(CLEANING)

edit:
	$(EDIT) $(FILES) $(HFILE) &
