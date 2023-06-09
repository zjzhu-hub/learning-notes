#include <stdio.h>
#include <stdlib.h>

int main() {
    int size, count, i;
    while (1) {
        printf("请输入要申请的内存大小（单位为KB），输入0退出：");
        scanf("%d", &size);
        if (size <= 0) {
            break;
        }
        printf("请输入要申请内存的次数：");
        scanf("%d", &count);
        for (i = 0; i < count; i++) {
            void* p = malloc(size * 1024);
            if (p == NULL) {
                printf("内存申请失败！\n");
                break;
            }
            printf("成功申请 %dkB 内存\n", size);
        }
        printf("共申请了 %d 次内存\n", i);
    }
    return 0;
}
