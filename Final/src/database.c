
#include <stdio.h> 
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_DATA 512
#define MAX_ROWS 1000

//#include "enclosed.h"

struct Users {
        int id;
        int set;
        char username[MAX_DATA];
        char password[MAX_DATA];
};

struct Database {
        struct Users rows[MAX_ROWS];
};

struct Connection {
        FILE *file;
        struct Database *db;
};

void die(const char *message)
{
        if(errno) {
                perror(message);
        } else {
                printf("ERROR: %s\n", message);
        }

        exit(1);
}

void Users_print(struct Users *user)
{
        printf("%d %s %s\n",
                user->id, user->username, user->password);
}
void Database_load(struct Connection *conn)
{
        int rc = fread(conn->db, sizeof(struct Database), 1, conn->file);
        if(rc != 1) die("Failed to load database.");
}

struct Connection *Database_open(const char *filename, char mode)
{
        struct Connection *conn = malloc(sizeof(struct Connection));
        if(!conn) die("Memory error");

        conn->db = malloc(sizeof(struct Database));
        if(!conn->db) die("Memory error");

        if(mode == 'c') {
                conn->file = fopen(filename, "w");
        }else {
                conn->file = fopen(filename, "r+");

                if(conn->file) {
                        Database_load(conn);
                }
        }

        if(!conn-> file) die("Failed to open the file");

        return conn;
}

void Database_close(struct Connection *conn)
{
        if(conn) {
                if(conn->file) fclose(conn->file);
                if(conn->db) free(conn->db);
                free(conn);
        }
}
void Database_write(struct Connection *conn)
{
        rewind(conn->file);

        int rc = fwrite(conn->db, sizeof(struct Database), 1, conn->file);
        if(rc != 1) die("failed to write database.");

        rc = fflush(conn->file);
        if(rc == -1) die("Cannot flush database.");
}

void Database_create(struct Connection *conn)
{
        int i = 0;

        for(i = 0; i < MAX_ROWS; i++) {
                //Initizalize Database
                struct Users user = {.id = i, .set = 0};
                // Assign database
                conn->db->rows[i] = user;
        }
}

void Database_set(struct Connection *conn, int id, const char *username, const char *password)
{
        struct Users *user = &conn->db->rows[id];
        if(user->set) die("Already set, delete it first");

        user->set = 1;
        //we have to find a better way to do this rather than strncpy
        char *res = strncpy(user->username, username, MAX_DATA);
        if(!res) die("Username copy failed");

        res = strncpy(user->password, password, MAX_DATA);
        if(!res) die("Password copy failed");
}
void Database_get(struct Connection *conn, int id)
{
        struct Users *user = &conn->db->rows[id];

        if(user->set) {
                Users_print(user);
        } else {
                die("ID is not set");
        }
}

void Database_delete(struct Connection *conn, int id)
{
        struct Users user = {.id = id, .set = 0};
        conn->db->rows[id] = user;
}

void Database_list(struct Connection *conn)
{
        int i = 0;
        struct Database *db = conn->db;

        for(i = 0; i < MAX_ROWS; i++) {
                struct Users *cur = &db->rows[i];

                if(cur->set) {
                        Users_print(cur);
                }
        }
}

int main(int argc, char *argv[])
{
        if(argc < 3) die("USAGE: Database <dbfile> <action> [action params]");

        char *filename = argv[1];
        char action = argv[2][0];
        struct Connection *conn = Database_open(filename, action);
        int id = 0;

        if(argc > 3) id = atoi(argv[3]);
        if(id >= MAX_ROWS) die("There's not that many records.");

        switch(action) {
                case 'c':
                        Database_create(conn);
                        Database_write(conn);
                        break;

                case 'g':
                        if(argc != 4) die("Need an id to get");

                        Database_get(conn, id);
                        break;
                case 's':
                        if(argc != 6) die("Need id, username, password to set");

                        Database_set(conn, id, argv[4], argv[5]);
                        Database_write(conn);
                        break;
                case 'd':
                        if(argc != 4) die("Need id to delete");

                        Database_delete(conn, id);
                        Database_write(conn);
                        break;

                case 'l':
                        Database_list(conn);
                        break;
                default:
                        die("Invalid action, only c=create, g=get, s=set, d=del, l=list");
        }
        Database_close(conn);

        return 0;
}

