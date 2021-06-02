This is a C/C + + structure serialization/deserialization framework, its use is very simple and efficient, does not add any mental burden.

exp1:
	at iniparse.c::iniparse_test function is a very complicated comprehensive test cases.
	
exp2:
	#include"iniparse.h"
	//define struct inimsg2
	#ifndef INIMSG2
		#define INIMSG2
		cfgdef(inimsg2,
	#else
		)
	#endif
	typedef struct {
		cstring info[16];//string type
		char x;
		char d[8];		//char array;
		ushort name;
		struct {
			int id;
			int key;
		}ids[2];
	}inimsg2;
	);
	
	//init msg
	inimsg2 msg = {
		.x = 2,
	},descmsg={0};
	msg.d[1] = 2;
	msg.ids[0].id = 1;
	
	//get inimsg2 runtine infos
	ini_list_type *dlist = inimake(inimsg2);
	//serialize msg
	char * buf = iniserialize_to_txt(dlist, &msg, ISF_REMOVE_ZERO);
	//ok ?
	if (buf != NULL)
	{
		//show
		printf("serialize:\r\n%s", buf);
		int len = strlen(buf);
		if (len > 0) {
			//deserialization
			len = inideserialize_from_txt(dlist, buf, &descmsg, sizeof(descmsg), NULL, ISF_LL_MARK_HIT);
			int c = memcmp(&msg, &descmsg, sizeof(descmsg));
			//show deserialization and msg is same
			printf("\r\ndeserialize result=%s , c = %d\r\n", c == 0 ? "success" : "failed", c);
		}
		inifree(buf);//free mem
	}
	inidel(dlist);//del runtine info