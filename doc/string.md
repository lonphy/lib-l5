## #include <string.h>

#### `void * memchr (const void * buf, int ch, size_t count);`
    从buf所指内存区域的前count个字节查找字符ch。
    说明: 当第一次遇到字符ch时停止查找。如果成功，返回指向字符ch的指针；否则返回NULL

#### `int memcmp (const void * buf1, const void * buf2, size_t count);`
    比较内存区域buf1和buf2的前count个字节
    - 当buf1<buf2时，返回值小于0
    - 当buf1==buf2时，返回值=0
    - 当buf1>buf2时，返回值大于0

#### `void * memcpy (void * dest, const void * src, size_t n);`
    从源src所指的内存地址的起始位置开始拷贝n个字节到目标dest所指的内存地址的起始位置中
    返回指向dest的指针

#### `void * memmove (void * dest, const void * src, size_t n);`
    用于从src拷贝count个字节到dest，如果目标区域和源区域有重叠的话，
    memmove能够保证源串在被覆盖之前将重叠区域的字节拷贝到目标区域中。
    但复制后src内容会被更改。但是当目标区域与源区域没有重叠则和memcpy函数功能相同

#### `void * memset (void * buf, int ch, size_t count);`
    将buf所指内存区域的count个字节用ch低8bit填充

#### `char 	*strcat (char * dest, const char * src);`
    把src所指向的字符串（包括“\0”）复制到dest所指向的字符串后面（删除*dest原来末尾的“\0”）
    要保证*dest足够长，以容纳被复制进来的*src。*src中原有的字符不变。返回指向dest的指针

#### `char 	*strchr (const char * str, int val);`
    返回首次出现val的位置的指针，返回的地址是被查找字符串指针开始的第一个与val相同字符的指针，如果str中不存在val则返回NULL

#### `int strcmp (const char * s1, const char * s2);`
    比较两个字符串:
    - 当s1<s2时，返回为负数
    - 当s1==s2时，返回值= 0
    - 当s1>s2时，返回正数
    即：两个字符串自左向右逐个字符相比（按ASCII值大小相比较），直到出现不同的字符或遇'\0'为止

#### `char * strcpy (char * dest, const char * src);`
    把从src地址开始且含有'\0'结束符的字符串复制到以dest开始的地址空间，返回值的类型为char*。

#### `size_t strcspn (const char * s, const char * reject);`
    获取字符串s起始位置起字符不在reject中任何字符的长度

#### `char * strerror (int err);`
    获取标准错误的字符串指针

#### `size_t strlen (const char * s);`
    返回s的长度，不包括结束符NULL

#### `char *strncat (char * dest, const char * src, size_t n);`
    把src所指字符串的前n个字符添加到dest所指字符串的结尾处，并覆盖dest所指字符串结尾的'\0'

#### `int strncmp (const char * s1, const char * s2, size_t n);`
    比较s1, s2前n个字节， 碰到'\0'结束

#### `char * strncpy (char * dest, const char * src, size_t n);`
    把src所指向的字符串中以src地址开始的前n个字节复制到dest所指的数组中，并返回被复制后的dest

#### `char * strpbrk (const char * s1, const char * s2);`
    在s1中找出最先含有搜索字符串s2中任一字符的位置并返回，若找不到则返回空指针

#### `char * strrchr (const char * str, int c);`
    查找c在str中末次出现的位置(右侧开始),并返回这个位置的地址。如果未找到c，将返回NULL。
    使用这个地址返回从最后一个字符c到str末尾的字符串

#### `size_t strspn (const char * s, const char * accept);`
    返回字符串s开头连续包含字符串accept内的字符数目

#### `char *strstr (const char * s1, const char * s2);`
    判断s2是否为s1的子串，若是则返回s2在s1中首次出现的指针位置，若不是返回NULL

#### `char * strtok (char * s, const char * delim);` (非线程安全, 避免使用)
    分解s为一组字符串。delim为分隔符字符(如果多于1个，则每个都是分隔符)
    首次调用时，s指向要分解的字符串，之后再次调用要把s设成NULL

#### `char * strsep(char ** s1, const char * delim);`
    从s1指向的位置起向后扫描，遇到delim指向的字符串中的字符后，将此字符替换为NULL，返回s1指向的地址
