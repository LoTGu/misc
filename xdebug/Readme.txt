Xdebug系统
——
 									Be a ’ to make impossilbe to I’m possible！ 

Xdebug系统即Extended debug 的意思，扩展的debug系统提供了更加完备的函数。可以打印出该行信息所在文件，函数名以及行号。另外在CGI一类，进行过标准输出重定向的应用程序中，标准的printf将无法满足需求。Xdebug系统将输出信息直接定向到了物理终端/dev/console中。另外不同的信息可以标记上不同的颜色，字体也可以选择粗体斜体等，对于显示一些特别的信息比如警告等是很有帮助的。此外系统中提供了一个类似于Wireshark中显示二进制信息的函数，对于一些数据包之类的debug，通过此函数可以很清晰的打印出其中的内容。

Xdebug系统有两个文件组成，xdebug.h和xdebug.c。

xdebug.h定义了函数声明，以及Xdebug系统的全局开关。xdebug.c中定义了函数的实体。

1.	Xdebug 系统中的Hello world?
位于xdebug.h中的#define XDEBUG宏是Xdebug系统的全局开关。如果需要关闭，直接注释掉此函即可，也即//#define XDEBUG，此时系统中的引用的Xdebug函数将为空，此时Xdebug系统不会被编译进源码，代码大小不会增加。

hello.c：
0	include  “xdebug.h”
1	int main()
2   {
3		xprintf("hello world!\n");
4 		return 0;
5	}

gcc –o hello xdump.c hello.c
#./hello
=xdump_test.c(3):main= hello world!
xdump_test.c指明信息所在的函数，(3)指明信息所在的行号，main指明了此信息所在的函数的名称。

在一个复杂的应用程序中，将xdebug.h放到include中，xdebug.c放到源码文件夹下，在需要debug的.c文件中添加声明include <include/xdebug.h>，在编译的Makefile中的依赖中根据情况添加xdebug.c 或者xdebug.o。这是一种通用的方法。

2.	xprintf 函数
xprintf函数是Xdebug系统的核心函数，它相当于printf，也即它可以接受printf可以接受的所有参数。比如：
printf(“%s %d %x%lu\n”, “hello”, 1, 2, 3);

3.	如何定制输出的颜色和字体格式（console必须支持）
xcprintf函数，满足了这种要求，可以通过第一个参数定制。
例如xcprintf(“red gree bold”, “%s”, “hello\n”);
第一个参数中的字符串分别指定了foreground background和textstyle。
foreground和background可以取: red/green/yellow/blue/magenata/cyan。
textstyle可以取: bold。

它们都是可选的，但是必须至少提供一个参数。另外要指定background，必须提供foreground，foreground就是字体的颜色。

Xdebug系统在xdebug.h中自定义了一些常用的需要颜色显示的宏。
#define xwprintf(fmt...)	xcprintf("red bold", fmt)
#define xiprintf(fmt...)	xcprintf("green bold", fmt)

xwprintf意味着输出warning信息，也即输出红色粗体的信息。
xiprintf意味着输出info信息，也即输出绿色粗体的信息。

你可以像使用xprintf一样使用它们。而无需在指定颜色和字体格式字符参数。
你也可以根据具体需要定义自己的宏函数。

4.	如何以二进制形式显示信息
xdumphex函数提供了此功能，第一个参数是需要输出的信息的指针，第二个参数提供了信息长度。
void xdumphex(const void *inbuf, unsigned int len)；

例如：
char teststr[100] = 
"1234567890abcdefghijlmnopqrstuvwxyz!@#$%^&*()_+-=;'<>?,./";
xdumphex(teststr, 100);

输出信息为：
0000  31 32 33 34 35 36 37 38  39 30 61 62 63 64 65 66 12345678 90abcdef
0001  67 68 69 6a 6c 6d 6e 6f  70 71 72 73 74 75 76 77 ghijlmno pqrstuvw
0002  78 79 7a 21 40 23 24 25  5e 26 2a 28 29 5f 2b 2d xyz!@#$% ^&*()_+-
0003  3d 3b 27 3c 3e 3f 2c 2e  2f 00 00 00 00 00 00 00 =;'<>?,. /.......
0004  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 ........ ........
0005  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 ........ ........
0006  00 00 00 00                                      ....

在xdebug.c中定义了#define PRINT printf，如果你需要将此函数移植到内核（在内核它很有用），那么只需要赋值该函数的定义，然后更改#define PRINT printk即可。
5.	如何关闭行号等信息
xdebug.h 中定义了宏XDEBUG_PURE，打开该宏将关闭行号等信息。

6.	如何在CGI等程序中使用
在CGI等一类对标准输出stdout进行过重定向的操作的程序中使用Xdebug系统，只需打开xdebug.h 中XDEBUG_CGI宏

7．xerror和xdie宏函数
xerrorr尝试将信息通过stderr输出，信息输出为红色，并尝试使用stderror解析错误信息并输出。
xdie与xerrorr类似，不过它在抛出信息后将使用abort函数终端程序的执行。

8．其他
xendian是一个测试系统字节序的函数。
目前xcprintf不支持行号等信息的输出。xcprintf，xerror和xdie因为没有对重定向处理，所以不支持类CGI程序。

