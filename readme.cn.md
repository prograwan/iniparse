
# iniparse是什么?
    
这是一个C/c++结构序列化/反序列化框架,它的使用非常简单,高效,不添加任何心理负担。

# iniparse有哪些功能？

* 提供结构体的序列化和反序列化功能
* 提供可选择的结构体序列化到cjson/cjson反序列化到结构体功能
* 提供可配置的内存管理和算法实现
* 提供结构体运行时元信息，为结构体反射提供基础

# 有问题反馈
在使用中有任何问题，欢迎反馈给我，可以用以下联系方式跟我交流

* 邮件(431159416@qq.com)


# 配置与移植
* iniconfig.h是一个为用户准备的空文件，当用户需要自定义某些实现时可以修改此文件
* INIJSON宏:默认==1,此时必须提供cjson源代码（因为本项目此时依赖他，否则请定义INIJSON=0）
* INIPORTENABLE宏:默认==0,代表使用iniparse内置的实现算法，当INIPORTENABLE!=0时用户必须提供算法实现


# 测试用例
* exp1: 在iniparse。c文件中存在一个iniparse_test函数是一个非常复杂的综合测试用例
* 
* exp2: 
```exp2
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
```