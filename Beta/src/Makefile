NAME1=client
NAME2=server
NAME3=database
SRC1=client.c
SRC2=server.c
SRC3=database.c
OBJ1=$(SRC1:.c=.o)
OBJ2=$(SRC2:.c=.o)
OBJ3=$(SRC3:.c=.o)
RM=rm -f
INCLPATH= ../include
CFLAGS+=-I$(INCLPATH)
LDFLAGS+= -L../lib
LDLIBS+= -lcrypto -lssl -Wall -pedantic-errors

all:$(NAME1) $(NAME2) $(NAME3)

$(NAME1):$(OBJ1)
$(NAME2):$(OBJ2)
$(NAME3):$(OBJ3)

client:

$(NAME1):$(OBJ1)

server:

$(NAME2):$(OBJ2)

clean:
	-$(RM) *~
	-$(RM) *.o
	-$(RM) *.core
	-$(RM) *.swp
	-$(RM) \#*

fclean:clean all

re:fclean all
