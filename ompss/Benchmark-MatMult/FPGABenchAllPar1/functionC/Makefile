# Fichier makefile trame
#--------------------------

CC=g++ -g
FLAG= -o
TARGET=prog
FILES=testOmpSs.c fonction.c r_w_file.c random.c generatMatrix.c
OBJECTS=$(FILES:.c=.o)
HFILE=$(FILES:.c=.h)
CLEANING=rm -f $(OBJECTS) $(TARGET) core a.out && reset && ls
EDIT=gedit
#-------------------------------------------------------------

#$(TARGET): $(OBJECTS)
	#$(CC) $(OBJECTS) $(FLAG) $(TARGET) -lm

#-------------------------------------------------------------

#testOmpSs.o: fonction.h testOmpSs.c
#	$(CC) -c testOmpSs.c $(FLAG) testOmpSs.o

fonction.o: fonction.h fonction.c
	$(CC) -c fonction.c $(FLAG) fonction.o

r_w_file.o: r_w_file.h r_w_file.c
	$(CC) -c r_w_file.c $(FLAG) r_w_file.o

generatMatrix.o: generatMatrix.h generatMatrix.c
	$(CC) -c generatMatrix.c $(FLAG) generatMatrix.o

random.o: random.h random.c
	$(CC) -c random.c $(FLAG) random.o


#-------------------------------------------------------------

.c.o :
	$(CC) -c $< -o $*.o

clean:
	$(CLEANING)

edit:
	$(EDIT) $(FILES) $(HFILE) &
