#define LEN 255 /* fgets reading length*/


typedef struct s_package {
	char data[8];
	char op;
}package;

int convert_recv (char data[]);
void convert_send(int n, char data[]);
