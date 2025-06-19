# Makefile para compilar o jogo BioGun com Allegro

# -----------------------------------------------------------------------------
# Variáveis de compilação
# -----------------------------------------------------------------------------

# Compilador C
CC = gcc

# Comando pkg-config para obter flags de compilação e linkagem do Allegro
PKG_CONFIG = pkg-config
ALLEGRO_CFLAGS = $(shell $(PKG_CONFIG) allegro-5 allegro_main-5 allegro_font-5 allegro_image-5 allegro_ttf-5 allegro_primitives-5 --cflags)
ALLEGRO_LIBS   = $(shell $(PKG_CONFIG) allegro-5 allegro_main-5 allegro_font-5 allegro_image-5 allegro_ttf-5 allegro_primitives-5 --libs)

# Arquivos-fonte do projeto
SRCS = Biogun.c Enviroments.c Player.c Joystick.c Pistol.c Bullet.c

# Nome do executável de saída
TARGET = BG

# -----------------------------------------------------------------------------
# Alvo padrão: compilar tudo
# -----------------------------------------------------------------------------
all: $(TARGET)

# -----------------------------------------------------------------------------
# Regra para construir o executável
# -----------------------------------------------------------------------------
$(TARGET): $(SRCS)
	# $^: todas as dependências (arquivos-fonte)
	# -o $@: define o nome do executável
	$(CC) $^ -o $@ $(ALLEGRO_CFLAGS) $(ALLEGRO_LIBS)

# -----------------------------------------------------------------------------
# Limpeza dos arquivos gerados
# -----------------------------------------------------------------------------
clean:
	# Remove o executável e objetos (.o)
	rm -f $(TARGET) *.o

# -----------------------------------------------------------------------------
# Declaração de alvos que não são arquivos
# -----------------------------------------------------------------------------
.PHONY: all clean