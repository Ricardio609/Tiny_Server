/*
 * encode: utf-8
 * @author Ricardo609
 * @date 2022-06-12
 */

#include <unistd.h>
#include "webServer.h"

int main() {
    /* 守护进程
     *daemon()函数主要用于希望脱离控制台，以守护进程的形式在后台运行的程序
     *第一个参数为0时，daemon将更改进程的根目录为root(“/”)，非0时保持当前执行目录不变
     *第二个参数为0时，会将标准输入(0)，标准输出(1)，标准错误(2)重定向到/dev/null，否则保持原有标准输入(0)，标准输出(1)，标准错误(2)不变
     */
    //daemon(1, 0);

    WebServer server(1316, 3, 60000, false, 4);
    server.Start();
}