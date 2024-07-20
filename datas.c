#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define ALPHABET_SIZE 26

struct npt_node {
    char *name;
    struct npt_node **children;
};

struct hash_table {
    float *access_probabilities;
};

int char_to_index(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - 'a';
    } else if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    }
    return -1;
}

struct npt_node *npt_create() {
    struct npt_node *root = (struct npt_node *)malloc(sizeof(struct npt_node));
    if (root) {
        root->name = NULL;
        root->children = (struct npt_node **)malloc(ALPHABET_SIZE * sizeof(struct npt_node *));
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            root->children[i] = NULL;
        }
    }
    return root;
}

struct hash_table *create_hash_table() {
    struct hash_table *ht = (struct hash_table *)malloc(sizeof(struct hash_table));
    if (ht) {
        ht->access_probabilities = (float *)malloc(ALPHABET_SIZE * sizeof(float));
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            ht->access_probabilities[i] = 0.5;
        }
    }
    return ht;
}

void free_npt(struct npt_node *root) {
    if (root) {
        free(root->name);
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            free_npt(root->children[i]);
        }
        free(root->children);
        free(root);
    }
}

void free_hash_table(struct hash_table *ht) {
    if (ht) {
        free(ht->access_probabilities);
        free(ht);
    }
}

void npt_insert(struct npt_node *root, const char *name) {
    struct npt_node *current = root;
    for (int i = 0; i < strlen(name); i++) {
        char lowercase = tolower(name[i]);
        int index = char_to_index(lowercase);
        if (index == -1) {
            printf("Invalid character in name: %c\n", name[i]);
            return;
        }
        if (!current->children[index]) {
            current->children[index] = npt_create();
        }
        current = current->children[index];
    }
    current->name = (char *)malloc((strlen(name) + 1) * sizeof(char));
    strcpy(current->name, name);
}

int npt_lookup(struct npt_node *root, const char *name, struct hash_table *ht) {
    struct npt_node *current = root;
    for (int i = 0; i < strlen(name); i++) {
        char lowercase = tolower(name[i]);
        int index = char_to_index(lowercase);
        if (index == -1 || !current->children[index]) {
            return 0;
        }
        current = current->children[index];
        if (ht) {
            ht->access_probabilities[index] += 0.1;
        }
    }
    return current->name != NULL;
}

void npt_delete(struct npt_node *root, const char *name) {
    struct npt_node *current = root;
    struct npt_node *parent = NULL;
    int index;

    for (int i = 0; i < strlen(name); i++) {
        char lowercase = tolower(name[i]);
        index = char_to_index(lowercase);
        if (index == -1 || !current->children[index]) {
            printf("Node '%s' not found for deletion.\n", name);
            return;
        }
        parent = current;
        current = current->children[index];
    }

    int hasChildren = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (current->children[i] != NULL) {
            hasChildren = 1;
            break;
        }
    }

    if (hasChildren) {
        free(current->name);
        current->name = NULL;
    } else {
        free(current->name);
        free(current);
        parent->children[index] = NULL;
    }

    printf("Node '%s' deleted successfully.\n", name);
}

void print_npt_recursive(struct npt_node *node, int level) {
    if (node) {
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (node->children[i]) {
                printf("%*s%c", level * 2, "", i + 'a');
                if (node->children[i]->name) {
                    printf(" (%s)\n", node->children[i]->name);
                } else {
                    printf("\n");
                }
                print_npt_recursive(node->children[i], level + 1);
            }
        }
    }
}

void print_npt(struct npt_node *root) {
    printf("\nName Prefix Tree (NPT):\n");
    print_npt_recursive(root, 0);
}

void print_access_probabilities(struct hash_table *ht) {
    printf("\nAccess Probabilities:\n");
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        printf("%c: %.2f\n", i + 'a', ht->access_probabilities[i]);
    }
}

void print_average_access_probability(struct hash_table *ht) {
    float sum = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        sum += ht->access_probabilities[i];
    }
    float average = sum / ALPHABET_SIZE;
    printf("\nAverage Access Probability: %.2f\n", average);
}

void load_npt_from_file(struct npt_node *root, const char *input_filename) {
    FILE *input_file = fopen(input_filename, "r");
    if (input_file) {
        char buffer[100];
        while (fscanf(input_file, "%s", buffer) != EOF) {
            npt_insert(root, buffer);
        }
        fclose(input_file);
    } else {
        printf("Error opening file for NPT loading.\n");
    }
}

void save_npt_to_file(struct npt_node *root, const char *output_filename) {
    FILE *output_file = fopen(output_filename, "w");
    if (output_file) {
        print_npt_recursive(root, 0);
        fclose(output_file);
    } else {
        printf("Error opening file for NPT saving.\n");
    }
}

int main() {
    struct npt_node *root = npt_create();
    struct hash_table *ht = create_hash_table();

    char input_name[100];
    int choice;
    const char *output_filename = "npt_output.txt";

    load_npt_from_file(root, output_filename);

    while (1) {
        printf("\nMenu:\n");
        printf("1. Insert Name\n");
        printf("2. Lookup Name\n");
        printf("3. Delete Name\n");
        printf("4. Print NPT\n");
        printf("5. Print Access Probabilities\n");
        printf("6. Print Average Access Probability\n");
        printf("7. Save NPT to File\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter name to insert: ");
                scanf("%s", input_name);
                npt_insert(root, input_name);
                break;

            case 2:
                printf("Enter name to lookup: ");
                scanf("%s", input_name);
                printf("Lookup result for '%s': %s\n", input_name, npt_lookup(root, input_name, ht) ? "Found" : "Not Found");
                break;

            case 3:
                printf("Enter name to delete: ");
                scanf("%s", input_name);
                npt_delete(root, input_name);
                break;

            case 4:
                print_npt(root);
                break;

            case 5:
                print_access_probabilities(ht);
                break;

            case 6:
                print_average_access_probability(ht);
                break;

            case 7:
                save_npt_to_file(root, output_filename);
                break;

            case 8:
                free_npt(root);
                free_hash_table(ht);
                exit(0);

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}

