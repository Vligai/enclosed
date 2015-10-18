NAME1=client
NAME2=server
SRC1=client.c
SRC2=server.c
OBJ1=$(SRC1:.c=.o)
OBJ2=$(SRC2:.c=.o)
RM=rm -f
INCLPATH= ../../include
CFLAGS+=-I$(INCLPATH)
LDFLAGS+= -L../../lib -pedantic-errors
LDLIBS+= -lcrypto -lssl

all:$(NAME1) $(NAME2)

$(NAME1):$(OBJ1)
$(NAME2):$(OBJ2)

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
