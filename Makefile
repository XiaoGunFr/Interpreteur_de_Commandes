CC = gcc#Compilateur
CFLAGS = -Wall -D_DEFAULT_SOURCE#Options de compilation
DEPS = *.h
INC = include/
SRC = src/
EXEC = jsh out
PO = command.o signaux.o redirection.o jobs.o auxiliaire.o#fichier .o

all : $(EXEC)

%.o: $(SRC)%.c $(INC)$(DEPS)
	$(CC) $(CFLAGS) -c $<

jsh : jsh.o $(PO)
	$(CC) $(CFLAGS) -o $@ $^ -lreadline -lm

out :
	mkdir out
	mv *.o out/

clean :
	rm -rf $(EXEC) *.o

valgrind :
	valgrind --leak-check=full --show-leak-kinds=definite ./jsh