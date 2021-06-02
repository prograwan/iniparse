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



#include "iniparse.h"
#define __NAME_OFFSET_X(x)	(((x)->type&INI_TYPE_STRUCT)==INI_TYPE_STRUCT?1:0)

//************************************
// Method:    ini_correction_memory_impl
// FullName:  ini_correction_memory_impl
// Access:    public 
// Returns:   int
// Qualifier: 满足指定内存需求
// Parameter: void * * ptr
// Parameter: int oldsize
// Parameter: int newsize
//************************************
int ini_correction_memory_impl(void**ptr, int oldsize, int newsize) {

	int s = (oldsize);
	char *old = (char *)*ptr;
	if ((oldsize) < (newsize)) {

		s = -1;
		char *n = (char *)inimalloc((newsize)+4);
		if (n != 0) {

			memcpy(n, old, (oldsize));
			inifree(old);
			s = newsize;
			*ptr = n;
		}
	}
	return s;
}

//使用默认内置提供的算法结构
#if (defined(INIPORTENABLE) && INIPORTENABLE!=0)
//获取list指定索引项
#define         ini_list_get_item(head,index)  ((ini_list_type*)({int z = 0x00,aindex=index;if(aindex<0){aindex=(head)->count-((aindex*(-1))%((head)->count));}ini_list_type *item = head;for(;z < aindex; z++) {item = item->next;}item;}))
//获取list指定索引项指向的内存地址
#define         ini_list_object(head,index,type)    ((type)(ini_list_get_item(head,index)->data))     
//循环访问list列表,v是迭代器，类型是ini_list_type*
#define         ini_list_foreach(list,v)        for(ini_list_type *v = list->next;list&&v;v=v->next)    
//循环访问list列表,type指定目标指定的类型,v是目标类型迭代器
#define         ini_list_foreach_object(list,type,v)        for(type v = (type)((list)->next->data),*v##__temp = (type)((list)->next);(list)&&(list)->count>0&&v##__temp;\
                                                                v##__temp=(v##__temp==NULL)?NULL:((type)(((ini_list_type*)v##__temp)->next)),v=(v##__temp==NULL?NULL:((type)(((ini_list_type*)v##__temp)->data))))

//************************************
// Method:    ini_list_clear
// FullName:  ini_list_clear
// Access:    public 
// Returns:   int
// Qualifier: 清除列表项，并释放data指向的内存（如果不为空）
// Parameter: ini_list_type * _list
//************************************
int ini_list_clear(ini_list_type *_list)
{
	ini_list_type * node = NULL; if (_list == NULL) { return 0; }
	if (_list->count == 0x00) { _list->next = NULL; return 1; }
	while (_list->next != NULL) {
		node = _list->next;
		_list->next = node->next;
		if (node->data) { inifree(node->data); }
		inifree(node);
	}
	_list->count = 0x00;
	return 1;
}
//************************************
// Method:    ini_list_del
// FullName:  ini_list_del
// Access:    public 
// Returns:   int
// Qualifier: 清除列表项，并释放data指向的内存（如果不为空），释放list节点内存
// Parameter: ini_list_type * _list
//************************************
int ini_list_del(ini_list_type *_list) { if (_list == NULL)return 0x00; ini_list_clear(_list); return 1; }
//************************************
// Method:    ini_list_push
// FullName:  ini_list_push
// Access:    public 
// Returns:   int
// Qualifier: 创建一个节点追加到list最后，并将节点内存指向data内存，标记此节点data指向的内存大小未count
// Parameter: ini_list_type * _list
// Parameter: const void * data
// Parameter: unsigned int count
//************************************
int ini_list_push(ini_list_type *_list, const void * data, unsigned int count)
{
	ini_list_type *node = NULL;
	ini_list_type *add_node = NULL;
	if (_list == NULL)return 0;
	if (_list->count != 0x00) {
		node = _list->next;
		while (node->next != NULL)
		{
			node = node->next;
		}
	}
	else
		node = _list;
	add_node = (ini_list_type *)inimalloc(sizeof(ini_list_type));
	if (add_node == NULL)
	{
		return (char)-1;

	}add_node->next = NULL;
	add_node->count = count;
	add_node->data = (uchar*)data;
	_list->count += 0x01;
	if (node == NULL)//队列空
		_list->next = add_node;
	else
		node->next = add_node;
	return 1;

}
//************************************
// Method:    ini_list_head
// FullName:  ini_list_head
// Access:    public 
// Returns:   ini_list_type *
// Qualifier: 创建一个list节点头
//************************************
ini_list_type * ini_list_head() {
	int sc = sizeof(ini_list_type);//默认的列表使用统一的资源锁，用不到扩展字段部分..
	ini_list_type * inode = (ini_list_type *)inimalloc(sc);
	memset(inode, 0x00, sc);
	return (ini_list_type*)inode;
}
//************************************
// Method:    ini_list_join
// FullName:  ini_list_join
// Access:    public 
// Returns:   int
// Qualifier: 合并data列表至list列表..
// Parameter: ini_list_type * list
// Parameter: ini_list_type * data
//************************************
int ini_list_join(ini_list_type *list, ini_list_type *data)
{
	if (data == NULL || list == NULL)
		return -1;
	ini_list_foreach_object(data, void*, item) {
		ini_list_push(list, item, ((ini_list_type*)item__temp)->count);
	}
	return data->count;
}
//************************************
// Method:    ini_list_reset
// FullName:  ini_list_reset
// Access:    public 
// Returns:   int
// Qualifier: 清空列表，但不释放data指向的内存空间
// Parameter: ini_list_type * _list
//************************************
int ini_list_reset(ini_list_type *_list)
{
	ini_list_type * node = NULL;
	if (_list == NULL)
		return 0;
	if (_list->count == 0x00)
	{
		_list->next = NULL;
		return 1;
	}
	while (_list->next != NULL)
	{
		node = _list->next;
		_list->next = node->next;
		inifree(node);
	}
	_list->count = 0x00;
	return 1;
}
//************************************
// Method:    ini_list_push_for_data
// FullName:  ini_list_push_for_data
// Access:    public 
// Returns:   int
// Qualifier: 加入一个节点到list最后（使用拷贝副本的方式）
// Parameter: ini_list_type * _list
// Parameter: const void * data 引用的目标
// Parameter: unsigned int offset 相对目标的内存偏移
// Parameter: unsigned int len	拷贝的长度（字节）
//************************************
int ini_list_push_for_data(ini_list_type *_list, const void  *data, unsigned int offset, unsigned int len)
{
	uchar *item = NULL;
	if (_list == NULL || data == NULL)return -1;

	item = (uchar*)inimalloc(len);
	if (item == NULL)return -2;
	memcpy((void*)item, (void*)((char*)data + offset), (int)len);
	if (ini_list_push(_list, item, len) < 0x00)
	{
		inifree(item); return -3;
	}
	return 0x01;
}
//************************************
// Method:    ini_list_find_at
// FullName:  ini_list_find_at
// Access:    public 
// Returns:   ini_list_type *
// Qualifier: 查找指定索引的列表节点
// Parameter: ini_list_type * _list
// Parameter: int index
//************************************
ini_list_type * ini_list_find_at(ini_list_type *_list, int index)
{
	ini_list_type * node = NULL;
	if (_list == NULL)
		return NULL;
	if (index < 0) {
		index = _list->count - ((index*(-1)) % _list->count);
	}
	if (_list->next == NULL)
	{
		return NULL;
	}
	if (index >= (int)_list->count)
		index = _list->count - 1;

	node = _list->next;
	while (index > 0x00)
	{
		index -= 0x01;
		node = node->next;
	}
	return node;
}

