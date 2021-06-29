#include<stdio.h>

#define MAX_PRODUCTION2_NUM 512
#define MAX_PRODUCTION1_NUM 128
#define MAX_VN_NUM 128
#define MAX_VT_NUM 128
#define MAX_STRING_LENGTH 1024

typedef struct SubTree {
    /* trees[vn] 代表可由 vn 推导而出的语法树数目 */
    unsigned trees[MAX_VN_NUM];
} SubTree;

/* <parent>::=<child1><child2> */
typedef struct VnProduction {
    int parent, child1, child2; // vn
} VnProduction;

/* <parent>::=<child> */
typedef struct VtProduction {
    int parent; // vn
    char child; // vt
} VtProduction;

/* 
    dp数组 
    dp[i][j].trees[vn] 代表 
    solve(i,j) 可由 vn 推导而出的语法树数目 
 */
SubTree dp[MAX_STRING_LENGTH][MAX_STRING_LENGTH];

int vnProductions_cnt, vtProductions_cnt;
VtProduction vtProductions[MAX_PRODUCTION1_NUM];
VnProduction vnProductions[MAX_PRODUCTION2_NUM];

int slen;
char solve[MAX_STRING_LENGTH];

void input() {
    freopen("input.txt", "r", stdin);
    int vnNum;
    scanf("%d\n", &vnNum);
    scanf("%d\n", &vnProductions_cnt);
    for (int i = 0; i < vnProductions_cnt; i++)
        scanf("<%d>::=<%d><%d>\n", &vnProductions[i].parent, &vnProductions[i].child1, &vnProductions[i].child2);
    scanf("%d\n", &vtProductions_cnt);
    for (int i = 0; i < vtProductions_cnt; i++)
        scanf("<%d>::=%c\n", &vtProductions[i].parent, &vtProductions[i].child);
    scanf("%d\n", &slen);
    scanf("%s\n", solve);
}

int main() {
    input();
    /* 初始化 对于 len = 1 的情况 */
    for (int i = 0; i < slen; i++) {
        char c = solve[i];
        for (int vt_index = 0; vt_index < vtProductions_cnt; vt_index++) {
            VtProduction tmp = vtProductions[vt_index];
            if (c == tmp.child)  dp[i][i].trees[tmp.parent]++;
        }
    }

    /* 三重 dp 转移方程: dp(i,j).trees[parent] = dp(i,k).trees[c1] * dp(k,j).trees[c2] */
    /* 前提是 <parent>::=<c1><c2> */
    for (int len = 2; len <= slen; len++) {
        for (int left = 0; left <= slen - len; left++) {
            for (int right = left + 1; right < left + len; right++) {     
                unsigned *all =  &(dp[left][left + len - 1].trees[0]);
                unsigned *leftPart =  &(dp[left][right - 1].trees[0]);
                unsigned *rightPart =  &(dp[right][left + len - 1].trees[0]);
                for (int vn_index = 0; vn_index < vnProductions_cnt; vn_index++) {
                    VnProduction tmp = vnProductions[vn_index];
                    all[tmp.parent] += leftPart[tmp.child1] * rightPart[tmp.child2];
                }
            }
        }
    }
    /* 输出以 0 开始的语法树数目 */
    printf("%u\n", dp[0][slen - 1].trees[0]);
    return 0;
}