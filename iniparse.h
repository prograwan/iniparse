/*
	Copyright (c) 2020 prograwan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/



#ifndef __INIPARSE__
#define __INIPARSE__
#include "iniconfig.h"

//想要实现完全无依赖运行的配置为:INIJSON=0,INIPORTENABLE=1
//want to zero independent operation need configured to: INIJSON = 0 and INIPORTENABLE = 1




/////////////////////////////config start////////////////////////////////////

//默认开启cjson功能,当不存在cjson时INIJSON必须为0
//cjson is enable default,you can INIJSON = 0 close it
//if INIJSON ==1 ,so , You must provide cjson source code modules
#ifndef INIJSON
#define INIJSON			1	
#endif

//默认使用框架提供的算法，INIPORTENABLE!=0使用文件内置算法结构，可以实现不依赖任何文件
//Default framework provided by the algorithm, INIPORTENABLE == 0 use file built-in algorithm structure, 
//can be achieved is not dependent on any file, you must provide an available algorithm implementation
#ifndef INIPORTENABLE
#define INIPORTENABLE	0	
#endif


///////////////////////////////////config end///////////////////////////////////////////



///////////////////////////////////macro define///////////////////////////////////////////
//string cmp
#define INI_STR_CS(str0 , str)                   (strcmp((char *)str0, str) == 0x00)

//asert satrt
#define INI_ERR_ASSERT_START()                   int __ad__err_code__  = 0

//asert
#define INI_ERR_ASSERT(t,r)                      do{if((t) ==0x00){__ad__err_code__ = (int)(r); goto INI_ERR_OUT;} }while(0)

//asert and run code
#define INI_ERR_ASSERT_CALL(t,r,call)            do{if((t) ==0x00){__ad__err_code__ = (int)(r); call; goto INI_ERR_OUT;} }while(0)

//asert end
#define INI_ERR_ASSERT_END()                     INI_ERR_OUT:

//asert
#define INI_ASSERT(t,r)							do{if((t) ==0x00){__ad__err_code__ = (int)(r); goto INI_ERR_CATCH;} }while(0)

//asert Exception occurs
#define INI_ERR_ASSERT_CATCH()					INI_ASSERT(0,1); INI_ERR_CATCH:do{if(__ad__err_code__>=0)goto INI_ERR_FINALLY;}while(0)

//asert final
#define INI_ERR_ASSERT_FINALLY()					INI_ERR_FINALLY:

//get asert err code
#define INI_ERR_GET_CODE_INT()                   (__ad__err_code__)

//asert return code
#define INI_ERR_ASSERT_RETURN(t)                 return (t)__ad__err_code__

//memory alloc
#ifndef inimalloc
#define inimalloc malloc
#endif
//memory free
#ifndef inifree
#define inifree free
#endif

//string function strtok
#define	ini_strtok(str,rex)	 __ini_ll_strtok(str,-1,1,rex)
#include "math.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

//if defined INIPORTENABLE!=0, use custom implementation
#if !(defined(INIPORTENABLE) && INIPORTENABLE!=0)
#include "vfs_core.h"
#include "vfs_list.h"
#define			__ini_ll_strtok		 __vfs_core_ll_strtok
#define         ini_list_get_item	vfs_list_get_item
#define         ini_list_object		vfs_list_object
#define         ini_list_foreach	vfs_list_foreach
#define         ini_list_foreach_object vfs_list_foreach_object

#define         ini_list_clear	vfs_list_clear
#define         ini_list_del	vfs_list_del
#define         ini_list_push	vfs_list_push
#define         ini_list_head	vfs_list_head
#define         ini_list_join	vfs_list_join
#define         ini_list_reset	vfs_list_reset
#define         ini_list_push_for_data	vfs_list_push_for_data
#define         ini_list_find_at	vfs_list_find_at
#define			ini_list_type vfs_list_type

#else
	// use build-in impl
	typedef	unsigned short	        ushort;
	typedef	unsigned int	        uint;
	typedef	unsigned char	        uchar;
	typedef long long int           int64;
	typedef unsigned long long int	uint64;
	typedef uchar                   uint8;
	typedef ushort                  uint16;
	typedef uint                    uint32;
	typedef char                    int8;
	typedef short int               int16;
	typedef int                     int32;
	typedef struct low_ini_list_type
	{
		unsigned char             *data;
		unsigned int              count;
		struct low_ini_list_type  *next;
	}ini_list_type;

#endif

//memory growth management function
#ifndef ini_correction_memory
#define ini_correction_memory(p,o,n) ini_correction_memory_impl((void **)&p,o,n)
#endif

//ini format out max path len
#ifndef ini_path_maxlen
#define ini_path_maxlen	256
#endif
//item name max len
#ifndef __NAMEMAXLEN
#define __NAMEMAXLEN	64
#endif
/*
//这个版本WINDOWS不支持..
//this version ini_correction_memory at windows system not support , so ini_correction_memory_impl is default impl 
#define ini_correction_memory(old,oldsize,newsize)({\
int s=(oldsize);\
if((oldsize)<(newsize)){\
s=-1;\
char *n=inimalloc((newsize)+4);\
if(n!=NULL){\
memcpy(n,old,(oldsize));inifree(old);s=newsize;old=n;\
}\
}\
s;})*/

