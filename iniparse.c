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
// Qualifier: ����ָ���ڴ�����
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

//ʹ��Ĭ�������ṩ���㷨�ṹ
#if (defined(INIPORTENABLE) && INIPORTENABLE!=0)
//��ȡlistָ��������
#define         ini_list_get_item(head,index)  ((ini_list_type*)({int z = 0x00,aindex=index;if(aindex<0){aindex=(head)->count-((aindex*(-1))%((head)->count));}ini_list_type *item = head;for(;z < aindex; z++) {item = item->next;}item;}))
//��ȡlistָ��������ָ����ڴ��ַ
#define         ini_list_object(head,index,type)    ((type)(ini_list_get_item(head,index)->data))     
//ѭ������list�б�,v�ǵ�������������ini_list_type*
#define         ini_list_foreach(list,v)        for(ini_list_type *v = list->next;list&&v;v=v->next)    
//ѭ������list�б�,typeָ��Ŀ��ָ��������,v��Ŀ�����͵�����
#define         ini_list_foreach_object(list,type,v)        for(type v = (type)((list)->next->data),*v##__temp = (type)((list)->next);(list)&&(list)->count>0&&v##__temp;\
                                                                v##__temp=(v##__temp==NULL)?NULL:((type)(((ini_list_type*)v##__temp)->next)),v=(v##__temp==NULL?NULL:((type)(((ini_list_type*)v##__temp)->data))))

//************************************
// Method:    ini_list_clear
// FullName:  ini_list_clear
// Access:    public 
// Returns:   int
// Qualifier: ����б�����ͷ�dataָ����ڴ棨�����Ϊ�գ�
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
// Qualifier: ����б�����ͷ�dataָ����ڴ棨�����Ϊ�գ����ͷ�list�ڵ��ڴ�
// Parameter: ini_list_type * _list
//************************************
int ini_list_del(ini_list_type *_list) { if (_list == NULL)return 0x00; ini_list_clear(_list); return 1; }
//************************************
// Method:    ini_list_push
// FullName:  ini_list_push
// Access:    public 
// Returns:   int
// Qualifier: ����һ���ڵ�׷�ӵ�list��󣬲����ڵ��ڴ�ָ��data�ڴ棬��Ǵ˽ڵ�dataָ����ڴ��Сδcount
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
	if (node == NULL)//���п�
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
// Qualifier: ����һ��list�ڵ�ͷ
//************************************
ini_list_type * ini_list_head() {
	int sc = sizeof(ini_list_type);//Ĭ�ϵ��б�ʹ��ͳһ����Դ�����ò�����չ�ֶβ���..
	ini_list_type * inode = (ini_list_type *)inimalloc(sc);
	memset(inode, 0x00, sc);
	return (ini_list_type*)inode;
}
//************************************
// Method:    ini_list_join
// FullName:  ini_list_join
// Access:    public 
// Returns:   int
// Qualifier: �ϲ�data�б���list�б�..
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
// Qualifier: ����б������ͷ�dataָ����ڴ�ռ�
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
// Qualifier: ����һ���ڵ㵽list���ʹ�ÿ��������ķ�ʽ��
// Parameter: ini_list_type * _list
// Parameter: const void * data ���õ�Ŀ��
// Parameter: unsigned int offset ���Ŀ����ڴ�ƫ��
// Parameter: unsigned int len	�����ĳ��ȣ��ֽڣ�
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
// Qualifier: ����ָ���������б�ڵ�
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
			continue;//���ǿ�ʼ��λ��..
		else if (j > 0 && str[j + start] == '\"' && str[j + start - 1] != '\\') {
			continue;//��һ����"�Ŀ�ͷhe��β..
		}
		//����ǺϷ��������ַ�..
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
// Qualifier: �ָ��ַ�������ָ���ַ����Ϸָ�
// Parameter: const char * str	���ָ��ַ���
// Parameter: int _len	str�ܹ����зָ����󳤶ȣ�=-1ʱ�ָ������ַ�������ָ�ָ���Ĳ���
// Parameter: uint isfilterempty �Ƿ���˿�
// Parameter: const char * rex �ָ��ַ�����
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
					findex = i;//��¼��һ��"��λ��..
			}
		}
		if (size % 2 == 1)
			continue;//�ִ��ڲ�..����ƥ��..
		if ((x = (char *)strchr(rex, str[i])) == NULL)
			continue;
		if (i - 1 == lindex) {//��������
			lindex++;
			if (isfilterempty == 0) {
				//����һ���մ����б�
				char *np = (char *)inimalloc(1);
				if (np) {
					np[0] = 0x00;
					ini_list_push(list, np, 1);
				}
			}
			continue;
		}
		//��ôlindex+1<---------->i-1����һ���µ��ַ���..
		//�����б�..
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
	//�����쳣����..
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
// Qualifier: ɾ�����ͷ�����ʱ�б�
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
		((ini_list_type*)x__temp)->data = NULL;//�Ѿ���ǰ�ͷ����ڴ�..
	}
	ini_list_del(dlist);
	return 1;
}

