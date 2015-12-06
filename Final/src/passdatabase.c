#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_DATA 512
#define MAX_ROWS 1000

struct Passwords {
        int id;
        int set;
        char account[MAX_DATA];
        char username[MAX_DATA];
        char password[MAX_DATA];
};

struct Databasep {
        struct Passwords rows[MAX_ROWS];
};
struct Connectionp {
        FILE *filep;
        struct Databasep *db;
};
void Passwords_print(struct Passwords *pass)
{
        printf("%s %s\n",
                pass->username, pass->password);
}

void Passwords_print1(struct Passwords *pass,const char *account)
{
	const char *find = pass->account;
	if (strcmp((const char*)account,(const char*)find)==0){
        printf("%s %s\n",
                pass->username, pass->password);
}
}

void die(const char *message)
{
        if(errno) {
                perror(message);
        } else {
                printf("ERROR: %s\n", message);
        }

        exit(1);
}

void Database_loadp(struct Connectionp *conn)
{
        int rc = fread(conn->db, sizeof(struct Databasep), 1, conn->filep);
        if(rc != 1) die("Failed to load database.");
}

struct Connectionp *Database_openp(const char *filenamep, char mode)
{
        struct Connectionp *conn = malloc(sizeof(struct Connectionp));
        if(!conn) die("Memory error");

        conn->db = malloc(sizeof(struct Databasep));
        if(!conn->db) die("Memory error");

        if(mode == 'c') {
                conn->filep = fopen(filenamep, "w");
        }else {
                conn->filep = fopen(filenamep, "r+");

                if(conn->filep) {
                        Database_loadp(conn);
                }
        }

        if(!conn-> filep) die("Failed to open the file");

        return conn;
}

void Database_closep(struct Connectionp *conn)
{
        if(conn) {
                if(conn->filep) fclose(conn->filep);
                if(conn->db) free(conn->db);
                free(conn);
        }
}
void Database_writep(struct Connectionp *conn)
{
        rewind(conn->filep);

        int rc = fwrite(conn->db, sizeof(struct Databasep), 1, conn->filep);
        if(rc != 1) die("failed to write database.");

        rc = fflush(conn->filep);
        if(rc == -1) die("Cannot flush database.");
}

void Database_createp(struct Connectionp *conn)
{
        int i = 0;

        for(i = 0; i < MAX_ROWS; i++) {
                //Initizalize Database
                struct Passwords pass = {.id = i, .set = 0};
                // Assign database
                conn->db->rows[i] = pass;
        }
}
void Database_setp(struct Connectionp *conn, int id, const char *account, const char *username, const char *password)
{

        struct Passwords *pass = &conn->db->rows[id];

      /*if(user->set) 
        {
                id++;
        }
*/
        pass->set = 1;
 
        char *res = strncpy(pass->account, account, MAX_DATA);
        if(!res) die("Account copy failed");

        res = strncpy(pass->username, username, MAX_DATA);
        if(!res) die("Username copy failed");

        res = strncpy(pass->password, password, MAX_DATA);
        if(!res) die("Password copy failed");
}
void Database_getp(struct Connectionp *conn, int id)
{
        struct Passwords *pass = &conn->db->rows[id];

        if(pass->set) {
                Passwords_print(pass);
        } else {
                die("ID is not set");
        }
}


void Database_deletep(struct Connectionp *conn, int id)
{
        struct Passwords pass = {.id = id, .set = 0};
        conn->db->rows[id] = pass;
}

void Database_listp(struct Connectionp *conn)
{
        int i = 0;
        struct Databasep *db = conn->db;

        for(i = 0; i < MAX_ROWS; i++) {
                struct Passwords *cur = &db->rows[i];

                if(cur->set) {
                        Passwords_print(cur);
                }
        }
}


void Database_listp1(struct Connectionp *conn,const char *account, char *result)
{
        int i = 0;
        struct Databasep *db = conn->db;

        for(i = 0; i < MAX_ROWS; i++) {
                struct Passwords *cur = &db->rows[i];

                if(cur->set) {
                               const char *find = cur->account;
        		if (strcmp((const char*)account,(const char*)find)==0){
        	//	printf("%s %s\n",
                //cur->username, cur->password);
			result = cur->account;
                }
        }
}
}
int main(int argc, char *argv[])
{
        if(argc < 3) die("USAGE: Database <dbfile> <action> [action params]");
	char *result;
        char *filenamep = argv[1];
        char action = argv[2][0];
        struct Connectionp *conn = Database_openp(filenamep, action);
	const char *account = argv[3];
	int id =0;
       //if(argc > 3) id = atoi(argv[3]);
        if(id >= MAX_ROWS) die("There's not that many records.");

        switch(action) {
                case 'c':
                        Database_createp(conn);
                        Database_writep(conn);
                        break;

                case 'g':
                        if(argc != 4) die("Need an id to get");
			  int i = 0;
        			struct Databasep *db = conn->db;

        			for(i = 0; i < MAX_ROWS; i++) {
                		struct Passwords *cur = &db->rows[i];

                		if(cur->set) {
                               		const char *find = cur->account;
                        		if (strcmp((const char*)account,(const char*)find)==0){
                //      printf("%s %s\n",
                //cur->username, cur->password);
                    		    result = cur->password;
                }
        }
}
			puts(result);
                        //Database_getp(conn, id);
                        break;
                case 's':
                        if(argc != 6) die("Need id, username, password to set");

                        Database_setp(conn, id, "facebook", argv[4], argv[5]);
                        Database_writep(conn);
                        break;
                case 'd':
                        if(argc != 4) die("Need id to delete");

                        Database_deletep(conn, id);
                        Database_writep(conn);
                        break;

                case 'l':
                        Database_listp(conn);
                        break;
                default:
                        die("Invalid action, only c=create, g=get, s=set, d=del, l=list");
        }
        Database_closep(conn);

        return 0;
}