int __ini_strtok_fix(const char * str, int lindex, int findex, int i, char **re) {

	int hlen = 0;
	int len = i - lindex - 1, start = lindex + 1;
	if (len <= 0 || re == NULL || str == NULL)
		return -1;

	char *item = (char *)inimalloc(len + 1);
	if (item == NULL)
		return -2;
	for (int j = 0; j < len; j++)
	{
		if (j == (findex - lindex - 1) || (str[j + start] == '\\' && str[j + start + 1] == '\"'))
			continue;//这是开始的位置..
		else if (j > 0 && str[j + start] == '\"' && str[j + start - 1] != '\\') {
			continue;//这一般是"的开头he结尾..
		}
		//这才是合法的数据字符..
		item[hlen++] = str[j + start];
	}
	item[hlen] = 0x00;
	len = hlen + 1;

	if (len > 0 && item && re) {
		*re = item;
		return len;
	}
	inifree(item);
	return -3;
}
//************************************
// Method:    __ini_ll_strtok
// FullName:  __ini_ll_strtok
// Access:    public 
// Returns:   ini_list_type *
// Qualifier: 分割字符串，以指定字符集合分割
// Parameter: const char * str	待分割字符串
// Parameter: int _len	str能够进行分割的最大长度，=-1时分割整个字符串否则分割指定的部分
// Parameter: uint isfilterempty 是否过滤空
// Parameter: const char * rex 分割字符集合
//************************************
ini_list_type * __ini_ll_strtok(const char * str, int _len, uint isfilterempty, const char *rex) {
	ini_list_type * list = ini_list_head();
	INI_ERR_ASSERT_START();

	INI_ASSERT(list && str && rex, -1);
	int l = _len, lindex = -1, size = 0, len = 0, findex = -1, hlen = 0;
	if (l < 0)
		l = strlen(str);
	char *x = NULL, *item = NULL, *t = NULL;
	for (int i = 0; i < l; i++)
	{
		if (str[i] == '\"') {
			if (i > 0 && str[i - 1] != '\\') {
				size++;
				if (findex < 0)
					findex = i;//记录第一个"的位置..
			}
		}
		if (size % 2 == 1)
			continue;//字串内部..不用匹配..
		if ((x = (char *)strchr(rex, str[i])) == NULL)
			continue;
		if (i - 1 == lindex) {//连续命中
			lindex++;
			if (isfilterempty == 0) {
				//加入一个空串到列表
				char *np = (char *)inimalloc(1);
				if (np) {
					np[0] = 0x00;
					ini_list_push(list, np, 1);
				}
			}
			continue;
		}
		//那么lindex+1<---------->i-1就是一个新的字符串..
		//加入列表..
		len = i - lindex - 1;
		if (len > 0)
		{
			item = NULL;
			len = __ini_strtok_fix(str, lindex, findex, i, &item);
			if (item != NULL && len > 0)
				ini_list_push(list, item, len);
			item = NULL;
		}
		findex = -1;
		lindex = i;
	}
	len = l - lindex - 1;
	if (len > 0)
	{
		item = NULL;
		len = __ini_strtok_fix(str, lindex, findex, l, &item);
		if (item != NULL && len > 0)
			ini_list_push(list, item, len);
		item = NULL;
	}
	INI_ERR_ASSERT_CATCH();
	//进行异常处理..
	if (list) {
		ini_list_del(list);
		list = NULL;
	}
	INI_ERR_ASSERT_FINALLY();

	return list;
}



#endif


//************************************
// Method:    inidel
// FullName:  inidel
// Access:    public 
// Returns:   int
// Qualifier: 删除并释放运行时列表
// Parameter: ini_list_type * dlist
//************************************
int inidel(ini_list_type *dlist) {
	if (dlist == NULL)
		return 0;
	ini_list_foreach_object(dlist, iniitem*, x) {
		if ((x->type & INI_TYPE_ALL) == INI_TYPE_STRUCT) {
			inidel(x->items);
		}
		inifree(x);
		((ini_list_type*)x__temp)->data = NULL;//已经提前释放了内存..
	}
	ini_list_del(dlist);
	return 1;
}

