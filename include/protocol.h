#define SIZE 8 /* fgets reading length*/
#define LEN 255


typedef struct s_package {
	char data[8];
	char op;
}package;

char * convert_send (int a);
int convert_recv (char * b);
