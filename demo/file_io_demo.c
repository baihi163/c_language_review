/*
 * 文件读写三模式体验 · 慧眼识猪日志版
 *
 * 编译运行（在 demo 目录下打开终端）：
 *   gcc file_io_demo.c -o file_io_demo
 *   chcp 65001
 *   .\file_io_demo.exe
 *
 * 生成的 pig_log.txt 就在本 demo 文件夹里。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>  /* SetConsoleOutputCP：设置控制台编码 */
#include <direct.h>   /* _getcwd：获取当前工作目录 */
#define getcwd _getcwd

/* Windows 终端默认 GBK，源码是 UTF-8 时会乱码，启动时切到 UTF-8 */
static void setup_console_utf8(void)
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
}
#else
#include <unistd.h>
static void setup_console_utf8(void) { }
#endif

#define LOG_FILE "pig_log.txt"

/* 按回车继续，方便你去资源管理器里打开 pig_log.txt 对照 */
static void wait_enter(const char *hint)
{
    int c;
    printf("\n>>> %s\n>>> 按【回车】继续...", hint);
    fflush(stdout);
    /* 清掉输入缓冲里残留字符，再等待一次回车（Windows/VS Code 终端更稳） */
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/* 打印当前目录，避免「文件写哪儿了找不到」 */
static void show_where_we_are(void)
{
    char cwd[512];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("【当前工作目录】%s\n", cwd);
        printf("【日志文件路径】%s\\%s\n\n", cwd, LOG_FILE);
    } else {
        printf("【日志文件】相对路径 %s（与 exe 的工作目录相同）\n\n", LOG_FILE);
    }
}

/* 用只读方式把文件内容打印到屏幕 */
static void print_file_content(const char *title)
{
    FILE *fp = fopen(LOG_FILE, "r");
    char buffer[100];

    printf("---------- %s ----------\n", title);
    if (fp == NULL) {
        printf("（文件不存在或无法打开）\n\n");
        return;
    }
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("  | %s", buffer); /* buffer 里通常已带换行 */
    }
    printf("---------- 文件结束 ----------\n\n");
    fclose(fp);
}

int main(void)
{
    FILE *fp;
    char buffer[100];

    setup_console_utf8();

    printf("============================================\n");
    printf("  C 语言文件 fopen 模式体验 · pig_log.txt\n");
    printf("============================================\n\n");
    show_where_we_are();

    /* ========== 第一幕："w" 只写/覆盖 ========== */
    printf("========== 第一幕：\"w\" (write · 只写/覆盖) ==========\n");
    printf("规则：没有文件就新建；有文件就【整份清空】再写。\n\n");

    fp = fopen(LOG_FILE, "w");
    if (fp == NULL) {
        printf("文件打开失败！\n");
        return 1;
    }
    fprintf(fp, "第1条记录：发现1号猪，状态：吃饲料\n");
    fclose(fp);

    printf("【操作完成】已写入第 1 条记录。\n");
    print_file_content("第一幕后 · 磁盘上的内容");
    wait_enter("请用 VS Code 或资源管理器打开 pig_log.txt，确认只有 1 行");

    /* ========== 第二幕："a" 追加 ========== */
    printf("\n========== 第二幕：\"a\" (append · 追加) ==========\n");
    printf("规则：在文件【末尾】接着写，不会删掉前面的内容。\n\n");

    fp = fopen(LOG_FILE, "a");
    if (fp == NULL) {
        printf("文件打开失败！（\"a\" 一般要求文件已存在，先跑过第一幕）\n");
        return 1;
    }
    fprintf(fp, "第2条记录：发现2号猪，状态：打架\n");
    fprintf(fp, "第3条记录：发现3号猪，状态：睡觉\n");
    fclose(fp);

    printf("【操作完成】已追加第 2、3 条记录。\n");
    print_file_content("第二幕后 · 磁盘上的内容（应有 3 行）");
    wait_enter("再看一眼 pig_log.txt，应该变成 3 行了");

    /* ========== 第三幕："r" 只读 ========== */
    printf("\n========== 第三幕：\"r\" (read · 只读) ==========\n");
    printf("规则：只能读；文件不存在时 fopen 返回 NULL。\n\n");

    fp = fopen(LOG_FILE, "r");
    if (fp == NULL) {
        printf("文件打开失败！(文件不存在时用 \"r\" 就会这样)\n");
        return 1;
    }

    printf("【fgets 逐行读取】:\n");
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("  读取到: %s", buffer);
    }
    fclose(fp);
    printf("【读取完毕】注意：\"r\" 模式不能 fprintf 写回文件。\n\n");
    wait_enter("第三幕结束，文件内容未被程序改动");

    /* ========== 第四幕：再次 "w" · 亲眼见证清空 ========== */
    printf("\n========== 第四幕：再次 \"w\" · 覆盖演示 ==========\n");
    printf("⚠️  下面会用 \"w\" 再打开一次，前面 3 条记录会【全部消失】！\n\n");
    wait_enter("做好心理准备后按回车，执行覆盖");

    fp = fopen(LOG_FILE, "w");
    if (fp == NULL) {
        printf("文件打开失败！\n");
        return 1;
    }
    fprintf(fp, "（覆盖后）只剩这一条新记录\n");
    fclose(fp);

    printf("【操作完成】\"w\" 再次打开 = 清空 + 重写。\n");
    print_file_content("第四幕后 · 原来的 3 条已经没了");

    /* ========== 第五幕："r+" 不存在会失败（可选知识点） ========== */
    printf("========== 第五幕：小实验 · \"r\" 打开不存在的文件 ==========\n");
    remove("no_such_pig.txt"); /* 确保不存在 */
    fp = fopen("no_such_pig.txt", "r");
    if (fp == NULL) {
        printf("预期结果：fopen(\"no_such_pig.txt\", \"r\") 返回 NULL ✓\n");
        printf("        → 所以读之前要么先 \"w\"/\"a\" 创建，要么检查 NULL。\n");
    } else {
        fclose(fp);
    }

    printf("\n============================================\n");
    printf("体验结束！模式速记：\n");
    printf("  w = 覆盖写（危险，像重写整本日志）\n");
    printf("  a = 追加写（日记本接着写）\n");
    printf("  r = 只读（文件必须存在）\n");
    printf("============================================\n");

    return 0;
}
