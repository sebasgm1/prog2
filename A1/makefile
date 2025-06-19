# makefile A1
# Sebastián Galvis Moreno

CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -g -std=gnu99
LDLIBS  = -lm
MAIN    = main
ENTREGA = $(MAIN)

# lista de arquivos de cabeçalho
HDR = lz.h vinac.h

# lista de arquivos-objeto
OBJ = lz.o vinac.o

# construir o executável
$(MAIN): $(MAIN).o $(OBJ)
	$(CC) $(CFLAGS) -o $(MAIN) $(MAIN).o $(OBJ) $(LDLIBS)

# construir os arquivos-objeto
$(MAIN).o: $(MAIN).c $(HDR)
	$(CC) $(CFLAGS) -c $(MAIN).c

lz.o: lz.c lz.h
	$(CC) $(CFLAGS) -c lz.c

vinac.o: vinac.c vinac.h
	$(CC) $(CFLAGS) -c vinac.c

# executar
run: $(MAIN)
	./$(MAIN)

# testar no Valgrind
valgrind: $(MAIN)
	valgrind --leak-check=full --track-origins=yes ./$(MAIN)

# gerar arquivo TGZ para entregar
tgz: clean
	-mkdir -p /tmp/$(USER)/$(ENTREGA)
	chmod 0700 /tmp/$(USER)/$(ENTREGA)
	cp *.c *.h *.o makefile /tmp/$(USER)/$(ENTREGA)
	tar czvf $(ENTREGA).tgz -C /tmp/$(USER) $(ENTREGA)
	rm -rf /tmp/$(USER)
	@echo "Arquivo $(ENTREGA).tgz criado para entrega"

# limpar arquivos temporários
clean:
	rm -f *~ $(OBJ) $(MAIN) $(MAIN).o /tmp/$(USER)/$(ENTREGA) $(ENTREGA).tgz