//************************************
// Method:    initostring
// FullName:  initostring
// Access:    public 
// Returns:   int
// Qualifier: 将目标内存以指定类型转换成字符串
// Parameter: iniitem * x 类型信息
// Parameter: void * ptr 目标内存指针
// Parameter: char * buf 字符串存放指针
//************************************
int initostring(iniitem *x, void *ptr, char *buf) {
	//将器转换成字符串格式..
	int re = 0;
	switch (x->type&INI_TYPE_ALL)
	{
	case INI_TYPE_CHAR: {
		re = sprintf(buf, "%d", (x->type&INI_TYPE_UNSIGNED) != 0 ? (*((uchar*)ptr)) : (*((char*)ptr)));
	}break;
	case INI_TYPE_SHORT: {
		re = sprintf(buf, "%d", (x->type&INI_TYPE_UNSIGNED) != 0 ? (*((ushort*)ptr)) : (*((short*)ptr)));
	}break;
	case INI_TYPE_INT: {
		re = sprintf(buf, "%d", (x->type&INI_TYPE_UNSIGNED) != 0 ? (*((uint*)ptr)) : (*((int*)ptr)));
	}break;
	case INI_TYPE_FLOAT: {
		re = sprintf(buf, INIFFMTSTR, INIFFMTVAL(*((float*)ptr)));
	}break;
	case INI_TYPE_DOUBLE: {
		re = sprintf(buf, INIFFMTSTR, INIFFMTVAL((float)(*((double*)ptr))));//目前没有好的办法进行序列化，只能使用float代替
	}break;
	case INI_TYPE_LONG: {
		re = sprintf(buf, "%ld", *((long*)ptr));
	}break;
	case INI_TYPE_STRING: {
		re = strlen((char*)ptr);
		if (re <= 0)
			return re;
		//此处需要转移防止字符串中携带了转义字符'\'
		if (strstr((char*)ptr, "\n") == NULL) {
			memcpy(buf, ptr, re);
			return re;
		}
		//需要进行转义，否则存储后会出现歧义..
		char *str = (char *)ptr;
		int al = 0;
		for (int i = 0, j = 0; j < re; i++, j++)
		{
			if (str[j] == '\n' || str[j] == '\r' || str[j] == '\\') {
				buf[i++] = '\\';//隐藏转义..
				al++;
				if (str[j] == '\n')
					buf[i] = 'n';
				else if (str[j] == '\r')
					buf[i] = 'r';
				else if (str[j] == '\\')
					buf[i] = '\\';
				else
					al--;
				continue;
			}
			buf[i] = str[j];
		}
		return re + al;
	}break;
	default:return -1;//不支持的类型..
	}

	return re;
}
//************************************
// Method:    inifiledgetsize
// FullName:  inifiledgetsize
// Access:    public 
// Returns:   int
// Qualifier: 获取基本数据类型的内存大小
// Parameter: int type
//************************************
int inifiledgetsize(int type) {
	type &= INI_TYPE_ALL;
	switch (type)
	{
	case INI_TYPE_CHAR:return 1;
	case INI_TYPE_SHORT:return 2;
	case INI_TYPE_INT:return 4;
	case INI_TYPE_FLOAT:return 4;
	case INI_TYPE_DOUBLE:return 8;
	case INI_TYPE_LONG:return 8;
	case INI_TYPE_LLONG:return 8;
	case INI_TYPE_STRING:return 1;
	case INI_TYPE_STRUCT:return -2;
	}
	return -3;
}
//************************************
// Method:    initypescan
// FullName:  initypescan
// Access:    public 
// Returns:   ini_type
// Qualifier: 扫描获取数据类型，并返回枚举
// Parameter: const char * s
//************************************
ini_type initypescan(const char *s) {
	ini_type re = INI_TYPE_ALL;
	if (INI_STR_CS("char", s)|| INI_STR_CS("int8_t", s)|| INI_STR_CS("bool", s))
		re = INI_TYPE_CHAR;
	else if (INI_STR_CS("uchar", s)|| INI_STR_CS("uint8_t", s)|| INI_STR_CS("byte", s))
		re = INI_TYPE_UNSIGNED | INI_TYPE_CHAR;
	else if (INI_STR_CS("short", s)|| INI_STR_CS("int16_t", s))
		re = INI_TYPE_SHORT;
	else if (INI_STR_CS("ushort", s)|| INI_STR_CS("uint16_t", s))
		re = INI_TYPE_UNSIGNED | INI_TYPE_SHORT;
	else if (INI_STR_CS("int", s)|| INI_STR_CS("int32_t", s))
		re = INI_TYPE_INT;
	else if (INI_STR_CS("uint", s)|| INI_STR_CS("uint32_t", s))
		re = INI_TYPE_UNSIGNED | INI_TYPE_INT;
	else if (INI_STR_CS("float", s))
		re = INI_TYPE_FLOAT;
	else if (INI_STR_CS("double", s))
		re = INI_TYPE_DOUBLE;
	else if (INI_STR_CS("long", s)|| INI_STR_CS("int64_t", s))
		re = INI_TYPE_LONG;
	else if (INI_STR_CS("ulong", s)|| INI_STR_CS("uint64_t", s))
		re = INI_TYPE_LONG | INI_TYPE_UNSIGNED;
	else if (INI_STR_CS("cstring", s))
		return INI_TYPE_STRING;
	return re;
}
//************************************
// Method:    inifindstructdata
// FullName:  inifindstructdata
// Access:    public 
// Returns:   ini_list_type *
// Qualifier: 扫描获取一个子结构体的运行时信息
// Parameter: ini_list_type * list 字符串分割后的列表
// Parameter: const char * name
// Parameter: int * alignsize 子结构内存对齐单位
//************************************
ini_list_type *inifindstructdata(ini_list_type *list, const char *name, int *alignsize) {
	//返回此子结构最大成员大小..
	int sc = 0, ass = 0, av;
	ini_list_type *father = NULL;
	ini_type type = 0;
	if (alignsize != NULL)
		*alignsize = 0;
	while (list)
	{
		do
		{
			if ((type = initypescan(list->data)) == INI_TYPE_ALL) {
				if (INI_STR_CS("struct", list->data)) {
					sc++;
				}
				else if (father != NULL && (!INI_STR_CS("struct", father->data)) && sc > 0) {
					//结构体的第一个节点是类型名..
					sc--;
					if (sc == 0) {
						if (alignsize != NULL)
							*alignsize = ass;
						return list;
					}
				}
			}
			else {
				if ((av = inifiledgetsize(type)) > ass)
					ass = av;//保留最大的成员尺寸..
				list = list->next;
			}

		} while (0);
		father = list;
		list = list->next;
	}
	return NULL;
}
//************************************
// Method:    inimakeitem
// FullName:  inimakeitem
// Access:    public 
// Returns:   int
// Qualifier: 根据给定的信息构造一个运行时信息节点，此节点包含了一个字段的全部描述信息
// Parameter: ini_list_type * list
// Parameter: const char * tag
// Parameter: const char * names
// Parameter: int type
// Parameter: int * offset
//************************************
int inimakeitem(ini_list_type *list, const char *tag, const char *names, int type, int *offset) {
	ini_list_type *ff = ini_strtok(names, ",");
	int soffset = 0, size = 0, cc = 0;
	iniitem *x = NULL;
	int isarray = 0, cnt = 1;
	const char * fs = 0, *fe = 0;
	ini_list_foreach_object(ff, char *, name) {
		//计算每一个字段，如:int x,y,z;
		//判断当前字段是否为一个数组:如z[2]
		cnt = 1;
		isarray = ((fs = strstr(name, "[")) != NULL && (fe = strstr(name, "]")) != NULL) ? 1 : 0;
		if (isarray != 0) {
			//将name中的[]标记删除..
			name[((uint)fe) - (uint)name] = 0x00;
			cnt = atoi(fs + 1);//计算个数
			name[((uint)fs) - (uint)name] = 0x00;//给定正确的名字..

		}
		size = inifiledgetsize(type);
		x = inimalloc(sizeof(iniitem) + strlen(tag) + strlen(name) + 1);
		x->name = ((char*)x) + sizeof(iniitem);
		x->name[0] = 0x00;
		strcpy(x->name, tag);
		strcat(x->name, name);
		x->cnt = cnt;
		x->size = size;
		x->type = type | ((cnt > 1&&((type&INI_TYPE_STRING)!= INI_TYPE_STRING)) ? (INI_TYPE_ARRAY) : 0);
		if (size > 1 && (((*offset) % size) != 0)) {
			//没有对齐内存..
			offset[0] += (size - (offset[0] % size));
		}
		x->offset = *offset;
		*offset += size*cnt;
		cc++;
		ini_list_push(list, x, sizeof(iniitem));
	}
	ini_list_del(ff);
	return cc;
}

//************************************
// Method:    inimemcmp
// FullName:  inimemcmp
// Access:    public 
// Returns:   int
// Qualifier: 确认一段内存是否全部未一个固定的值
// Parameter: const void * ptr
// Parameter: uchar tt
// Parameter: int len
//************************************
int inimemcmp(const void *ptr, uchar tt, int len) {
	//对指定地址确认是否全部为tt的值..
	uchar *p = (uchar *)ptr;
	for (int i = 0; i < len; i++)
	{
		if (p[i] != tt)
			return i + 1;
	}
	return 0;
}
//************************************
// Method:    iniconvert
// FullName:  iniconvert
// Access:    public 
// Returns:   ini_list_type*
// Qualifier: 将字符串分割后的列表转换成iniitem运行时列表
// Parameter: ini_list_type * list
// Parameter: const char * tag
// Parameter: int * offset
//************************************
ini_list_type* iniconvert(ini_list_type *list, const char *tag, int *offset) {
	int _offset = 0, alignsize = 0;
	if (offset == NULL)
		offset = &_offset;
	uint slen = ini_path_maxlen;
	char *sv = NULL, *ev = NULL;
	if (tag != NULL)
		slen = strlen(tag) + 64;
	uchar *stag = inimalloc(slen);
	if (stag == NULL)
		return NULL;
	if (tag == NULL) {
		stag[0] = '.';
		stag[1] = 0x00;
	}
	else {
		strcpy(stag, tag);
	}
	ini_list_type* re = ini_list_head(), *filed = NULL;
	if (re == NULL)
		return NULL;
	ini_type t = INI_TYPE_ALL;
	iniitem *it = NULL;
	for (ini_list_type *temp = list->next; temp&&temp->next; temp = temp->next)
	{
		//识别类型..
		t = initypescan(temp->data);
		if (t == INI_TYPE_ALL) {
			//无法识别的类型..
			//需要确认是否未"struct"
			if (!INI_STR_CS("struct", temp->data))
				goto OVER;
			//这是一个结构体..
			//需要解决结构体多变量定义问题
			//exp:struct{int x;}a,b,c[5];
			//此时获取到的sname="a,b,c,[5]"
			//需要将sname针对",进行分割后逐一进行分布检查"
			//当前默认不支持结构体数组出现..
			ini_list_type *sname = inifindstructdata(temp, NULL, &alignsize);//查找当前结构体的字段名称..
			if (sname == NULL)
				goto ERR;
			ini_list_type *vs = ini_strtok(sname->data, ",");
			ini_list_foreach_object(vs, const char *, vname) {//构建每一个变量的特征信息
				int skl = strlen(stag);
				it = inimalloc(sizeof(iniitem) + strlen(vname) + skl + 4);
				it->name = ((uchar *)it) + sizeof(iniitem);//存储这个节点的名称..
				it->type = INI_TYPE_STRUCT;
				it->cnt = 1;
				it->items = NULL;
				ini_list_push(re, it, sizeof(iniitem));//加入这个节点信息..
				if ((sv = strstr(vname, "[")) != NULL) {
					//这是一个结构体数组..
					//需要在最前端插入一个节点，用于告知解析器，这个结构体时一个数组元素..
					sv++;
					ev = strstr(sv, "]");
					if (ev == NULL || (((uint)sv) >= ((uint)ev))) {
						goto ERR;
					}
					ev[0] = 0x00;
					it->cnt = atoi(sv);//记录这个结构体数组的最大个数..
					sv--;
					sv[0] = 0x00;
					it->type |= INI_TYPE_ARRAY;//标记此数组属性
				}

				//加上新的前缀..
				if (strlen(stag) + ((ini_list_type*)vname__temp)->count >= slen) {
					//名字太长了..
					slen = slen * 2 + ((ini_list_type*)vname__temp)->count;
					char *ss = inimalloc(slen);
					if (ss == NULL)
						goto ERR;
					strcpy(ss, stag);
					inifree(stag);
					stag = ss;
				}
				//合成新的定位路径..

				strcat(stag, vname);
				strcat(stag, ".");//结构体节点最后不再需要.符号结尾..
				strcpy(it->name, stag);
				ini_list_type*sb = NULL;
				if (alignsize > 1 && ((offset[0] % alignsize) != 0)) {//结构体强制对齐到指定字节..
					offset[0] += (alignsize - (offset[0] % alignsize));
				}
				it->offset = offset[0];
				if (initypescan(temp->next->data) == INI_TYPE_ALL)
					sb = iniconvert(temp->next, stag, offset);
				else
					sb = iniconvert(temp, stag, offset);
				if (sb != NULL) {//这些就是全部子元素.
					it->items = sb;
				}
				//删除子选项..
				stag[skl] = 0x00;
				if (alignsize > 1 && ((offset[0] % alignsize) != 0)) {//结构体大小强制对齐到最大成员大小..
					offset[0] += (alignsize - (offset[0] % alignsize));
				}
				it->size = (offset[0] - it->offset);//计算这个结构体有多大..
				offset[0] += it->size*(it->cnt - 1);
			}
			temp = sname;
			ini_list_del(vs);
		}
		else {//基于stag前缀新增一个节点信息..
			filed = temp->next;
			temp = temp->next;
			//解析filed字段是否存在,分隔符.定义了多个同类型变量，或者是否使用了数组[]下标
			inimakeitem(re, stag, filed->data, t, offset);
		}
	}
	goto OVER;
ERR:
	inidel(re);
	re = NULL;
OVER:
	inifree(stag);
	return re;
}