//************************************
// Method:    initostring
// FullName:  initostring
// Access:    public 
// Returns:   int
// Qualifier: ��Ŀ���ڴ���ָ������ת�����ַ���
// Parameter: iniitem * x ������Ϣ
// Parameter: void * ptr Ŀ���ڴ�ָ��
// Parameter: char * buf �ַ������ָ��
//************************************
int initostring(iniitem *x, void *ptr, char *buf) {
	//����ת�����ַ�����ʽ..
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
		re = sprintf(buf, INIFFMTSTR, INIFFMTVAL((float)(*((double*)ptr))));//Ŀǰû�кõİ취�������л���ֻ��ʹ��float����
	}break;
	case INI_TYPE_LONG: {
		re = sprintf(buf, "%ld", *((long*)ptr));
	}break;
	case INI_TYPE_STRING: {
		re = strlen((char*)ptr);
		if (re <= 0)
			return re;
		//�˴���Ҫת�Ʒ�ֹ�ַ�����Я����ת���ַ�'\'
		if (strstr((char*)ptr, "\n") == NULL) {
			memcpy(buf, ptr, re);
			return re;
		}
		//��Ҫ����ת�壬����洢����������..
		char *str = (char *)ptr;
		int al = 0;
		for (int i = 0, j = 0; j < re; i++, j++)
		{
			if (str[j] == '\n' || str[j] == '\r' || str[j] == '\\') {
				buf[i++] = '\\';//����ת��..
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
	default:return -1;//��֧�ֵ�����..
	}

	return re;
}
//************************************
// Method:    inifiledgetsize
// FullName:  inifiledgetsize
// Access:    public 
// Returns:   int
// Qualifier: ��ȡ�����������͵��ڴ��С
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
// Qualifier: ɨ���ȡ�������ͣ�������ö��
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
// Qualifier: ɨ���ȡһ���ӽṹ�������ʱ��Ϣ
// Parameter: ini_list_type * list �ַ����ָ����б�
// Parameter: const char * name
// Parameter: int * alignsize �ӽṹ�ڴ���뵥λ
//************************************
ini_list_type *inifindstructdata(ini_list_type *list, const char *name, int *alignsize) {
	//���ش��ӽṹ����Ա��С..
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
					//�ṹ��ĵ�һ���ڵ���������..
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
					ass = av;//�������ĳ�Ա�ߴ�..
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
// Qualifier: ���ݸ�������Ϣ����һ������ʱ��Ϣ�ڵ㣬�˽ڵ������һ���ֶε�ȫ��������Ϣ
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
		//����ÿһ���ֶΣ���:int x,y,z;
		//�жϵ�ǰ�ֶ��Ƿ�Ϊһ������:��z[2]
		cnt = 1;
		isarray = ((fs = strstr(name, "[")) != NULL && (fe = strstr(name, "]")) != NULL) ? 1 : 0;
		if (isarray != 0) {
			//��name�е�[]���ɾ��..
			name[((uint)fe) - (uint)name] = 0x00;
			cnt = atoi(fs + 1);//�������
			name[((uint)fs) - (uint)name] = 0x00;//������ȷ������..

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
			//û�ж����ڴ�..
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
// Qualifier: ȷ��һ���ڴ��Ƿ�ȫ��δһ���̶���ֵ
// Parameter: const void * ptr
// Parameter: uchar tt
// Parameter: int len
//************************************
int inimemcmp(const void *ptr, uchar tt, int len) {
	//��ָ����ַȷ���Ƿ�ȫ��Ϊtt��ֵ..
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
// Qualifier: ���ַ����ָ����б�ת����iniitem����ʱ�б�
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
		//ʶ������..
		t = initypescan(temp->data);
		if (t == INI_TYPE_ALL) {
			//�޷�ʶ�������..
			//��Ҫȷ���Ƿ�δ"struct"
			if (!INI_STR_CS("struct", temp->data))
				goto OVER;
			//����һ���ṹ��..
			//��Ҫ����ṹ��������������
			//exp:struct{int x;}a,b,c[5];
			//��ʱ��ȡ����sname="a,b,c,[5]"
			//��Ҫ��sname���",���зָ����һ���зֲ����"
			//��ǰĬ�ϲ�֧�ֽṹ���������..
			ini_list_type *sname = inifindstructdata(temp, NULL, &alignsize);//���ҵ�ǰ�ṹ����ֶ�����..
			if (sname == NULL)
				goto ERR;
			ini_list_type *vs = ini_strtok(sname->data, ",");
			ini_list_foreach_object(vs, const char *, vname) {//����ÿһ��������������Ϣ
				int skl = strlen(stag);
				it = inimalloc(sizeof(iniitem) + strlen(vname) + skl + 4);
				it->name = ((uchar *)it) + sizeof(iniitem);//�洢����ڵ������..
				it->type = INI_TYPE_STRUCT;
				it->cnt = 1;
				it->items = NULL;
				ini_list_push(re, it, sizeof(iniitem));//��������ڵ���Ϣ..
				if ((sv = strstr(vname, "[")) != NULL) {
					//����һ���ṹ������..
					//��Ҫ����ǰ�˲���һ���ڵ㣬���ڸ�֪������������ṹ��ʱһ������Ԫ��..
					sv++;
					ev = strstr(sv, "]");
					if (ev == NULL || (((uint)sv) >= ((uint)ev))) {
						goto ERR;
					}
					ev[0] = 0x00;
					it->cnt = atoi(sv);//��¼����ṹ�������������..
					sv--;
					sv[0] = 0x00;
					it->type |= INI_TYPE_ARRAY;//��Ǵ���������
				}

				//�����µ�ǰ׺..
				if (strlen(stag) + ((ini_list_type*)vname__temp)->count >= slen) {
					//����̫����..
					slen = slen * 2 + ((ini_list_type*)vname__temp)->count;
					char *ss = inimalloc(slen);
					if (ss == NULL)
						goto ERR;
					strcpy(ss, stag);
					inifree(stag);
					stag = ss;
				}
				//�ϳ��µĶ�λ·��..

				strcat(stag, vname);
				strcat(stag, ".");//�ṹ��ڵ��������Ҫ.���Ž�β..
				strcpy(it->name, stag);
				ini_list_type*sb = NULL;
				if (alignsize > 1 && ((offset[0] % alignsize) != 0)) {//�ṹ��ǿ�ƶ��뵽ָ���ֽ�..
					offset[0] += (alignsize - (offset[0] % alignsize));
				}
				it->offset = offset[0];
				if (initypescan(temp->next->data) == INI_TYPE_ALL)
					sb = iniconvert(temp->next, stag, offset);
				else
					sb = iniconvert(temp, stag, offset);
				if (sb != NULL) {//��Щ����ȫ����Ԫ��.
					it->items = sb;
				}
				//ɾ����ѡ��..
				stag[skl] = 0x00;
				if (alignsize > 1 && ((offset[0] % alignsize) != 0)) {//�ṹ���Сǿ�ƶ��뵽����Ա��С..
					offset[0] += (alignsize - (offset[0] % alignsize));
				}
				it->size = (offset[0] - it->offset);//��������ṹ���ж��..
				offset[0] += it->size*(it->cnt - 1);
			}
			temp = sname;
			ini_list_del(vs);
		}
		else {//����stagǰ׺����һ���ڵ���Ϣ..
			filed = temp->next;
			temp = temp->next;
			//����filed�ֶ��Ƿ����,�ָ���.�����˶��ͬ���ͱ����������Ƿ�ʹ��������[]�±�
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
// Qualifier: ��������ʱ�б��ӡȫ���ڵ���Ϣ
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
// Qualifier: ����һ���ṹ�������ַ���������һ��iniitem����ʱ�б�
// Parameter: const char * str
//************************************
ini_list_type* inimakeforstring(const char *str) {
	//����һ���ṹ�������ʱ��Ϣ..
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
	//��Ҫ�����˵���϶����б�ָ�..
	if (strstr(str, ",") != NULL) {
		ini_list_foreach_object(rs, char*, x)
		{
			ini_list_type *xt = (ini_list_type *)x__temp;
			if (xt->next != NULL && xt->count > 1 && xt->data[xt->count - 2] == ',') {
				//��Ҫ����һ���ڵ����ӵ����ڵ�..
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
	//��rsת����iniitem��ʽ���б�..
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
// Qualifier: �������'.'·���ָ��ַ�
// Parameter: const char * name
// Parameter: int index ������index����-1=������һ����-2=�����ڶ���,......
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
	return pos;//�Ҳ���..
}

//************************************
// Method:    ini_find_at
// FullName:  ini_find_at
// Access:    public 
// Returns:   iniitem*
// Qualifier: ������ʱ�б��в���ָ�����͵�ͬ���ֶνڵ���Ϣ
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
// Qualifier: ���л���txt�ı���ʽ�ĵײ�ʵ��
// Parameter: ini_list_type * dslists iniitem����ʱ�б�
// Parameter: const void * handle ��Ҫ���л���Ŀ���ڴ����
// Parameter: uint flag ���л�ѡ��ο�@ini_serialize_flag
// Parameter: char * bpath ���л��Ļ���·��
//************************************
char* iniserialize_txt_ll(ini_list_type *dslists, const void *handle, uint flag,char *bpath) {
	//��Ŀ���ַ����dslists���������������л�
	short blen = 64, slen = 0, tlen = 0, index = 0, alen = 0, re = 0,bpathlen=0;
	char *buf = inimalloc(blen), *ref = (char *)handle;
	//bpath���洢�˴������µĻ���·�����ر�������������������·������..
	bpathlen = (short)strlen(bpath);//���㵱ǰ����·���ĳ���..
	do
	{
		ini_list_foreach_object(dslists, iniitem*, x) {
			//�������ѡ��һ����Ҫ�����ֽ��ڴ棬ȷ�ϵ�ǰ��buf�Ƿ��ܹ�װ��..
			if ((flag&ISF_REMOVE_ZERO) && inimemcmp((char*)handle + x->offset, 0X00, x->size*x->cnt) == 0x00) {
				//����һ��0���ڵ�..���Բ��ý��д洢..
				continue;
			}
			bpath[bpathlen] = 0X00;//���»ָ�����·��
			if ((x->type&INI_TYPE_ALL) == INI_TYPE_STRUCT) {
				
				for (index = 0, alen = x->cnt; index < alen; index++)
				{
					if ((flag&ISF_REMOVE_ZERO) && inimemcmp((char*)handle + x->offset + index*x->size, 0X00, x->size) == 0x00) {
						//����һ��0���ڵ�..���Բ��ý��д洢..
						continue;
					}
					blen = (short)(ini_correction_memory(buf, blen, slen + strlen(x->name) + 16));
					int pos = ini_str_find_last_pp(x->name, -2);
					if (x->type&INI_TYPE_ARRAY) {
						//slen += sprintf(buf + slen, "\n::%s", x->name) - 1;
						//slen += sprintf(buf + slen, "[%d]", index);
						//��Ҫ��·���ֶ������ȷ��������..
						pos = sprintf(bpath + bpathlen, "%s", x->name + pos)-1;
						pos += sprintf(bpath + bpathlen + pos, "[%d]", index);
						//�µĴ������±��·���ϳ����..
					}
					else {
						pos = sprintf(bpath + bpathlen, "%s", x->name + pos) - 1;
					}
					bpath[bpathlen + pos] = 0x00;//������β..�������..
					char *t = iniserialize_txt_ll(x->items, ref + x->size*index, flag|((x->type&INI_TYPE_ARRAY)? ISF_LL_ARRAY:0),bpath);//�����������л�..
					if (t!=NULL) {
						//�õ��˾����������..
						//���кϲ�����..
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
			else {//����һ�������Ķ��󣬿���ֱ�ӽ������л�..
				index = 0; alen = x->cnt;
				blen = (short)(ini_correction_memory(buf, blen, slen + strlen(x->name) + 64));
				if (flag&ISF_LL_ARRAY) {
					//���ڵ��������..
					//��Ҫ���о�������..
					//��ȡx->name�ڵ����Ƶ����һ������·���ϲ�����..
					int pos = ini_str_find_last_pp(x->name,-1);
					if(pos<0)
						continue;
					slen += sprintf(buf + slen, "\n%s%s,%d,%d,%d,", bpath,x->name+pos, x->size, x->cnt, x->type);
				}
				else {
					slen += sprintf(buf + slen, "\n%s,%d,%d,%d,", x->name, x->size, x->cnt, x->type);
				}
				if (x->type& INI_TYPE_STRING) {//��Ϊ�ַ������д���..
											   //��Ҫ��֤blen�����ڴ����2����x.cnt
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
						//������tostringȻ��ϳ�..
						blen = ini_correction_memory(buf, blen, slen + 32);
						re = initostring(x, (void *)((char*)handle + x->offset + x->size*index), buf + slen);
						if (re > 0)
						{
							slen += re;
						}
						if (index < (alen - 1)) {//ʹ�÷ָ������и���..
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
// Returns:   char* ���л��ɹ����txt�ڴ�
// Qualifier: ���л���txt�ı���ʽ�ķ�װ
// Parameter: ini_list_type * dslists iniitem����ʱ�б�
// Parameter: const void * handle ��Ҫ���л���Ŀ���ڴ����
// Parameter: uint flag ���л�ѡ��ο�@ini_serialize_flag
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
// Qualifier: �����л���txt������ڵ�ֵ�����ַ�����
// Parameter: void * handle Ŀ���ڴ�
// Parameter: iniitem * item ����ʱ��Ϣ
// Parameter: const char * value txt����Ķ�Ӧ�ֶε�ֵ���ַ���
//************************************
int inidescrialize_txt_calc_value(void *handle, iniitem *item, const char *value) {
	//DBGF("\r\ndesc value:name=%s , type=%d ,offset = %d, value=%s", it->name, it->type,offset+it->offset, value);
	//�������ͽ��з����л�����..
	uint type = item->type&INI_TYPE_ALL;
	char*str = handle;
	
	ini_list_type *ss = ini_list_head();
	int len = strlen(value), pos = 0, errocde = 0;
	if (type == INI_TYPE_STRING) {//�ַ�����Ҫ�������⴦��,��ԭת���ַ�..
		pos = 0;
		for (int i = 0; i < len && item->cnt>(i-1); i++)
		{
			if (value[i] == '\\') {
				//����һ��ת���ַ�..
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
		//����һ������Ԫ��..
		//Ѱ�ҿ�ʼ�ͽ�����λ��[1,2,3,4,5]//����������
		//��⣬����ȫ��Ԫ��һ�����ķ����л���Ŀ���ڴ�..
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
		s += 1;//��s��ʼ����e��β���м��Ԫ����','�ַ��ָ�..
		ini_list_type *arrays = __ini_ll_strtok(s, e - s, 1, ", ");
		if (arrays == NULL) {
			errocde = -3; goto OVER;
		}
		if (arrays->count > 0) {
			ini_list_join(ss, arrays);
			ini_list_reset(arrays);
		}
		ini_list_del(arrays);//ȫ�����������ss�б�ֱ���ͷ�arraysָ�뼴��
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
		handle = ((char*)handle)+ (uint)item->size;//����ƶ��ڴ�..
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
// Qualifier: ����һ���ֶε�ֵ��������Ŀ���ڴ�
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
		//��һ����ƥ��
		ini_list_foreach_object(paths, char*, x) {
			vindex++;
			type = INI_TYPE_STRUCT;
			if (index == (paths->count - 1)) {
				//���һ���ڵ�..
				//��Ҫ���ҵ��ǻ����ڵ�..
				type = 0;
			}
			lcur = base;
			spos = 0;
			epos = 0;
			pos  = 0;
			if ((ctmp = strchr(x, '[')) != NULL) {
				pos = ((uint)ctmp - (uint)x);//����������±꿪ʼ��λ��..
				if ((ctmp = strchr(ctmp, ']')) != NULL) {
					epos = ((uint)ctmp - (uint)(x));
					//����������pos�±�..
					x[epos] = 0X00;
					spos = atoi(x + pos + 1);//��ȡ����·�����±�..
					x[epos] = ']';
				}
				x[pos] = 0x00;//Ĩȥ·�����±�Ĳ���..
				(((ini_list_type*)x__temp)->count) = pos+1;//��������..
			}
			ini_list_foreach_object(base, iniitem*, it) {
				lindex = -1;
				if ((it->type&INI_TYPE_STRUCT) == INI_TYPE_STRUCT) {//����һ���ṹ��ڵ�..
					lindex = -2;
				}
				lindex = ini_str_find_last_pp(it->name, lindex);
				if(lindex <0)
					continue;
				vname = it->name + lindex;
				//��ȡ���ڵ�������..
				if (memcmp(x, vname+1, (((ini_list_type*)x__temp)->count - 1)) == 0x00) {
					//����..
					//��Ҫ�ٴ�ȷ���ַ����Ƿ�������ƥ��..
					if (strlen(vname + 1) != ((((ini_list_type*)x__temp)->count) - ((it->type&INI_TYPE_STRUCT) == INI_TYPE_STRUCT ? 0 : 1))) {
						//�ַ���δ�������Ľ���ƥ��..
						continue;//������Ҫ��Ŀ����..
					}
					if ((it->type&INI_TYPE_STRUCT) != INI_TYPE_STRUCT && paths->count!=vindex) {
						continue;//���Ͳ�ƥ��..Ŀǰ����Ҫ�ṹ��ڵ�
					}
					if ((it->type&INI_TYPE_STRUCT) == INI_TYPE_STRUCT && paths->count == vindex) {
						continue;//���Ͳ�ƥ��..Ŀǰ��Ҫ�����ڵ�..
					}

					base = it->items;
					//�����ڴ�offset..
					offset += it->size*spos;//���Ǽ�¼����ƫ��
					if ((it->type&INI_TYPE_STRUCT) != INI_TYPE_STRUCT) {
						//����һ�������Ľڵ�..
						//ƥ�������Ƿ�һ��..
						if (item->type != it->type || it->size != item->size)
						{
							errcode = -1; break;
						}//���Ͳ�һ���޷����з����л�..
						if (it->cnt < spos)
						{
							errcode = -2; break;
						}//װ���£����������ʾû���㹻���ڴ���..
						//��ʼ���з����л�Ŀ���ڴ�δ:it->offset+offset+handle
						errcode = inidescrialize_txt_calc_value((char*)handle + it->offset + offset, it, value);
						if (errcode > 0)
							it->hit = 1;//�����л��ɹ�..
					}
					break;
				}
			}
			if(errcode<0)
				break;
			if (lcur == base) {
				//�޷�ƥ���·��..
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
// Qualifier: �������ʱ�б���iniitem�����б��
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
// Qualifier: ���л��ṹ�嵽txt�ı���ʽ���˸�ʽ����ini�����ļ�ʹ�ã�����vim����̨���������ļ���
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
		memset(handle, 0, size);//Ĭ������ڴ�..
	}
	if (flag&ISF_LL_MARK_HIT) {
		inidescrialize_txt_hit_clear(dslists);//������м�����..
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
			line = (char*)msg + i;//����Ϊj-i
			l = j - i;
			do
			{
				//��ʼ���н������еľ�������..
				//ֱ��ʹ���������ݹ���һ��������..
				for (x=0,s=0; x < l&&list->count<4; x++)
				{
					if (line[x] == ',') {
						//�������������..
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
				//�����ݽ��зָ���..
				//�ж������Բ���..
				if(list->count<4|| list->next->data[0]!='.')
					continue;
				//����һ��iniitem����..
				memset(&item, 0, sizeof(item));
				item.name = list->next->data;
				item.size = atoi(ini_list_find_at(list,1)->data);
				item.cnt  = atoi(ini_list_find_at(list, 2)->data);
				item.type = atoi(ini_list_find_at(list, 3)->data);
				//�õ���һ���ڵ�..
				//ȷ���������Ƿ����[]���Դ�������Ŀ���ڴ�..
				//DBGF("\r\ndesc:name=%s,size=%d,type=%d,type=%d,value=%s", item.name, item.size, item.cnt, item.type,value);
				re=inidescrialize_txt_prase(dslists, handle, &item, value);//�����л�����..
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

//INIJSONָʾ�Ƿ��ṩcjson���л�����
#if defined(INIJSON) && INIJSON!=0
//************************************
// Method:    _iniserialize_to_json_object
// FullName:  _iniserialize_to_json_object
// Access:    public 
// Returns:   cJSON*
// Qualifier: ���л��ṹ����cjson����
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
		value = ( inivalue *)(x->offset + (handle));//Ŀ���ڴ�..

		pos = ini_str_find_last_pp(x->name, -1- __NAME_OFFSET_X(x));
		nlen = strlen(x->name);
		INI_ASSERT((nlen - pos - 1 - __NAME_OFFSET_X(x)) < __NAMEMAXLEN, -3);
		memcpy(name, x->name + pos + 1, nlen - pos - 1- __NAME_OFFSET_X(x));
		name[nlen - pos - 1- __NAME_OFFSET_X(x)] = 0X00;


		//����ÿһ���ڵ㣬����json��..
		if ((x->type & INI_TYPE_ALL) == INI_TYPE_STRUCT) {
			
			cJSON *item = NULL;
			if ((x->type&INI_TYPE_ARRAY) == 0x00) {//��������ڵ�..
				item = _iniserialize_to_json_object(x->items, handle, flag, err);
			}
			else {//����ڵ�..���б�������ÿһ���ڵ�..
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
					//�������͵�����..
					//��֧��..
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
// Qualifier: ���л��ṹ����json�ַ���
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
// Qualifier: ���ַ��������л���cjson�ṹ
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
	//����ÿһ��cjson�ڵ�..
	for (cJSON *item = root->child; item; item=((item==NULL)?NULL:item->next))
	{
		//���Ҷ�Ӧ��iniitem�ڵ�..
		iitem = ini_find_at(dslists, item->string, -1);
		if (iitem == NULL)
		{
			if (errcnt)
				errcnt[0] += 1;
			continue;//������޷�ƥ��..
		}
		if (item->type == cJSON_Object || (item->type == cJSON_Array && item->child->type== cJSON_Object)) {//�ṹ����߽ṹ������..
			//��dslist�в���Ŀ��ڵ�..
			if (item->type == cJSON_Object) {
				INI_ASSERT((stmp=_inideserialize_from_json(iitem->items, item, handle, size, errcnt, flag)) >= 0, -2);
				scnt += stmp;
			}
			else {//�ṹ������..
				int i = 0;
				for (cJSON*s = item->child; s!=NULL&&i<iitem->cnt;s=s->next, i++)
				{//����ѭ������..
					INI_ASSERT((stmp=_inideserialize_from_json(iitem->items, s, handle + iitem->size*i, size, errcnt, flag)) >= 0, -3);
					scnt += stmp;
				}
			}
		}
		else {//��������..��ʼ���н���..
			//��ȡ����ڵ��·��..���ϳɣ����Ҷ�Ӧ��dlist�ڵ�..
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
					maxlen = maxlen >= iitem->cnt ? (iitem->cnt - 1) : (maxlen);//��ȡ���Դ�ŵ����ߴ�..
					memcpy(value->strvalue, item->valuestring, maxlen);
					value->strvalue[maxlen] = 0x00;
					break;//��Ϊcnt>1��Ҫ����ȥ..
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
// Returns:   int �ɹ�ƥ�䲢���µ��ֶ�����
// Qualifier: ���ַ��������л����ṹ��
// Parameter: ini_list_type * dslists
// Parameter: const char * json
// Parameter: void * handle
// Parameter: uint size
// Parameter: int * errcnt �ַ�����δ����dslists����������ʱ�ɹ�ƥ����ֶ�����
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
// Qualifier: ����
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
		inidel(dlist);
	}

}