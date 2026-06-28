/*
 * 综合填空第 4 题 · 成绩单文件读写 demo
 *
 * 对应作业逻辑：从 dat.txt 读 30 人 4 科成绩 → 算平均分 → 写入 out.txt
 *
 * 编译运行（在 demo 目录）：
 *   chcp 65001
 *   gcc score_file_demo.c -o score_file_demo.exe
 *   .\score_file_demo.exe
 *
 * 文件都在 demo 文件夹（不用 d:\，避免权限/路径问题）
 */
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define getcwd _getcwd
static void setup_console_utf8(void)
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
}
#else
#include <unistd.h>
static void setup_console_utf8(void) { }
#endif

#define DAT_FILE "dat.txt"
#define OUT_FILE "out.txt"
#define STUDENTS 30
#define SUBJECTS 4

static void wait_enter(const char *hint)
{
    int c;
    printf("\n>>> %s\n>>> 按【回车】继续...", hint);
    fflush(stdout);
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

static void show_cwd(void)
{
    char cwd[512];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("【工作目录】%s\n\n", cwd);
}

/* 把 out.txt 内容打印到屏幕，方便对照 */
static void print_out_file(void)
{
    FILE *fp = fopen(OUT_FILE, "r");
    char line[256];
    if (fp == NULL) {
        printf("（无法打开 %s）\n", OUT_FILE);
        return;
    }
    printf("---------- %s 内容预览 ----------\n", OUT_FILE);
    while (fgets(line, sizeof(line), fp) != NULL)
        printf("%s", line);
    printf("---------- 预览结束 ----------\n\n");
    fclose(fp);
}

int main(void)
{
    FILE *fp1, *fp2;
    int score[STUDENTS][SUBJECTS];
    int i, j, sum;
    float ave;
    int count = 0;

    setup_console_utf8();

    printf("============================================\n");
    printf("  综合填空第4题 · 成绩单文件读写\n");
    printf("============================================\n\n");
    show_cwd();

    /* ========== ①② 打开 dat.txt 只读 ========== */
    printf("========== 第1步：fopen 读文件 ==========\n");
    printf("代码：fp1 = fopen(\"%s\", \"r\");\n", DAT_FILE);
    fp1 = fopen(DAT_FILE, "r");   /* ① 路径  ② "r" */
    if (fp1 == NULL) {
        printf("打开 %s 失败！请确认 dat.txt 在 demo 目录下。\n", DAT_FILE);
        return 1;
    }
    printf("【成功】fp1 已打开。\n\n");
    wait_enter("打开资源管理器看看 dat.txt 原始数据");

    /* ========== ④⑤ 循环 fscanf 读入 ========== */
    printf("\n========== 第2步：!feof + fscanf 读数据 ==========\n");
    printf("代码：while (!feof(fp1)) { fscanf(...); }\n\n");

    i = 0;
    while (!feof(fp1)) {   /* ④ 作业标准写法 */
        if (fscanf(fp1, "%d %d %d %d",
                   &score[i][0], &score[i][1],
                   &score[i][2], &score[i][3]) == SUBJECTS) {
            printf("  读到第 %2d 人: %d %d %d %d\n",
                   i + 1, score[i][0], score[i][1], score[i][2], score[i][3]);
            i++;
            if (i >= STUDENTS)
                break;
        }
    }
    count = i;
    printf("\n【共读取 %d 条记录】\n", count);
    wait_enter("读完了，准备写入 out.txt");

    /* ========== ③ 打开 out.txt 只写（会清空旧内容）========== */
    printf("\n========== 第3步：fopen 写文件 ==========\n");
    printf("代码：fp2 = fopen(\"%s\", \"w\");\n", OUT_FILE);
    fp2 = fopen(OUT_FILE, "w");   /* ③ fopen + "w" */
    if (fp2 == NULL) {
        printf("打开 %s 失败！\n", OUT_FILE);
        fclose(fp1);
        return 1;
    }

    /* 表头 */
    fprintf(fp2, "学号\t语文\t数学\t英语\t计算机\t平均\n");

    /* ⑥⑦⑧ 循环写学号、各科、累加求平均 */
    for (i = 0; i < count; i++) {
        fprintf(fp2, "%d\t", i + 1);   /* ⑥ 学号从 1 开始 */
        sum = 0;
        for (j = 0; j < SUBJECTS; j++) {
            sum += score[i][j];        /* ⑦ 累加 */
            fprintf(fp2, "%d\t", score[i][j]);  /* ⑧ 输出单科 */
        }
        ave = sum / 4.0;               /* 用 4.0 避免整数除法 */
        fprintf(fp2, "%6.2f\n", ave);
    }

    printf("【成功】已写入 %d 行到 %s\n", count, OUT_FILE);

    /* ⑨ 关两个文件 */
    fclose(fp1);
    fclose(fp2);   /* ⑨ */
    printf("【已 fclose】fp1 和 fp2 都关闭了。\n\n");

    print_out_file();
    wait_enter("用 VS Code 打开 out.txt，对照上面预览");

    printf("\n============================================\n");
    printf("填空对照速记：\n");
    printf("  ① \"dat.txt\"  ② \"r\"\n");
    printf("  ③ fopen(\"out.txt\",\"w\")\n");
    printf("  ④ !feof(fp1)  ⑤ fscanf\n");
    printf("  ⑥ i+1  ⑦ sum+=score[i][j]\n");
    printf("  ⑧ fprintf(fp2,...)  ⑨ fclose(fp2)\n");
    printf("============================================\n");

    return 0;
}