//************************************
// Method:    iniprint
// FullName:  iniprint
// Access:    public 
// Returns:   void
// Qualifier: 基于运行时列表打印全部节点信息
// Parameter: ini_list_type * list
//************************************
void iniprint(ini_list_type *list) {
	ini_list_foreach_object(list, iniitem *, x) {
		printf("\r\nname=%s,size=%d,cnt=%d,type=%d,offset=%d\r\n", x->name, x->size, x->cnt, x->type, x->offset);
		if ((x->type&INI_TYPE_ALL) == INI_TYPE_STRUCT) {
			iniprint(x->items);
		}
	}
}
//************************************
// Method:    inimake
// FullName:  inimake
// Access:    public 
// Returns:   ini_list_type*
// Qualifier: 给定一个结构体描述字符串，构造一个iniitem运行时列表
// Parameter: const char * str
//************************************
ini_list_type* inimakeforstring(const char *str) {
	//构造一个结构体的运行时信息..
	//const char *__teststr = "{ int x; int y; int z; char name[32]; struct { int d1; int d2; }data; int fs;}";
	if (str == NULL)
		return NULL;
	const char *mstr = strstr(str, "{");
	if (mstr == NULL)
		return NULL;
	str = mstr;
	ini_list_type *rs = ini_strtok(str, ";{} ");
	if (rs == NULL)
		return NULL;
	//需要将误伤的组合定义列表恢复..
	if (strstr(str, ",") != NULL) {
		ini_list_foreach_object(rs, char*, x)
		{
			ini_list_type *xt = (ini_list_type *)x__temp;
			if (xt->next != NULL && xt->count > 1 && xt->data[xt->count - 2] == ',') {
				//需要将下一个节点链接到本节点..
				char *ss = inimalloc(xt->count + xt->next->count + 4);
				if (ss == NULL)
					return NULL;
				strcpy(ss, xt->data);
				strcat(ss, xt->next->data);
				inifree(xt->data);
				xt->data = ss;
				xt->count = xt->count + xt->next->count - 1;
				inifree(xt->next->data);
				void *ptr = xt->next;
				xt->next = xt->next->next;
				inifree(ptr);
				rs->count--;
			}
		}
	}
	//将rs转换成iniitem格式的列表..
	ini_list_type *re = NULL;
	re = iniconvert(rs, NULL, NULL);
	ini_list_del(rs);
	return re;
}

//************************************
// Method:    ini_str_find_last_pp
// FullName:  ini_str_find_last_pp
// Access:    public 
// Returns:   int
// Qualifier: 倒序查找'.'路径分割字符
// Parameter: const char * name
// Parameter: int index 倒数第index个，-1=倒数第一个，-2=倒数第二个,......
//************************************
int ini_str_find_last_pp(const char * name,int index) {
	int pos = -1,cnt=0,size=0;
	for (int i = 0; name[i]!=0X00; i++)
	{
		if (name[i] == '.') {
			cnt++;
			pos = i;
		}
		size++;
	}
	if (index != -1) {
		for (int i = size-1; i>=0; i--)
		{
			if (name[i] == '.') {
				index++;
				pos = i;
			}
			if (index == 0)
				return pos;
		}
		pos = -1;
	}
	return pos;//找不到..
}

//************************************
// Method:    ini_find_at
// FullName:  ini_find_at
// Access:    public 
// Returns:   iniitem*
// Qualifier: 在运行时列表中查找指定类型的同名字段节点信息
// Parameter: ini_list_type * dlists
// Parameter: const char * name
// Parameter: int type
//************************************
iniitem*ini_find_at(ini_list_type *dlists, const char *name, int type) {
	int pos = 0,nlen=0;
	ini_list_foreach_object(dlists, iniitem*, x) {
		pos = ini_str_find_last_pp(x->name, -1 - __NAME_OFFSET_X(x));
		nlen = strlen(x->name);
		if (memcmp(name, x->name + pos + 1, nlen - pos - 1 - __NAME_OFFSET_X(x)) == 0x00) {
			if (type > 0) {
				if((x->type&INI_TYPE_ALL)!=type)
					continue;
			}
			return x;
		}
	}
	return NULL;
}

