#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
 
static unsigned int JSHash(char* key, unsigned int key_len)
{
	unsigned int hash = 1315423911;
	unsigned int i = 0;

	for(i = 0; i < key_len; key++, i++)
	{
		hash ^= ((hash << 5) + (*key) + (hash >> 2));
	}
	return hash;
}
// // string harsh function
static unsigned int harsh_table_harsh_string(const char * sKey)
{
	const unsigned  char* p =  (const unsigned  char*) sKey;
	unsigned int value = *p;
	if(value)
	{
		for( p += 1; *p != '\0'; p++)
		{
			value = (value << 5) - value + *p;  
		}
	}
	return value;
}

// /** BKDR hash function  对字符串进行散列，得到一个整数的hash值*/
static uint bkdrHash(char *key)
{
    uint seed = 131;
    uint hash = 0;
    while (*key != '\n' && *key != 0)
    {
        hash = hash * seed + (*key++);
    }
    return (hash & 0x7FFFFFFF);
}

#define MAX_TABLE_SIZE 100
/*hash 节点*/
typedef struct HashNode{
    char *key;
    int value;
    struct HashNode *nextNode;
}HashNode;

/*hash 整个表，有一个数组存放了每个由节点组成的链表的位置*/
typedef struct HashTable{
    HashNode *hashNode[MAX_TABLE_SIZE];
    int currentIndex;
}HashTable;

/*初始化整个表*/
void InitHashTable(HashTable *hashTable)
{
    memset(hashTable->hashNode, 0, sizeof(HashNode *) * MAX_TABLE_SIZE);
    hashTable->currentIndex = 0;
}
/*一个规则：key转换成一个long型数据，用于确定存放在表中的那个位置*/
unsigned long HashFun(const char *key)
{
    unsigned long hash = 0;
    int len = strlen(key);
    for(int i = 0; i < len; i++)
    {   
        hash = hash * 33 + key[i];
    }
    return hash;
}
/*插入一个新的key：value数据*/
void Insert(HashTable *hashTable, char *key, int value)
{
    int pos = HashFun(key) % MAX_TABLE_SIZE;//通过计算的key转化的值与表的大小取余，来确定存储的位置
    /*下面是创建个新节点，把key和value都存在这个节点中*/
    HashNode *newNode = (HashNode *)malloc(sizeof(HashNode));
    newNode->nextNode = NULL;
    newNode->key = (char *)malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(newNode->key, key);
    newNode->key[strlen(key)] = 0;
    newNode->value = value;
	/*把新节点插入到表中去*/
    HashNode *p = hashTable->hashNode[pos];
    if(p == NULL)//如果这个位置未空就直接插入
    {
        hashTable->hashNode[pos] = newNode;
        hashTable->currentIndex++;
        return;
    }
	/*key的值已成存在，就覆盖写入value*/
    if(strcmp(p->key, key) == 0)
    {
        p->value = value;
        return;
    }
	/*通过下面，把节点查到第二个节点的位置，这里对节点的顺序没有要求*/
    HashNode *q = p->nextNode;
    newNode->nextNode = q;
    p->nextNode = newNode;
}
 /*通过给定的key，来找到表中的value。 没找到就返回NULL*/
int *Get(HashTable *hashTable, char *key)
{
    int pos = HashFun(key) % MAX_TABLE_SIZE;
    HashNode *p = hashTable->hashNode[pos];
    if (NULL == p)
    {
        return NULL;
    }
    else
    {
        HashNode *q = p;
        while(q != NULL)
        {
            if(0 == strcmp(q->key, key))
            {
                return &(q->value);
            }
            q = q->nextNode;

        }
        return NULL;
    }
}
 /*把表中名为key的节点删掉*/
int Dorp(HashTable *hashTable, char *key)
{   
    int pos = HashFun(key) % MAX_TABLE_SIZE;
    HashNode *p = hashTable->hashNode[pos];
    if (NULL == p)
    {   
        return 0;
    }
    else
    {   
        if(0 == strcmp(p->key, key))
        {   
            hashTable->hashNode[pos] = p->nextNode;
            free(p->key);
            free(p);
            return 1;
        }
        
        HashNode *q = p->nextNode;
        HashNode *last = p;
        while(NULL != q)
        {   
            
            if(0 == strcmp(q->key, key))
            {   
                last->nextNode = q->nextNode;
                free(q->key);
                free(q);
                return 1;
            }
            last = q;
            q = q->nextNode;
        }
        return 0;
    }
}
 /*打印表中的信息*/
void PrintHashTable(HashTable *hashTable)
{   
    for(int i = 0; i < MAX_TABLE_SIZE; i++)
    {   
        HashNode *head = hashTable->hashNode[i];
        if(head == NULL)
            continue;
        
        printf("\n数组下标：%d ==>", i);
        printf("(%s:%d)", head->key, head->value);
        head = head->nextNode;
        while(head)
        {
            printf("-->(%s:%d)", head->key, head->value);
            head = head->nextNode;
        }
    }
}
/*清空整个表*/
void clearHashTable(HashTable *hashTable)
{
    for (int i = 0; i < MAX_TABLE_SIZE; i++)
    {
        HashNode *head = hashTable->hashNode[i];
        while(head)
        {
            HashNode *temp = head->nextNode;
            free(head->key);
            free(head);
            head = temp;
        }
    }
}

int main_hash2()
{
    HashTable *hashTable = (HashTable *)malloc(sizeof(HashTable));
    InitHashTable(hashTable);
    Insert(hashTable, "1234", 1);
    Insert(hashTable, "11234", 1);
    Insert(hashTable, "hello", 2);
    PrintHashTable(hashTable);
    Dorp(hashTable, "hello");
    PrintHashTable(hashTable);
    clearHashTable(hashTable);
    free(hashTable);
}


///////////////////////////////////////////////////////////////////
