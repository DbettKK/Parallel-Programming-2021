#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<memory.h>

#define MAX_PRODUCTION2_NUM 128
#define MAX_PRODUCTION1_NUM 128
#define MAX_VN_NUM 128
#define MAX_VT_NUM 128
#define MAX_STRING_LENGTH 1024

typedef struct SubTree {
    unsigned vn[MAX_VN_NUM];
    int keys[MAX_VN_NUM];
    int mark[MAX_VN_NUM];
    int key_cnt;
} SubTree;

int vnNum;
int production1[128][MAX_VT_NUM];
int production1_cnt[128];
SubTree dp[MAX_STRING_LENGTH][MAX_STRING_LENGTH];
int vnIndex[MAX_VN_NUM][MAX_VN_NUM][MAX_PRODUCTION2_NUM];
int vnIndex_num[MAX_VN_NUM][MAX_VN_NUM];
char solve[MAX_STRING_LENGTH];
int slen;
void input() {
    int production1_num, production2_num;
    freopen("input2.txt", "r", stdin);
    scanf("%d\n", &vnNum);
    scanf("%d\n", &production2_num);
    for (int i = 0; i < production2_num; i++) {
        int key, value1, value2;
        scanf("<%d>::=<%d><%d>\n", &key, &value1, &value2);
        vnIndex[value1][value2][vnIndex_num[value1][value2]++] = key;
    }
    scanf("%d\n", &production1_num);
    for (int i = 0; i < production1_num; i++) {
        char key;
        int value;
        scanf("<%d>::=%c\n", &value, &key);
        production1[key][production1_cnt[key]++] = value;
    }
    scanf("%d\n", &slen);
    scanf("%s\n", solve);
    for (int i = 0; i < slen; i++) {
        char c = solve[i];
        for (int index = 0; index < production1_cnt[c]; ++index) {
            int integer = production1[c][index];
            dp[i][i].vn[integer] = 1;
            if (!dp[i][i].mark[integer]) {
                dp[i][i].keys[dp[i][i].key_cnt++] = integer;
                dp[i][i].mark[integer] = 1;
            }
        }
    }
}

int main() {
    input();
    clock_t start, end;
    start = clock();
    int tcnt = 0, count = 0;
    for (int len = 2; len <= slen; len++) {
        printf("%d\n", len);
        for (int left = 0; left <= slen - len; left++) {
            for (int right = left + 1; right < left + len; right++) {
                int left_cnt = dp[left][right - 1].key_cnt;
                int right_cnt = dp[right][left + len - 1].key_cnt;
                tcnt++;
                if (left_cnt * right_cnt == 0) {
                    count++;
                }
                for (int leftIndex = 0; leftIndex < dp[left][right - 1].key_cnt; leftIndex++) {
                    int leftVn = dp[left][right - 1].keys[leftIndex];
                    int leftNum = dp[left][right - 1].vn[leftVn];
                    for (int rightIndex = 0; rightIndex < dp[right][left + len - 1].key_cnt; rightIndex++) {
                        int rightVn = dp[right][left + len - 1].keys[rightIndex];
                        int rightNum = dp[right][left + len - 1].vn[rightVn];
                        if (vnIndex_num[leftVn][rightVn] <= 0) continue;
                        int tmpNum = vnIndex_num[leftVn][rightVn];
                        int total = leftNum * rightNum;
                        for (int index = 0; index < tmpNum; index++) {
                            int i = vnIndex[leftVn][rightVn][index];
                            if (!dp[left][left + len - 1].mark[i]) {
                                dp[left][left + len - 1].keys[dp[left][left + len - 1].key_cnt++] = i;
                                dp[left][left + len - 1].mark[i] = 1;
                            }
                            dp[left][left + len - 1].vn[i] += total;
                        }
                    
                    }
                }
            }
        }
    }
    if (dp[0][slen - 1].vn[0] != 0) {
        printf("%u\n", dp[0][slen - 1].vn[0]);
    } else {
        printf("%u\n", 0);
    }
    end = clock();
    printf("%ld\n", (end - start));
    printf("%lf%%", (double)count / tcnt * 100);
}