//************************************
// Method:    iniserialize_txt_ll
// FullName:  iniserialize_txt_ll
// Access:    public 
// Returns:   char*
// Qualifier: 序列化成txt文本格式的底层实现
// Parameter: ini_list_type * dslists iniitem运行时列表
// Parameter: const void * handle 需要序列化的目标内存起点
// Parameter: uint flag 序列化选项，参考@ini_serialize_flag
// Parameter: char * bpath 序列化的基本路径
//************************************
char* iniserialize_txt_ll(ini_list_type *dslists, const void *handle, uint flag,char *bpath) {
	//将目标地址按照dslists描述进行明文序列化
	short blen = 64, slen = 0, tlen = 0, index = 0, alen = 0, re = 0,bpathlen=0;
	char *buf = inimalloc(blen), *ref = (char *)handle;
	//bpath，存储了从上往下的基本路径，特别声明，用于修正数组路径存在..
	bpathlen = (short)strlen(bpath);//计算当前基本路径的长度..
	do
	{
		ini_list_foreach_object(dslists, iniitem*, x) {
			//计算这个选项一共需要多少字节内存，确认当前的buf是否能够装下..
			if ((flag&ISF_REMOVE_ZERO) && inimemcmp((char*)handle + x->offset, 0X00, x->size*x->cnt) == 0x00) {
				//这是一个0填充节点..可以不用进行存储..
				continue;
			}
			bpath[bpathlen] = 0X00;//重新恢复基本路径
			if ((x->type&INI_TYPE_ALL) == INI_TYPE_STRUCT) {
				
				for (index = 0, alen = x->cnt; index < alen; index++)
				{
					if ((flag&ISF_REMOVE_ZERO) && inimemcmp((char*)handle + x->offset + index*x->size, 0X00, x->size) == 0x00) {
						//这是一个0填充节点..可以不用进行存储..
						continue;
					}
					blen = (short)(ini_correction_memory(buf, blen, slen + strlen(x->name) + 16));
					int pos = ini_str_find_last_pp(x->name, -2);
					if (x->type&INI_TYPE_ARRAY) {
						//slen += sprintf(buf + slen, "\n::%s", x->name) - 1;
						//slen += sprintf(buf + slen, "[%d]", index);
						//需要将路径字段添加正确的索引号..
						pos = sprintf(bpath + bpathlen, "%s", x->name + pos)-1;
						pos += sprintf(bpath + bpathlen + pos, "[%d]", index);
						//新的带数组下标的路径合成完毕..
					}
					else {
						pos = sprintf(bpath + bpathlen, "%s", x->name + pos) - 1;
					}
					bpath[bpathlen + pos] = 0x00;//修正结尾..修正完毕..
					char *t = iniserialize_txt_ll(x->items, ref + x->size*index, flag|((x->type&INI_TYPE_ARRAY)? ISF_LL_ARRAY:0),bpath);//继续进行序列化..
					if (t!=NULL) {
						//得到了具体的数据了..
						//进行合并即可..
						int tn = strlen(t);
						blen = ini_correction_memory(buf, blen, slen + tn + 4);
						memcpy(buf + slen, t, tn);
						slen += tn;
					}
					if (t != NULL) {
						inifree(t);
					}
					if (x->type&INI_TYPE_ARRAY) {
						//blen = ini_correction_memory(buf, blen, slen + 8);
						//slen += sprintf(buf + slen, "\n:::");
					}
				}
			}
			else {//这是一个基本的对象，可以直接进行序列化..
				index = 0; alen = x->cnt;
				blen = (short)(ini_correction_memory(buf, blen, slen + strlen(x->name) + 64));
				if (flag&ISF_LL_ARRAY) {
					//父节点存在数组..
					//需要进行距离修正..
					//获取x->name节点名称的最后一项，与基本路径合并即可..
					int pos = ini_str_find_last_pp(x->name,-1);
					if(pos<0)
						continue;
					slen += sprintf(buf + slen, "\n%s%s,%d,%d,%d,", bpath,x->name+pos, x->size, x->cnt, x->type);
				}
				else {
					slen += sprintf(buf + slen, "\n%s,%d,%d,%d,", x->name, x->size, x->cnt, x->type);
				}
				if (x->type& INI_TYPE_STRING) {//作为字符串进行处理..
											   //需要保证blen空闲内存大于2倍于x.cnt
					blen = (short)(ini_correction_memory(buf, blen, slen + strlen(x->name) + 64 + x->cnt * 2));
					re = initostring(x, (void *)((char*)handle + x->offset), buf + slen);
					if (re > 0)
					{
						slen += re;
					}
				}
				else {
					if (alen > 1) {
						slen += sprintf(buf + slen, "[");
					}
					for (index = 0, alen = x->cnt; index < alen; index++)
					{
						//将数据tostring然后合成..
						blen = ini_correction_memory(buf, blen, slen + 32);
						re = initostring(x, (void *)((char*)handle + x->offset + x->size*index), buf + slen);
						if (re > 0)
						{
							slen += re;
						}
						if (index < (alen - 1)) {//使用分隔符进行隔离..
							buf[slen++] = ',';
						}
					}
					if (alen > 1) {
						slen += sprintf(buf + slen, "]");
					}
				}

			}
		}
	} while (0);
	buf[slen] = 0x00;
	return buf;
}
//************************************
// Method:    iniserialize_to_txt
// FullName:  iniserialize_to_txt
// Access:    public 
// Returns:   char* 序列化成功后的txt内存
// Qualifier: 序列化成txt文本格式的封装
// Parameter: ini_list_type * dslists iniitem运行时列表
// Parameter: const void * handle 需要序列化的目标内存起点
// Parameter: uint flag 序列化选项，参考@ini_serialize_flag
//************************************
char* iniserialize_to_txt(ini_list_type *dslists, const void *handle, uint flag) {
	char *path = inimalloc(ini_path_maxlen);
	if (path == NULL)
		return NULL;
	path[0] = 0X00;
	char *buf = iniserialize_txt_ll(dslists, handle, flag&(ISF_LL_ARRAY-1), path);
	inifree(path);
	return buf;
}

//************************************
// Method:    inidescrialize_txt_calc_value
// FullName:  inidescrialize_txt_calc_value
// Access:    public 
// Returns:   int
// Qualifier: 反序列化（txt）计算节点值（从字符串）
// Parameter: void * handle 目标内存
// Parameter: iniitem * item 运行时信息
// Parameter: const char * value txt传入的对应字段的值的字符串
//************************************
int inidescrialize_txt_calc_value(void *handle, iniitem *item, const char *value) {
	//DBGF("\r\ndesc value:name=%s , type=%d ,offset = %d, value=%s", it->name, it->type,offset+it->offset, value);
	//根据类型进行反序列化数据..
	uint type = item->type&INI_TYPE_ALL;
	char*str = handle;
	
	ini_list_type *ss = ini_list_head();
	int len = strlen(value), pos = 0, errocde = 0;
	if (type == INI_TYPE_STRING) {//字符串需要进行特殊处理,还原转义字符..
		pos = 0;
		for (int i = 0; i < len && item->cnt>(i-1); i++)
		{
			if (value[i] == '\\') {
				//这是一个转义字符..
				i++;
				if (value[i] == 'r') {
					str[pos++] = '\r';
				}
				else if (value[i] == 'n') {
					str[pos++] = '\n';
				}
				else if (value[i] == 't') {
					str[pos++] = '\t';
				}
				else if (value[i] == '\\') {
					str[pos++] = '\\';
				}
			}
			else {
				str[pos++] = value[i];
			}
		}
		str[pos++] = 0X00;
		errocde = pos;
		goto OVER;
	}
	if (item->type&INI_TYPE_ARRAY) {
		//这是一个数组元素..
		//寻找开始和结束的位置[1,2,3,4,5]//这样的序列
		//拆解，并将全部元素一个个的反序列化到目标内存..
		char *s = strstr(value, "[");
		if(s == NULL)
		{
			errocde = -1; goto OVER;
		}
		char *e = strstr(s+1, "]");
		if(e==NULL)
		{
			errocde = -2; goto OVER;
		}
		s += 1;//从s开始，到e结尾，中间的元素以','字符分割..
		ini_list_type *arrays = __ini_ll_strtok(s, e - s, 1, ", ");
		if (arrays == NULL) {
			errocde = -3; goto OVER;
		}
		if (arrays->count > 0) {
			ini_list_join(ss, arrays);
			ini_list_reset(arrays);
		}
		ini_list_del(arrays);//全部的项都加入了ss列表，直接释放arrays指针即可
	}
	else {
		ini_list_push_for_data(ss, value, 0, len+1);
		ini_list_find_at(ss, -1)->data[len] = 0x00;
	}
	ini_list_foreach_object(ss, char*, x) {
		switch (type)
		{
			case  INI_TYPE_CHAR: {
				((char*)handle)[0] = (char)atoi(x);
			}break;
			case  INI_TYPE_SHORT: {
				((short*)handle)[0] = (short)atoi(x);
			}break;
			case  INI_TYPE_INT: {
				((int*)handle)[0] = atoi(x);
			}break;
			case  INI_TYPE_FLOAT: {
				((float*)handle)[0] = (float)atof(x);
			}break;
			case  INI_TYPE_DOUBLE: {
				((double*)handle)[0] = atof(x);
			}break;
			default: {
				errocde = -4; goto OVER;
			}break;
		}
		handle = ((char*)handle)+ (uint)item->size;//向后移动内存..
		errocde++;
	}
OVER:
	ini_list_del(ss);
	return errocde;
}

