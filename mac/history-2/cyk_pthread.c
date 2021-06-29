#include <stdio.h>
#include <time.h>
#include <pthread.h>

#define MAX_PRODUCTION2_NUM 512
#define MAX_PRODUCTION1_NUM 128
#define MAX_VN_NUM 10
#define MAX_VT_NUM 128
#define MAX_STRING_LENGTH 1024

#define NUM_OF_THREADS 31

char str[MAX_STRING_LENGTH];

struct SubTree
{
    int cnt;
    int vn_index[MAX_VN_NUM];
    int vn[MAX_VN_NUM][2]; //表示VN和子树数目
} subtree_table[MAX_STRING_LENGTH][MAX_STRING_LENGTH];

struct VNTable
{
    int cnt;
    int vn[MAX_VN_NUM];
} production2_table[MAX_VN_NUM][MAX_VN_NUM], production1_table[MAX_VT_NUM];

int vn_num;
int production2_num;
int production1_num;
int string_length;
//多线程变量
int len;
pthread_mutex_t mutex[NUM_OF_THREADS];

void *cal(void *tid)
{

    int my_rank = (long long)tid;
    int max_left = string_length - len;
    for (int left = my_rank; left <= max_left; left += NUM_OF_THREADS)
    {
        int max_right = left + len;
        for (int right = left + 1; right < max_right; right++)
        {
            int child1_cnt = subtree_table[left][right - 1].cnt;
            int child2_cnt = subtree_table[right][left + len - 1].cnt;
            for (int i = 0; i < child1_cnt; i++)
            {
                int child1 = subtree_table[left][right - 1].vn[i][0];
                for (int j = 0; j < child2_cnt; j++)
                {
                    int child2 = subtree_table[right][left + len - 1].vn[j][0];
                    int vn_cnt = production2_table[child1][child2].cnt;
                    for (int k = 0; k < vn_cnt; k++)
                    {
                        int parent = production2_table[child1][child2].vn[k];
                        int vn_index = subtree_table[left][left + len - 1].vn_index[parent] - 1;
                        if (vn_index != -1)
                        {
                            subtree_table[left][left + len - 1].vn[vn_index][1] += subtree_table[left][right - 1].vn[i][1] * subtree_table[right][left + len - 1].vn[j][1];
                        }
                        else
                        {
                            int cnt = subtree_table[left][left + len - 1].cnt++;
                            subtree_table[left][left + len - 1].vn_index[parent] = cnt + 1;
                            subtree_table[left][left + len - 1].vn[cnt][0] = parent;
                            subtree_table[left][left + len - 1].vn[cnt][1] = subtree_table[left][right - 1].vn[i][1] * subtree_table[right][left + len - 1].vn[j][1];
                        }
                    }
                }
            }
        }
    }
    return NULL;
}

int main()
{
    clock_t start = clock();
    freopen("input.txt", "r", stdin);
    scanf("%d\n", &vn_num);
    scanf("%d\n", &production2_num);
    for (int i = 0; i < production2_num; i++)
    {
        int parent, child1, child2;
        scanf("<%d>::=<%d><%d>\n", &parent, &child1, &child2);
        int cnt = production2_table[child1][child2].cnt++;
        production2_table[child1][child2].vn[cnt] = parent;
    }
    scanf("%d\n", &production1_num);
    for (int i = 0; i < production1_num; i++)
    {
        int parent;
        char c;
        scanf("<%d>::=%c\n", &parent, &c);
        int child = c;
        int cnt = production1_table[child].cnt++;
        production1_table[child].vn[cnt] = parent;
    }
    scanf("%d\n", &string_length);
    scanf("%s\n", str);

    for (int i = 0; i < string_length; i++)
    {
        int t = str[i];
        for (int j = 0; j < production1_table[t].cnt; j++)
        {
            int cnt = subtree_table[i][i].cnt++;
            subtree_table[i][i].vn[cnt][0] = production1_table[t].vn[j];
            subtree_table[i][i].vn[cnt][1]++;
            subtree_table[i][i].vn_index[production1_table[t].vn[j]] = cnt + 1;
        }
    }
    pthread_t threads[NUM_OF_THREADS];
    for (size_t i = 0; i < NUM_OF_THREADS; i++)
    {
        pthread_mutex_init(&mutex[i], NULL);
    }
    for (len = 2; len <= string_length; len++)
    {
        for (size_t i = 0; i < NUM_OF_THREADS; i++)
        {
            pthread_create(&threads[i], NULL, cal, (void *)i);
        }
        for (size_t i = 0; i < NUM_OF_THREADS; i++)
        {
            pthread_join(threads[i], NULL);
        }
    }
    unsigned treeNum = 0;
    int vn_index = subtree_table[0][string_length - 1].vn_index[0] - 1;
    if (vn_index != -1)
    {
        treeNum = subtree_table[0][string_length - 1].vn[vn_index][1];
    }
    printf("%u\n", treeNum);
    printf("time:%lldms\n", clock() - start);
    return 0;
}
