#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

#ifdef DEBUG
#include <assert.h>
#endif

#define MAX_EDGES 96  // number of characters with code from 32 to 127

struct node {
    struct node* edges[MAX_EDGES];
    int nil;
};

struct node* alloc_node() {
    struct node* node = (struct node*)calloc(1, sizeof(struct node));
    if (node == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    return node;
}

void freetrie(struct node* node) {
    size_t i;
    for (i = 0; i < MAX_EDGES; ++i) {
        if (node->edges[i] != NULL) {
            freetrie(node->edges[i]);
        }
    }
    free(node);
}

int main(int argc, char* argv[]) {
    if (argc != 2) exit(EXIT_FAILURE);

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    ssize_t i;
    struct node* root = alloc_node();
    struct node* cur_node = root;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1) {
        if (line[read - 1] == '\n') --read;
        for (i = 0; i < read; ++i) {
            int ind = line[i] - 32;
#ifdef DEBUG
            assert(ind >= 0 && ind < MAX_EDGES);
#endif
            if (cur_node->edges[ind] == NULL) {
                struct node* new_node =
                    (struct node*)malloc(sizeof(struct node));
                cur_node->edges[ind] = new_node;
            }
            cur_node = cur_node->edges[ind];
        }
        cur_node->nil = 1;
        cur_node = root;
    }

    cur_node = root;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') {
            line[read - 1] = '\0';
            --read;
        }
        if (!strcmp("exit", line)) break;

        for (i = 0; i < read; ++i) {
            if (line[i] < 32 || line[i] > 127 ||
                !cur_node->edges[line[i] - 32]) {
                break;
            }
            cur_node = cur_node->edges[line[i] - 32];
        }
        printf(i == read && cur_node->nil ? "YES\n" : "NO\n");
        cur_node = root;
    }
    freetrie(root);
    return 0;
}