//************************************
// Method:    inidescrialize_txt_prase
// FullName:  inidescrialize_txt_prase
// Access:    public 
// Returns:   int
// Qualifier: 解析一个字段的值并更新至目标内存
// Parameter: ini_list_type * dslists
// Parameter: void * handle
// Parameter: iniitem * item
// Parameter: const char * value
//************************************
int inidescrialize_txt_prase(ini_list_type *dslists, void *handle, iniitem*item, const char * value) {

	int offset = 0, index = 0, spos = 0, errcode = 1;
	ini_list_type *base = dslists,*lcur=NULL;
	ini_list_type * paths = ini_strtok(item->name, ".");
	int type = 0, lindex = -1, pos = -1,epos=-1,vindex=0;
	char *vname = NULL,*ctmp=NULL;
	do 
	{
		//逐一进行匹配
		ini_list_foreach_object(paths, char*, x) {
			vindex++;
			type = INI_TYPE_STRUCT;
			if (index == (paths->count - 1)) {
				//最后一个节点..
				//需要查找的是基本节点..
				type = 0;
			}
			lcur = base;
			spos = 0;
			epos = 0;
			pos  = 0;
			if ((ctmp = strchr(x, '[')) != NULL) {
				pos = ((uint)ctmp - (uint)x);//计算出数组下标开始的位置..
				if ((ctmp = strchr(ctmp, ']')) != NULL) {
					epos = ((uint)ctmp - (uint)(x));
					//计算真正的pos下标..
					x[epos] = 0X00;
					spos = atoi(x + pos + 1);//获取到了路径的下标..
					x[epos] = ']';
				}
				x[pos] = 0x00;//抹去路径中下标的部分..
				(((ini_list_type*)x__temp)->count) = pos+1;//修正长度..
			}
			ini_list_foreach_object(base, iniitem*, it) {
				lindex = -1;
				if ((it->type&INI_TYPE_STRUCT) == INI_TYPE_STRUCT) {//这是一个结构体节点..
					lindex = -2;
				}
				lindex = ini_str_find_last_pp(it->name, lindex);
				if(lindex <0)
					continue;
				vname = it->name + lindex;
				//获取到节点名称了..
				if (memcmp(x, vname+1, (((ini_list_type*)x__temp)->count - 1)) == 0x00) {
					//命中..
					//需要再次确认字符串是否完整的匹配..
					if (strlen(vname + 1) != ((((ini_list_type*)x__temp)->count) - ((it->type&INI_TYPE_STRUCT) == INI_TYPE_STRUCT ? 0 : 1))) {
						//字符串未能完整的进行匹配..
						continue;//不是需要的目标项..
					}
					if ((it->type&INI_TYPE_STRUCT) != INI_TYPE_STRUCT && paths->count!=vindex) {
						continue;//类型不匹配..目前还需要结构体节点
					}
					if ((it->type&INI_TYPE_STRUCT) == INI_TYPE_STRUCT && paths->count == vindex) {
						continue;//类型不匹配..目前需要基本节点..
					}

					base = it->items;
					//计算内存offset..
					offset += it->size*spos;//这是记录附加偏移
					if ((it->type&INI_TYPE_STRUCT) != INI_TYPE_STRUCT) {
						//这是一个基本的节点..
						//匹配类型是否一致..
						if (item->type != it->type || it->size != item->size)
						{
							errcode = -1; break;
						}//类型不一致无法进行反序列化..
						if (it->cnt < spos)
						{
							errcode = -2; break;
						}//装不下，这个描述表示没有足够的内存存放..
						//开始进行反序列化目标内存未:it->offset+offset+handle
						errcode = inidescrialize_txt_calc_value((char*)handle + it->offset + offset, it, value);
						if (errcode > 0)
							it->hit = 1;//反序列化成功..
					}
					break;
				}
			}
			if(errcode<0)
				break;
			if (lcur == base) {
				//无法匹配的路径..
				errcode = -3; break;
			}
		}
	} while (0);
	ini_list_del(paths);
	return errcode;
}

//************************************
// Method:    inidescrialize_txt_hit_clear
// FullName:  inidescrialize_txt_hit_clear
// Access:    public 
// Returns:   void
// Qualifier: 清除运行时列表中iniitem的命中标记
// Parameter: ini_list_type * base
//************************************
void inidescrialize_txt_hit_clear(ini_list_type *base) {
	ini_list_foreach_object(base, iniitem*,x) {
		if ((x->type & INI_TYPE_STRUCT) == INI_TYPE_STRUCT) {
			inidescrialize_txt_hit_clear(x->items);
		}
		else {
			x->hit = 0x00;
		}
	}
}

//************************************
// Method:    inideserialize_from_txt
// FullName:  inideserialize_from_txt
// Access:    public 
// Returns:   int
// Qualifier: 序列化结构体到txt文本格式（此格式用于ini配置文件使用，便于vim控制台更新配置文件）
// Parameter: ini_list_type * dslists
// Parameter: const char * msg
// Parameter: void * handle
// Parameter: uint size
// Parameter: int * errcnt
// Parameter: uint flag
//************************************
int inideserialize_from_txt(ini_list_type *dslists, const char *msg, void *handle, uint size,int *errcnt,uint flag) {
	if (msg==NULL || handle == NULL || dslists == NULL || dslists->count == 0)
		return -1;
	char *line = NULL,*value=NULL;
	int scnt=0,ecnt=0,re=0;
	ini_list_type *list = ini_list_head();
	iniitem item = { 0 };
	if (flag&ISF_LL_FILL_ZERO) {
		memset(handle, 0, size);//默认清空内存..
	}
	if (flag&ISF_LL_MARK_HIT) {
		inidescrialize_txt_hit_clear(dslists);//清空命中计数器..
	}
	int x = 0, s = 0;
	for (int i = 0, j = 0,l=0; msg[i] != 0x00; i = j + 1)
	{
		ini_list_clear(list);
		j = i;
		while (msg[j] != 0X00 && msg[j] != '\n')
		{
			j++;
		}
		if (i < j)
		{
			line = (char*)msg + i;//长度为j-i
			l = j - i;
			do
			{
				//开始进行解析这行的具体内容..
				//直接使用这行数据构造一个数据项..
				for (x=0,s=0; x < l&&list->count<4; x++)
				{
					if (line[x] == ',') {
						//将数据推入队列..
						ini_list_push_for_data(list, line, s, x - s+1);
						ini_list_find_at(list, -1)->data[x - s] = 0x00;
						s = x + 1;
					}
				}
				if (x < l) {
					ini_list_push_for_data(list, line, s, l - s+1);
					ini_list_find_at(list, -1)->data[l - s] = 0x00;
					value = ini_list_find_at(list, -1)->data;
				}
				//将数据进行分割了..
				//判断数量对不对..
				if(list->count<4|| list->next->data[0]!='.')
					continue;
				//构造一个iniitem对象..
				memset(&item, 0, sizeof(item));
				item.name = list->next->data;
				item.size = atoi(ini_list_find_at(list,1)->data);
				item.cnt  = atoi(ini_list_find_at(list, 2)->data);
				item.type = atoi(ini_list_find_at(list, 3)->data);
				//得到了一个节点..
				//确认名字中是否存在[]，以此来修正目标内存..
				//DBGF("\r\ndesc:name=%s,size=%d,type=%d,type=%d,value=%s", item.name, item.size, item.cnt, item.type,value);
				re=inidescrialize_txt_prase(dslists, handle, &item, value);//反序列化解析..
				if (re > 0) {
					scnt++;
				}
				else
					ecnt++;
			} while (0);
		}
	}
	ini_list_del(list);
	if (errcnt)
		*errcnt = ecnt;
	return scnt;
}

