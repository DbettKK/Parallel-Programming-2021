#include<iostream>
#include<cstdio>
#include<unordered_map>
#include<string>
#include<cstdlib>
#include<cstring>

using namespace std;

#define MAX_PRODUCTION2_NUM 512
#define MAX_PRODUCTION1_NUM 128
#define MAX_VN_NUM 128
#define MAX_VT_NUM 128
#define MAX_STRING_LENGTH 1024

typedef struct VnProduction {
    int parent, child1, child2;
} VnProduction;

typedef struct VtProduction {
    int parent;
    char child;
} VtProduction;

typedef struct SubTree {
    unsigned vn_num[MAX_VN_NUM];
} SubTree;

SubTree dp[MAX_STRING_LENGTH][MAX_STRING_LENGTH];
VtProduction vtProductions[MAX_PRODUCTION1_NUM];
VnProduction vnProductions[MAX_PRODUCTION2_NUM];

int slen;
char solve[MAX_STRING_LENGTH];

int vnNum;
int vnProductions_cnt, vtProductions_cnt;

unordered_map<string, SubTree*> memdp_map;

int main() {
    freopen("input.txt", "r", stdin);
    scanf("%d\n", &vnNum);
    scanf("%d\n", &vnProductions_cnt);
    for (int i = 0; i < vnProductions_cnt; i++)
        scanf("<%d>::=<%d><%d>\n", &vnProductions[i].parent, &vnProductions[i].child1, &vnProductions[i].child2);
    scanf("%d\n", &vtProductions_cnt);
    for (int i = 0; i < vtProductions_cnt; i++)
        scanf("<%d>::=%c\n", &vtProductions[i].parent, &vtProductions[i].child);
    scanf("%d\n", &slen);
    scanf("%s\n", solve);

    for (int i = 0; i < slen; i++) {
        char c = solve[i];
        for (int j = 0; j < vtProductions_cnt; j++) {
            VtProduction tmp = vtProductions[j];
            if (c == tmp.child)  dp[i][i].vn_num[tmp.parent]++;
        }
    }

    for (int len = 2; len <= slen; len++) {
        for (int left = 0; left <= slen - len; left++) {
            string tmp(solve + left, solve + left + len);
            if (memdp_map.find(tmp) != memdp_map.end()) {
                dp[left][left + len - 1] = *(memdp_map.find(tmp)->second);
                continue;
            } else {
                for (int right = left + 1; right < left + len; right++) {
                    for (int i = 0; i < vnProductions_cnt; i++) {
                        VnProduction tmp = vnProductions[i];
                        dp[left][left + len - 1].vn_num[tmp.parent] += 
                            dp[left][right - 1].vn_num[tmp.child1] * dp[right][left + len - 1].vn_num[tmp.child2];
                    }
                }
                memdp_map.insert(pair<string, SubTree*>(tmp, &dp[left][left + len - 1]));
            }
        }
    }
    printf("%u\n", dp[0][slen - 1].vn_num[0]);
    return 0;
}