# Emergency Drone Coordination System - Makefile

# Derleyici ve ayarları
CC = gcc
CFLAGS = -Wall -Wextra -Iheaders -g
LDFLAGS = -lSDL2 -lm -lpthread

# Ana kaynak dosyalar (main programı için)
SRC = ai.c controller.c drone.c list.c map.c server.c survivor.c view.c
OBJ = $(SRC:.c=.o)

# Varsayılan hedef: hem sunucu hem istemciyi derle
all: main drone_client

# Sunucu (koordinatör) uygulaması
main: $(OBJ)
	$(CC) $(CFLAGS) -o main $(OBJ) $(LDFLAGS)

# İstemci uygulaması
drone_client: drone_client/drone_client.c
	$(CC) $(CFLAGS) -o drone_client/drone_client drone_client/drone_client.c $(LDFLAGS)

# Temizlik komutu
clean:
	rm -f *.o main drone_client/drone_client