//INIJSON指示是否提供cjson序列化功能
#if defined(INIJSON) && INIJSON!=0
//************************************
// Method:    _iniserialize_to_json_object
// FullName:  _iniserialize_to_json_object
// Access:    public 
// Returns:   cJSON*
// Qualifier: 序列化结构体至cjson类型
// Parameter: ini_list_type * dslists
// Parameter: const void * handle
// Parameter: uint flag
// Parameter: int * err
//************************************
cJSON* _iniserialize_to_json_object(ini_list_type * dslists, const void * handle, uint flag, int *err) {
	
	int pos = -1,nlen=0;
	inivalue *value = NULL;
	cJSON *root = cJSON_CreateObject();
	char *name = inimalloc(__NAMEMAXLEN);
	memset(name, 0, __NAMEMAXLEN);
	INI_ERR_ASSERT_START();
	INI_ASSERT(root&&dslists&&handle, -1);
	ini_list_foreach_object(dslists, iniitem*, x) {
		value = ( inivalue *)(x->offset + (handle));//目标内存..

		pos = ini_str_find_last_pp(x->name, -1- __NAME_OFFSET_X(x));
		nlen = strlen(x->name);
		INI_ASSERT((nlen - pos - 1 - __NAME_OFFSET_X(x)) < __NAMEMAXLEN, -3);
		memcpy(name, x->name + pos + 1, nlen - pos - 1- __NAME_OFFSET_X(x));
		name[nlen - pos - 1- __NAME_OFFSET_X(x)] = 0X00;


		//遍历每一个节点，建立json树..
		if ((x->type & INI_TYPE_ALL) == INI_TYPE_STRUCT) {
			
			cJSON *item = NULL;
			if ((x->type&INI_TYPE_ARRAY) == 0x00) {//不是数组节点..
				item = _iniserialize_to_json_object(x->items, handle, flag, err);
			}
			else {//数组节点..进行遍历构造每一个节点..
				item = cJSON_CreateArray();
				INI_ASSERT(item, -2);
				for (int i = 0; i < x->cnt; i++)
				{
					cJSON* sub= _iniserialize_to_json_object(x->items, handle+x->size*i, flag| ISF_LL_ARRAY, err);
					INI_ASSERT(sub, -3);
					cJSON_AddItemToArray(item, sub);
				}
			}
			INI_ASSERT(item, -4);
			cJSON_AddItemToObject(root, name, item);
		}
		else {
			
			if ((x->type & INI_TYPE_ALL) == INI_TYPE_STRING) {
				if (x->type&INI_TYPE_ARRAY) {
					//基本类型的数组..
					//不支持..
				}
				else {
					cJSON_AddItemToObject(root, name, cJSON_CreateString(value->strvalue));
				}
			}
			else if ((x->type & INI_TYPE_ALL) == INI_TYPE_INT) {
				if (x->type&INI_TYPE_ARRAY) 
					cJSON_AddItemToObject(root, name, cJSON_CreateIntArray(&value->ivalue, x->cnt));
				else 
					cJSON_AddItemToObject(root, name, cJSON_CreateNumber((x->type & INI_TYPE_UNSIGNED) ? value->uivalue : value->ivalue));
			}
			else if ((x->type & INI_TYPE_ALL) == INI_TYPE_SHORT) {
				if (x->type&INI_TYPE_ARRAY) {
					int *aa = inimalloc(sizeof(int)*x->cnt);
					memset(aa, 0x00, sizeof(int)*x->cnt);
					for (int z = 0; z < x->cnt; z++)aa[z] = (x->type & INI_TYPE_UNSIGNED) ? (&value->usvalue)[z] : (&value->svalue)[z];
					cJSON_AddItemToObject(root, name, cJSON_CreateIntArray(aa, x->cnt));
					inifree(aa);
				}
				else
					cJSON_AddItemToObject(root, name, cJSON_CreateNumber((x->type & INI_TYPE_UNSIGNED) ? value->usvalue : value->svalue));
			}
			else if ((x->type & INI_TYPE_ALL) == INI_TYPE_CHAR) {
				if (x->type&INI_TYPE_ARRAY) {
					int *aa = inimalloc(sizeof(int)*x->cnt);
					memset(aa, 0x00, sizeof(int)*x->cnt);
					for (int z = 0; z < x->cnt; z++)aa[z] = (x->type & INI_TYPE_UNSIGNED) ? (&value->ucvalue)[z] : (&value->cvalue)[z];
					cJSON_AddItemToObject(root, name, cJSON_CreateIntArray(aa, x->cnt));
					inifree(aa);
				}
				else
					cJSON_AddItemToObject(root, name, cJSON_CreateNumber((x->type & INI_TYPE_UNSIGNED) ? value->ucvalue : value->cvalue));
			}
			else if ((x->type & INI_TYPE_ALL) == INI_TYPE_FLOAT) {
				if (x->type&INI_TYPE_ARRAY)
					cJSON_AddItemToObject(root, name, cJSON_CreateFloatArray(&value->fvalue, x->cnt));
				else
					cJSON_AddItemToObject(root, name, cJSON_CreateNumber(value->fvalue));
			}
			else if ((x->type & INI_TYPE_ALL) == INI_TYPE_DOUBLE) {
				if (x->type&INI_TYPE_ARRAY)
					cJSON_AddItemToObject(root, name, cJSON_CreateDoubleArray(&value->dvalue, x->cnt));
				else
					cJSON_AddItemToObject(root, name, cJSON_CreateNumber(value->dvalue));
			}
		}
	}

	INI_ERR_ASSERT_CATCH();


	INI_ERR_ASSERT_FINALLY();
	if (name)
		inifree(name);
	if (err) {
		*err =  INI_ERR_GET_CODE_INT();
	}
	return root;
}
//************************************
// Method:    iniserialize_to_json
// FullName:  iniserialize_to_json
// Access:    public 
// Returns:   char *
// Qualifier: 序列化结构体至json字符串
// Parameter: ini_list_type * dslists
// Parameter: const void * handle
// Parameter: uint flag
// Parameter: int * err
//************************************
char * iniserialize_to_json(ini_list_type * dslists, const void * handle, uint flag,int *err)
{
	cJSON*root = _iniserialize_to_json_object(dslists, handle, flag, err);
	if (root == NULL)
		return NULL;
	char *buf = cJSON_Print(root);
	cJSON_Delete(root);
	return buf;
}
//************************************
// Method:    _inideserialize_from_json
// FullName:  _inideserialize_from_json
// Access:    public 
// Returns:   int
// Qualifier: 从字符串反序列化至cjson结构
// Parameter: ini_list_type * dslists
// Parameter: const cJSON * root
// Parameter: void * handle
// Parameter: uint size
// Parameter: int * errcnt
// Parameter: uint flag
//************************************
int _inideserialize_from_json(ini_list_type * dslists, const cJSON*root, void * handle, uint size, int * errcnt, uint flag)
{
	int scnt= 0,stmp=0;
	inivalue *value = NULL;
	iniitem *iitem = NULL;
	INI_ERR_ASSERT_START();
	INI_ASSERT(dslists&&root&&handle&&size > 0&&(root->type== cJSON_Object||root->type== cJSON_Array), -1);
	//遍历每一个cjson节点..
	for (cJSON *item = root->child; item; item=((item==NULL)?NULL:item->next))
	{
		//查找对应的iniitem节点..
		iitem = ini_find_at(dslists, item->string, -1);
		if (iitem == NULL)
		{
			if (errcnt)
				errcnt[0] += 1;
			continue;//这个项无法匹配..
		}
		if (item->type == cJSON_Object || (item->type == cJSON_Array && item->child->type== cJSON_Object)) {//结构体或者结构体数组..
			//在dslist中查找目标节点..
			if (item->type == cJSON_Object) {
				INI_ASSERT((stmp=_inideserialize_from_json(iitem->items, item, handle, size, errcnt, flag)) >= 0, -2);
				scnt += stmp;
			}
			else {//结构体数组..
				int i = 0;
				for (cJSON*s = item->child; s!=NULL&&i<iitem->cnt;s=s->next, i++)
				{//数组循环解析..
					INI_ASSERT((stmp=_inideserialize_from_json(iitem->items, s, handle + iitem->size*i, size, errcnt, flag)) >= 0, -3);
					scnt += stmp;
				}
			}
		}
		else {//基本类型..开始进行解析..
			//获取这个节点的路径..并合成，查找对应的dlist节点..
			value = (inivalue *)(handle + iitem->offset);
			if (item->type == cJSON_Array) {
				item = item->child;
			}
			scnt++;
			for (int i = 0; i < iitem->cnt && item;++i,item=(i < iitem->cnt)?item->next:item)
			{
				if (item->type == cJSON_False || item->type == cJSON_True) {
					if ((iitem->type&INI_TYPE_ALL) <= INI_TYPE_INT) {
						(&value->cvalue)[i] = item->valueint == 0 ? 0 : 1;
					}
				}
				else if (item->type == cJSON_Number) {
					if ((iitem->type&INI_TYPE_ALL) == INI_TYPE_CHAR) {
						(&value->cvalue)[i] = (char)item->valueint;
					}
					else if ((iitem->type&INI_TYPE_ALL) == INI_TYPE_SHORT) {
						(&value->svalue)[i] = (short)item->valueint;
					}
					else if ((iitem->type&INI_TYPE_ALL) == INI_TYPE_INT) {
						(&value->ivalue)[i] = item->valueint;
					}
					else if ((iitem->type&INI_TYPE_ALL) == INI_TYPE_FLOAT) {
						(&value->fvalue)[i] = (float)item->valuedouble;
					}
					else if ((iitem->type&INI_TYPE_ALL) == INI_TYPE_DOUBLE) {
						(&value->dvalue)[i] = item->valuedouble;
					}
					else {
						if (i == 0) {
							scnt--;
							if (errcnt)
								errcnt[0] += 1;
						}
					}
				}
				else if ((iitem->type&INI_TYPE_ALL) == INI_TYPE_STRING) {
					int maxlen = strlen(item->valuestring);
					maxlen = maxlen >= iitem->cnt ? (iitem->cnt - 1) : (maxlen);//获取可以存放的最大尺寸..
					memcpy(value->strvalue, item->valuestring, maxlen);
					value->strvalue[maxlen] = 0x00;
					break;//因为cnt>1需要跳出去..
				}
				else {
					if (errcnt)
						errcnt[0] += 1;
					scnt--;
				}
			}
			
		}
	}
	INI_ASSERT(0, scnt);
	INI_ERR_ASSERT_CATCH();
	
	
	INI_ERR_ASSERT_FINALLY();

	return INI_ERR_GET_CODE_INT();
}
//************************************
// Method:    inideserialize_from_json
// FullName:  inideserialize_from_json
// Access:    public 
// Returns:   int 成功匹配并更新的字段数量
// Qualifier: 从字符串反序列化至结构体
// Parameter: ini_list_type * dslists
// Parameter: const char * json
// Parameter: void * handle
// Parameter: uint size
// Parameter: int * errcnt 字符串中未能与dslists描述的运行时成功匹配的字段数量
// Parameter: uint flag
//************************************
int inideserialize_from_json(ini_list_type * dslists, const char *json, void * handle, uint size, int * errcnt, uint flag)
{
	cJSON*root = cJSON_Parse(json);
	if (root == NULL)
		return -1;
	int re = _inideserialize_from_json(dslists, root, handle, size, errcnt, flag);
	cJSON_Delete(root);
	return re;
}
#endif