#define CFGMAKE	1
#define INIDString(...)                             # __VA_ARGS__
#define _INIString(x)								#x
#define INIString(...)                             INIDString(__VA_ARGS__)
#define cfgdef(x,...) static const char * x##str = INIString(__VA_ARGS__);\
__VA_ARGS__;
#define INIFFMT(x,n)		((int)(x)),((uint)(((x)-((int)(x)))*pow(10,n)))
#define INIFFMTS(mm,mn)		"%0" #mm "d.%0" #mn "d"
#define INIFFMTSTR			INIFFMTS(0,7)
#define INIFFMTVAL(x)		INIFFMT(x,7)
	typedef char cstring;
	typedef enum {
		ISF_REMOVE_ZERO = (1 << 0),//序列化时移除值为0的对象节点..
		ISF_LL_ARRAY = (1 << 24),//当出现数组元素时flag将标定这个选项..
		ISF_LL_FILL_ZERO = (1 << 25),//反序列化时是否强制清空目标内存
		ISF_LL_MARK_HIT = (1 << 26),//反序列化时是否标定命中成功的节点..
	}ini_serialize_flag;
	typedef enum {
		INI_TYPE_CHAR = 1,
		INI_TYPE_SHORT,
		INI_TYPE_INT,
		INI_TYPE_FLOAT,
		INI_TYPE_DOUBLE,
		INI_TYPE_LONG,
		INI_TYPE_LLONG,
		INI_TYPE_STRING,
		INI_TYPE_STRUCT,
		INI_TYPE_ALL = 31,
	}ini_type;
#define INI_TYPE_UNSIGNED	(1<<5)
#define INI_TYPE_ARRAY		(1<<6)

	typedef struct {
		uint   type : 8;//元素数据类型
		uint   offset : 24;//内存偏移位置，单位字节..
		ushort cnt : 8;//元素个数，默认=1，即，int x与 int x[1] 内存布局等效..
		ushort hit : 1;//反序列化时标定此节点释放成功命中且反序列化成功..
		ushort rev : 7;
		ushort size;//元素内存大小
		char *name;//节点名字
		ini_list_type *items;//节点子元素..
	}iniitem;

	typedef union {
		char    strvalue[4];
		char	cvalue;
		uchar	ucvalue;
		short   svalue;
		ushort  usvalue;
		int     ivalue;
		uint    uivalue;
		float   fvalue;
		double  dvalue;
		int64   lvalue;
		uint64  ulvalue;
	}inivalue;
	#ifndef inimake
	#define inimake(x) inimakeforstring(x##str)
	#endif
	int inidel(ini_list_type *dlist);
	void iniprint(ini_list_type *list);
	ini_list_type* inimakeforstring(const char *str);
	char* iniserialize_to_txt(ini_list_type *dslists, const void *handle, uint flag);
	int inideserialize_from_txt(ini_list_type *dslists, const char *msg, void *handle, uint size, int *errcnt, uint flag);

#if defined(INIJSON) && INIJSON!=0
	#include "cjson.h"
	char * iniserialize_to_json(ini_list_type * dslists, const void * handle, uint flag, int *err);
	int   inideserialize_from_json(ini_list_type *dslists, const char *json, void *handle, uint size, int *errcnt, uint flag);
#endif
	int iniparse_test();
//提供一个复杂的测试结构体进行序列化和反序列化..
//使用下面的写法是为了IDE的智能提示能够正常捕获结构体内部的变量定义.

#if defined(__GNUC__) || defined(__ARMCC_VERSION) || defined(__CC_ARM) || defined(__ICCARM__)
#ifndef __TESTMSG__
#define __TESTMSG__

	cfgdef(inimsg, 
#if 0
	)
#endif
		typedef struct {
		cstring info[16];//按照字符串处理..
		char x;
		ushort name;
		struct {
			char y;
			struct {
				double temper;
			}values;
			struct {
				uchar mode;
				short sv;
				int ssd;
				char sd, cc;
				int cf[4];
			}kv, ext[3];
		}data;
		struct {
			int version;
		}ex;
	}inimsg;
#if 0
	(
#else
	);
#endif

#endif


#else//默认提供给不支持__vA_ARGS__参数展开的编译器,比如msvc
#ifndef __TESTMSG__
#define __TESTMSG__
	cfgdef(inimsg, typedef struct {
		cstring info[16];//按照字符串处理..
		char x;
		ushort name;
		struct {
			char y;
			struct {
				double temper;
			}values;
			struct {
				uchar mode;
				short sv;
				int ssd;
				char sd, cc;
				int cf[4];
			}kv, ext[3];
		}data;
		struct {
			int version;
		}ex;
	}inimsg;
);
#endif
#endif
#endif