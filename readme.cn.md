这是一个C/c++结构序列化/反序列化框架,它的使用非常简单,高效,不添加任何心理负担。


测试用例1:
		在iniparse。c文件中存在一个iniparse_test函数是一个非常复杂的综合测试用例
	
测试用例2:
	#include"iniparse.h"
	
	//定义一个结构体
	//使用ifndef等方式包裹cfgdef宏是为了ide能够正常扫描到inimsg2的成员结构，进行智能提示。
	#ifndef INIMSG2
		#define INIMSG2
		cfgdef(inimsg2,
	#else
		)
	#endif
	
		typedef struct {
			cstring info[16];//字符串类型
			char x;
			char d[8];		//字符数组类型，与字符串不是同类型，需注意。
			ushort name;
			struct {
				int id;
				int key;
			}ids[2];
		}inimsg2;
	);
	
	//初始化结构体
	inimsg2 msg = {
		.x = 2,
	},descmsg={0};
	//设置结构体的值
	msg.d[1] = 2;
	msg.ids[0].id = 1;
	
	//获取结构体inimsg2这个类型的运行时信息
	ini_list_type *dlist = inimake(inimsg2);
	//序列化msg
	char * buf = iniserialize_to_txt(dlist, &msg, ISF_REMOVE_ZERO);
	//确认序列化是否成功,buf不为空代表序列化成功了。
	if (buf != NULL)
	{
		//显示序列化后的字符串内容
		printf("serialize:\r\n%s", buf);
		int len = strlen(buf);
		if (len > 0) {
			//进行反序列化
			len = inideserialize_from_txt(dlist, buf, &descmsg, sizeof(descmsg), NULL, ISF_LL_MARK_HIT);//标记反序列化命中的节点，在dlist内
			int c = memcmp(&msg, &descmsg, sizeof(descmsg));//内存比较一下，看反序列化的descmsg是否和原来的msg相同
			//显示结果
			printf("\r\ndeserialize result=%s , c = %d\r\n", c == 0 ? "success" : "failed", c);
		}
		//释放内存
		inifree(buf);//free mem
	}
	//删除运行时信息,如果涉及到大量反复的序列化和反序列化时，可保存此信息，加速序列化工作.
	inidel(dlist);//del runtine info