//************************************
// Method:    iniparse_test
// FullName:  iniparse_test
// Access:    public 
// Returns:   int
// Qualifier: 测试
//************************************
int iniparse_test() {
	ini_list_type *dlist = inimake(inimsg);
	inimsg msg = { 0 }, descmsg = { 0 };
	strcpy(msg.info, "hello\r\nworld\\");
	msg.x = 'A';
	msg.name = 'N';
	msg.data.y = 'C';
	msg.data.values.temper = 1.2;
	msg.data.kv.mode = 1;
	msg.data.kv.ssd = 15221;
	msg.data.kv.sv = 1200;
	msg.data.kv.sd = '1';
	msg.data.ext[0].mode = 2;
	msg.data.ext[0].ssd = 25221;
	msg.data.ext[0].sv = 2200;
	msg.data.ext[0].sd = '2';
	msg.data.ext[2].cf[3] = 1314159;
	msg.ex.version = 1122589;
	
	char * buf = iniserialize_to_txt(dlist, &msg, ISF_REMOVE_ZERO);

	if (buf != NULL)
	{
		printf("serialize:\r\n%s", buf);
		int len = strlen(buf);
		if (len > 0) {
			len = inideserialize_from_txt(dlist, buf, &descmsg, sizeof(descmsg), NULL, ISF_LL_MARK_HIT);
			int c = memcmp(&msg, &descmsg, sizeof(descmsg));
			printf("\r\ndeserialize result=%s , c = %d\r\n", c == 0 ? "success" : "failed", c);
			char * buf1 = iniserialize_to_txt(dlist, &descmsg, ISF_REMOVE_ZERO);
			if (buf1 != NULL)
			{

				printf("serialize:\r\n");
				printf("%s\r\nbuf==buf1 %s\r\n", buf1, strcmp(buf, buf1) == 0 ? "yes" : "no");
				
				inifree(buf1);
			}

		}
		inifree(buf);
	}

#if INIJSON!=0
	char *json = iniserialize_to_json(dlist, &msg, 0, NULL);
	printf("json serialize :\r\n%s", json);
	int scnt = inideserialize_from_json(dlist, json, &descmsg, sizeof(descmsg), NULL, 0);
	printf("json:deserialize %d,scnt=%d", memcmp(&msg, &descmsg, sizeof(descmsg)), scnt);
	inifree(json);
#endif
	inidel(dlist);
	return 1;
}
int iniparse_test2() {
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
	inimsg2 msg = {
		.x = 2,
	};
	msg.d[1] = 2;
	msg.ids[0].id = 1;
	ini_list_type *dlist = inimake(inimsg2);
	inimsg2 descmsg = { 0 };
	char * buf = iniserialize_to_txt(dlist, &msg, ISF_REMOVE_ZERO);

	if (buf != NULL)
	{
		printf("serialize:\r\n%s", buf);
		int len = strlen(buf);
		if (len > 0) {
			len = inideserialize_from_txt(dlist, buf, &descmsg, sizeof(descmsg), NULL, ISF_LL_MARK_HIT);
			int c = memcmp(&msg, &descmsg, sizeof(descmsg));
			printf("\r\ndeserialize result=%s , c = %d\r\n", c == 0 ? "success" : "failed", c);
		}
		inifree(buf);
	}
	inidel(dlist);

}
