#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

#define FILE_NAME "students.txt"
#define DELIMITER "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
#define BUFFER_SIZE 256
#define DATE_SIZE 11

typedef enum {
    ENROLLMENT,
    DROPOUT,
    REINSTATEMENT
} EventType;

typedef struct {
    int mark;
    char* discipline;
} Mark;

typedef struct {
    EventType type;
    char date[DATE_SIZE];
} Event;

typedef struct {
    unsigned long credit_card_number;
    char* full_name;
    Mark* marks;
    int num_marks;
    Event* events;
    int num_events;
} Student;

typedef struct {
    char* discipline;
    float average;
    int count;
} DisciplineAverage;

typedef struct DisciplineNode {
    char* discipline;
    float average;
    int count;
    int height;
    struct DisciplineNode* left;
    struct DisciplineNode* right;
} DisciplineNode;

int max_my(int a, int b) {
    return (a > b) ? a : b;
}

DisciplineNode* create_discipline_node(char* discipline, int mark) {
    DisciplineNode* node = (DisciplineNode*)malloc(sizeof(DisciplineNode));
    if (node == NULL) {
        printf("Ошибка выделения памяти\n");
        exit(1);
    }
    node->discipline = (char*)malloc(strlen(discipline) + 1);
    if (node->discipline == NULL) {
        printf("Ошибка выделения памяти\n");
        free(node);
        exit(1);
    }
    strcpy(node->discipline, discipline);
    node->average = mark;
    node->count = 1;
    node->height = 0;
    node->left = node->right = NULL;
    return node;
}

int get_height_discipline(DisciplineNode* node) {
    return (node == NULL) ? -1 : node->height;
}

int get_balance_discipline(DisciplineNode* node) {
    if (node == NULL) return 0;
    return get_height_discipline(node->right) - get_height_discipline(node->left);
}

void left_rotate_discipline(DisciplineNode** node) {
    DisciplineNode* new_root = (*node)->right;
    (*node)->right = new_root->left;
    new_root->left = *node;
    update_height_discipline(*node);
    update_height_discipline(new_root);
    *node = new_root;
}

void right_rotate_discipline(DisciplineNode** node) {
    DisciplineNode* new_root = (*node)->left;
    (*node)->left = new_root->right;
    new_root->right = *node;
    update_height_discipline(*node);
    update_height_discipline(new_root);
    *node = new_root;
}

void update_height_discipline(DisciplineNode* node) {
    if (node != NULL) {
        int left_height = get_height_discipline(node->left);
        int right_height = get_height_discipline(node->right);
        node->height = max_my(left_height, right_height) + 1;
    }
}

void balance_discipline_tree(DisciplineNode** node) {
    if (*node != NULL) {
        update_height_discipline(*node);
        int balance = get_balance_discipline(*node);
        if (balance == -2) {
            if (get_balance_discipline((*node)->left) == 1) left_rotate_discipline(&((*node)->left));
            right_rotate_discipline(node);
        }
        else if (balance == 2) {
            if (get_balance_discipline((*node)->right) == -1) right_rotate_discipline(&((*node)->right));
            left_rotate_discipline(node);
        }
    }
}

DisciplineNode* insert_discipline(DisciplineNode* root, char* discipline, int mark) {
    if (root == NULL) {
        return create_discipline_node(discipline, mark);
    }

    if (strcmp(discipline, root->discipline) < 0) {
        root->left = insert_discipline(root->left, discipline, mark);
    }
    else if (strcmp(discipline, root->discipline) > 0) {
        root->right = insert_discipline(root->right, discipline, mark);
    }
    else {
        root->average = ((root->average * root->count) + mark) / (root->count + 1);
        root->count++;
    }

    balance_discipline_tree(&root);
    return root;
}

DisciplineNode* find_min_discipline(DisciplineNode* node) {
    while (node->left != NULL) node = node->left;
    return node;
}

void update_discipline_tree(DisciplineNode** discipline_tree, Student* student, int add) {
    for (int i = 0; i < student->num_marks; i++) {
        if (add) {
            *discipline_tree = insert_discipline_node(*discipline_tree, student->marks[i].discipline, student->marks[i].mark);
        }
        else {
            *discipline_tree = remove_discipline_node(*discipline_tree, student->marks[i].discipline);
        }
    }
}

void print_disciplines_inorder(DisciplineNode* root) {
    if (root != NULL) {
        print_disciplines_inorder(root->right);
        printf("Дисциплина: %s, Средний балл: %.2f\n", root->discipline, root->average);
        print_disciplines_inorder(root->left);
    }
}

typedef struct StudentNode {
    Student* student;
    struct StudentNode* next;
} StudentNode;

typedef struct TreeNode {
    char date[DATE_SIZE];
    StudentNode* students;
    int height;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

int get_height(TreeNode* node) {
    return (node == NULL) ? -1 : node->height;
}

void update_height(TreeNode* node) {
    if (node != NULL) {
        int left_height = get_height(node->left);
        int right_height = get_height(node->right);
        node->height = max_my(left_height, right_height) + 1;
    }
}

int check_input() {
    int input;
    while (1) {
        if (scanf("%d", &input) == 1) {
            while (getchar() != '\n');
            return input;
        }
        else {
            printf("Неверный ввод. \nПожалуйста, введите целое число:");
            while (getchar() != '\n');
        }
    }
}

int get_balance(TreeNode* node) {
    if (node == NULL) return 0;
    return get_height(node->right) - get_height(node->left);
}

void left_rotate(TreeNode** node) {
    TreeNode* new_root = (*node)->right;
    (*node)->right = new_root->left;
    new_root->left = *node;
    update_height(*node);
    update_height(new_root);
    *node = new_root;
}

void right_rotate(TreeNode** node) {
    TreeNode* new_root = (*node)->left;
    (*node)->left = new_root->right;
    new_root->right = *node;
    update_height(*node);
    update_height(new_root);
    *node = new_root;
}

void balance(TreeNode** node) {
    if (*node != NULL) {
        update_height(*node);
        int balance = get_balance(*node);
        if (balance == -2) {
            if (get_balance((*node)->left) == 1) left_rotate(&((*node)->left));
            right_rotate(node);
        }
        else if (balance == 2) {
            if (get_balance((*node)->right) == -1) right_rotate(&((*node)->right));
            left_rotate(node);
        }
    }
}

TreeNode* create_node(char* date, Student* student) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (node == NULL) {
        printf("Ошибка выделения памяти\n");
        exit(1);
    }
    strcpy(node->date, date);
    node->students = (StudentNode*)malloc(sizeof(StudentNode));
    if (node->students == NULL) {
        printf("Ошибка выделения памяти\n");
        free(node);
        exit(1);
    }
    node->students->student = student;
    node->students->next = NULL;
    node->left = node->right = NULL;
    node->height = 0;
    return node;
}

TreeNode* insert(TreeNode* root, char* date, Student* student) {
    if (root == NULL) return create_node(date, student);
    if (strcmp(date, root->date) < 0) root->left = insert(root->left, date, student);
    else if (strcmp(date, root->date) > 0) root->right = insert(root->right, date, student);
    else {
        StudentNode* new_student_node = (StudentNode*)malloc(sizeof(StudentNode));
        if (new_student_node == NULL) {
            printf("Ошибка выделения памяти\n");
            exit(1);
        }
        new_student_node->student = student;
        new_student_node->next = root->students;
        root->students = new_student_node;
    }
    balance(&root);
    return root;
}

TreeNode* find_min(TreeNode* node) {
    while (node->left != NULL) node = node->left;
    return node;
}

void search(TreeNode* root, char* date, StudentNode** students) {
    if (root != NULL) {
        if (strcmp(date, root->date) == 0) {
            *students = root->students;
        }
        else if (strcmp(date, root->date) < 0) {
            search(root->left, date, students);
        }
        else {
            search(root->right, date, students);
        }
    }
}

void free_tree(TreeNode* root) {
    if (root) {
        free_tree(root->left);
        free_tree(root->right);
        StudentNode* current = root->students;
        while (current) {
            StudentNode* temp = current;
            current = current->next;
            free(temp);
        }
        free(root);
    }
}

void input_marks(Student* student) {
    char buffer[BUFFER_SIZE];
    student->marks = (Mark*)malloc(student->num_marks * sizeof(Mark));
    if (student->marks == NULL) {
        printf("Ошибка выделения памяти\n");
        exit(1);
    }
    for (int i = 0; i < student->num_marks; i++) {
        printf("Введите название дисциплины (за оценку %д): ", i + 1);
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        student->marks[i].discipline = (char*)malloc(strlen(buffer) + 1);
        if (student->marks[i].discipline == NULL) {
            printf("Ошибка выделения памяти\n");
            for (int j = 0; j < i; ++j) {
                free(student->marks[j].discipline);
            }
            free(student->marks);
            exit(1);
        }
        strcpy(student->marks[i].discipline, buffer);
        printf("Оценка за %s: ", buffer);
        student->marks[i].mark = check_input();
    }
}

void input_events(Student* student) {
    char buffer[BUFFER_SIZE];
    student->events = (Event*)malloc(student->num_events * sizeof(Event));
    if (student->events == NULL) {
        printf("Ошибка выделения памяти\n");
        exit(1);
    }
    for (int i = 0; i < student->num_events; i++) {
        printf("Введите тип события (0 - зачисление, 1 - отчисление, 2 - восстановление): ");
        student->events[i].type = check_input();
        printf("Введите дату события (ГГГГ.ММ.ДД): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        strcpy(student->events[i].date, buffer);
    }
}

void free_student_memory(Student* student) {
    if (student->full_name) free(student->full_name);
    for (int i = 0; i < student->num_marks; ++i) {
        if (student->marks[i].discipline) free(student->marks[i].discipline);
    }
    if (student->marks) free(student->marks);
    if (student->events) free(student->events);
}

void free_memory(Student** students, int num_students, TreeNode** root) {
    for (int i = 0; i < num_students; i++) free_student_memory(&(*students)[i]);
    free(*students);
    free_tree(*root);
    *students = NULL;
}

Student input_student() {
    Student student = { 0 };
    char buffer[BUFFER_SIZE];
    printf("Номер зачётки: ");
    if (scanf("%lu", &student.credit_card_number) != 1) {
        printf("Ошибка ввода\n");
        exit(1);
    }
    while (getchar() != '\n');
    printf("ФИО: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    student.full_name = (char*)malloc(strlen(buffer) + 1);
    if (student.full_name == NULL) {
        printf("Ошибка выделения памяти\n");
        exit(1);
    }
    strcpy(student.full_name, buffer);
    printf("Количество оценок: ");
    if (scanf("%d", &student.num_marks) != 1) {
        printf("Ошибка ввода\n");
        exit(1);
    }
    while (getchar() != '\n');
    input_marks(&student);
    printf("Введите количество событий: ");
    if (scanf("%d", &student.num_events) != 1) {
        printf("Ошибка ввода\n");
        exit(1);
    }
    while (getchar() != '\n');
    input_events(&student);
    return student;
}

void save_students(Student* students, int num_students, const char* file_name) {
    FILE* file;
    if ((file = fopen(file_name, "w")) == NULL) {
        printf("Файл \"%s\" не может быть открыт\n", file_name);
        return;
    }
    fprintf(file, "%d\n", num_students);
    for (int i = 0; i < num_students; ++i) {
        fprintf(file, "%lu,%s\n", students[i].credit_card_number, students[i].full_name);
        fprintf(file, "%d\n", students[i].num_marks);
        for (int j = 0; j < students[i].num_marks; ++j) {
            fprintf(file, "%s,%d\n", students[i].marks[j].discipline, students[i].marks[j].mark);
        }
        fprintf(file, "%d\n", students[i].num_events);
        for (int k = 0; k < students[i].num_events; ++k) {
            fprintf(file, "%d,%s\n", students[i].events[k].type, students[i].events[k].date);
        }
    }
    fclose(file);
}

void add_student(Student** students, int* num_students, Student new_student) {
    Student* temp = (Student*)realloc(*students, (*num_students + 1) * sizeof(Student));
    if (!temp) {
        printf("Ошибка перераспределения памяти");
        free(*students);
        exit(1);
    }
    *students = temp;
    (*students)[*num_students] = new_student;
    (*num_students)++;
}

void print_disciplines(Student* students, int num_students, int i) {
    for (int j = 0; j < students[i].num_marks; ++j) {
        printf("\tДисциплина: %s, Оценка: %d\n", students[i].marks[j].discipline, students[i].marks[j].mark);
    }
}

void print_events(Student* students, int num_students, int i) {
    for (int k = 0; k < students[i].num_events; ++k) {
        char* event_type_str;
        switch (students[i].events[k].type) {
        case ENROLLMENT: event_type_str = "Зачисление"; break;
        case DROPOUT: event_type_str = "Отчисление"; break;
        case REINSTATEMENT: event_type_str = "Восстановление"; break;
        default: event_type_str = "Неизвестное событие"; break;
        }
        printf("\tТип события: %s, Дата: %s\n", event_type_str, students[i].events[k].date);
    }
}

void display_students(Student* students, int num_students) {
    for (int i = 0; i < num_students; ++i) {
        printf("Номер зачётки: %lu, ФИО: %s\n", students[i].credit_card_number, students[i].full_name);
        printf("Оценки:\n");
        print_disciplines(students, num_students, i);
        printf("События:\n");
        print_events(students, num_students, i);
        printf("\n");
    }
}

void remove_student_from_tree(TreeNode** root, Student* student) {
    if (*root == NULL) return;
    TreeNode* current = *root;
    TreeNode* parent = NULL;
    while (current != NULL && current->students->student != student) {
        parent = current;
        if (strcmp(student->events[find_index_last_event(*student)].date, current->date) < 0) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }
    if (current == NULL) return;
    StudentNode* student_node = current->students;
    StudentNode* prev_student_node = NULL;
    while (student_node != NULL && student_node->student != student) {
        prev_student_node = student_node;
        student_node = student_node->next;
    }
    if (student_node == NULL) return;
    if (prev_student_node == NULL) {
        current->students = student_node->next;
    }
    else {
        prev_student_node->next = student_node->next;
    }
    free(student_node);
    if (current->students == NULL) {
        if (current->left == NULL || current->right == NULL) {
            TreeNode* temp = (current->left != NULL) ? current->left : current->right;
            if (parent == NULL) {
                *root = temp;
            }
            else if (parent->left == current) {
                parent->left = temp;
            }
            else {
                parent->right = temp;
            }
            free(current);
        }
        else {
            TreeNode* temp = find_min(current->right);
            strcpy(current->date, temp->date);
            current->students = temp->students;
            remove_student_from_tree(&current->right, temp->students->student);
        }
    }
    balance(root);
}

void delete_student(Student** students, int* num_students, int index, TreeNode** root) {
    if (index < 0 || index >= *num_students) {
        printf("Неизвестный индекс\n");
        return;
    }
    Student* student = &(*students)[index];
    remove_student_from_tree(root, student);
    free_student_memory(student);
    for (int i = index; i < *num_students - 1; ++i) (*students)[i] = (*students)[i + 1];
    (*num_students)--;
    *students = (Student*)realloc(*students, (*num_students) * sizeof(Student));
    if (*students == NULL && *num_students > 0) {
        printf("Ошибка перераспределения памяти");
        exit(1);
    }
}

void calculate_sums_and_counts(DisciplineAverage** averages, int* num_averages, int* capacity, Student* students, int num_students) {
    for (int i = 0; i < num_students; i++) {
        for (int j = 0; j < students[i].num_marks; j++) {
            int found = 0;
            for (int k = 0; k < *num_averages; k++) {
                if (strcmp((*averages)[k].discipline, students[i].marks[j].discipline) == 0) {
                    (*averages)[k].average += students[i].marks[j].mark;
                    (*averages)[k].count++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (*num_averages >= *capacity) {
                    *capacity *= 2;
                    DisciplineAverage* temp = (DisciplineAverage*)realloc(*averages, (*capacity) * sizeof(DisciplineAverage));
                    if (!temp) {
                        printf("Ошибка перераспределения памяти");
                        free(*averages);
                        exit(1);
                    }
                    *averages = temp;
                }
                (*averages)[*num_averages].discipline = (char*)malloc(strlen(students[i].marks[j].discipline) + 1);
                if ((*averages)[*num_averages].discipline == NULL) {
                    printf("Ошибка выделения памяти\n");
                    free(*averages);
                    exit(1);
                }
                strcpy((*averages)[*num_averages].discipline, students[i].marks[j].discipline);
                (*averages)[*num_averages].average = (float)students[i].marks[j].mark;
                (*averages)[*num_averages].count = 1;
                (*num_averages)++;
            }
        }
    }
}

void bubble_sort_disciplines(DisciplineAverage* averages, int num_averages) {
    for (int i = 0; i < num_averages - 1; i++) {
        for (int j = 0; j < num_averages - i - 1; j++) {
            if (averages[j].average < averages[j + 1].average) {
                DisciplineAverage temp = averages[j];
                averages[j] = averages[j + 1];
                averages[j + 1] = temp;
            }
        }
    }
}

void calculate_averages(DisciplineAverage* averages, int num_averages) {
    for (int i = 0; i < num_averages; i++) {
        averages[i].average /= averages[i].count;
    }
}

void sort_disciplines_by_average(Student* students, int num_students) {
    int capacity = 10;
    DisciplineAverage* averages = (DisciplineAverage*)malloc(capacity * sizeof(DisciplineAverage));
    if (!averages) {
        printf("Ошибка выделения памяти");
        return;
    }
    int num_averages = 0;
    calculate_sums_and_counts(&averages, &num_averages, &capacity, students, num_students);
    calculate_averages(averages, num_averages);
    bubble_sort_disciplines(averages, num_averages);
    printf("Дисциплины, сортированные по среднему баллу:\n");
    for (int i = 0; i < num_averages; i++) {
        printf("%s: %.2f\n", averages[i].discipline, averages[i].average);
    }
    for (int i = 0; i < num_averages; i++) {
        free(averages[i].discipline);
    }
    free(averages);
}

void load_students_from_file(Student** students, int* num_students, const char* file_name) {
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        printf("Ошибка открытия файла\n");
        return;
    }
    fscanf(file, "%d\n", num_students);
    *students = (Student*)malloc(*num_students * sizeof(Student));
    for (int i = 0; i < *num_students; i++) {
        char buffer[256];
        fscanf(file, "%lu,%255[^\n]\n", &((*students)[i].credit_card_number), buffer);
        (*students)[i].full_name = (char*)malloc(strlen(buffer) + 1);
        strcpy((*students)[i].full_name, buffer);
        fscanf(file, "%d\n", &((*students)[i].num_marks));
        (*students)[i].marks = (Mark*)malloc((*students)[i].num_marks * sizeof(Mark));
        for (int j = 0; j < (*students)[i].num_marks; j++) {
            fscanf(file, "%255[^,],%d\n", buffer, &((*students)[i].marks[j].mark));
            (*students)[i].marks[j].discipline = (char*)malloc(strlen(buffer) + 1);
            strcpy((*students)[i].marks[j].discipline, buffer);
        }
        fscanf(file, "%d\n", &((*students)[i].num_events));
        (*students)[i].events = (Event*)malloc((*students)[i].num_events * sizeof(Event));
        for (int k = 0; k < (*students)[i].num_events; k++)     fscanf(file, "%d,%s\n", (int*)&((*students)[i].events[k].type), (*students)[i].events[k].date);
    }
    fclose(file);
}

int find_index_last_event(Student students) {
    int last_index = 0;
    char* temp = students.events[0].date;
    for (int j = 1; j < students.num_events; j++) {
        if (strcmp(temp, students.events[j].date) <= 0) {
            temp = students.events[j].date;
            last_index = j;
        }
    }
    return last_index;
}

void remove_students_with_low_marks(Student** students, int* num_students, TreeNode** root) {
    for (int i = 0; i < *num_students; i++) {
        int has_low_mark = 0;
        if ((*students)[i].num_events > 0) {
            if ((*students)[i].events[find_index_last_event((*students)[i])].type != DROPOUT) {
                for (int j = 0; j < (*students)[i].num_marks; ++j) {
                    if ((*students)[i].marks[j].mark < 4) {
                        delete_student(students, num_students, i, root);
                        i--;
                        break;
                    }
                }
            }
        }
    }
}

void print_tree(TreeNode* root, int space) {
    if (root == NULL)  return;
    space += 10;
    print_tree(root->right, space);
    printf("\n");
    for (int i = 10; i < space; i++)
        printf(" ");
    int count = 0;
    StudentNode* current = root->students;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    if (count > 1) printf("%s (%d)\n", root->date, count);
    else    printf("%s\n", root->date);
    print_tree(root->left, space);
}

void print_discipline(Student* student) {
    for (int j = 0; j < student->num_marks; ++j) {
        printf("\tДисциплина: %s, Оценка: %d\n", student->marks[j].discipline, student->marks[j].mark);
    }
}

void print_event(Student* student) {
    for (int k = 0; k < student->num_events; ++k) {
        char* event_type_str;
        switch (student->events[k].type) {
        case ENROLLMENT: event_type_str = "Зачисление"; break;
        case DROPOUT: event_type_str = "Отчисление"; break;
        case REINSTATEMENT: event_type_str = "Восстановление"; break;
        default: event_type_str = "Неизвестное событие"; break;
        }
        printf("\tТип события: %s, Дата: %s\n", event_type_str, student->events[k].date);
    }
}

void print_student(Student* student) {
    printf("Найден студент: %s\nНомер зачётки: %lu\n", student->full_name, student->credit_card_number);
    printf("Оценки:\n");
    print_discipline(student);
    printf("События:\n");
    print_event(student);
    printf("\n");
}

void add_case(Student** students, int* num_students, TreeNode** root, DisciplineNode** discipline_root) {
    Student new_student = input_student();
    add_student(students, num_students, new_student);
    if (new_student.num_events > 0) {
        *root = insert(*root, new_student.events[find_index_last_event(new_student)].date, &new_student);
    }
    for (int i = 0; i < new_student.num_marks; ++i) {
        *discipline_root = insert_discipline(*discipline_root, new_student.marks[i].discipline, new_student.marks[i].mark);
    }
}

void delete_case(Student** students, int* num_students, int type_task2, TreeNode** root, DisciplineNode** discipline_root) {
    printf("Индекс: ");
    if (scanf("%d", &type_task2) != 1) {
        printf("Ошибка ввода\n");
        return;
    }
    Student* student = &(*students)[type_task2];
    for (int i = 0; i < student->num_marks; ++i) {
        *discipline_root = insert_discipline(*discipline_root, student->marks[i].discipline, -student->marks[i].mark);
    }
    delete_student(students, num_students, type_task2, root);
}

void print_discipline_tree(DisciplineNode* root) {
    if (root == NULL) return;

    print_discipline_tree(root->right);
    printf("%s: %.2f\n", root->average / root->count);
    print_discipline_tree(root->left);
}

void case_print_tree(TreeNode* root) {
    int k = 0;
    print_tree(root, k);
    printf("\n");
}

void find_student(TreeNode* root) {
    char date[DATE_SIZE];
    printf("Введите дату (ГГГГ.ММ.ДД): ");
    if (scanf("%10s", date) != 1) {
        printf("Ошибка ввода\n");
        return;
    }
    StudentNode* found_students = NULL;
    search(root, date, &found_students);
    if (found_students) {
        printf("Студенты с датой %s:\n", date);
        while (found_students) {
            print_student(found_students->student);
            found_students = found_students->next;
        }
    }
    else {
        printf("Студенты с датой %s не найдены.\n", date);
    }
}

void free_tree_discipline(DisciplineNode* root) {
    if (root) {
        free_tree_discipline(root->left);
        free_tree_discipline(root->right);
        free(root->discipline);
        free(root);
    }
}

int main() {
    setlocale(LC_ALL, "Ru");
    Student* students = NULL;
    int num_students = 0, type_task2;
    TreeNode* root = NULL;
    DisciplineNode* discipline_root = NULL;
    load_students_from_file(&students, &num_students, FILE_NAME);
    for (int i = 0; i < num_students; ++i) {
        if (students[i].num_events > 0) {
            root = insert(root, students[i].events[find_index_last_event(students[i])].date, &students[i]);
        }
        for (int j = 0; j < students[i].num_marks; ++j) {
            discipline_root = insert_discipline(discipline_root, students[i].marks[j].discipline, students[i].marks[j].mark);
        }
    }
    while (1) {
        printf("0 - выйти\n1 - добавить студента\n2 - удалить студента\n3 - показать студентов\n4 - сортировать дисциплины\n5 - удалить студентов с плохими оценками\n6 - найти студента по дате\n7 - напечатать дерево\n8 - показать дисциплины\nВыберите действие: ");
        type_task2 = check_input();
        if (type_task2 == 0) break;
        switch (type_task2) {
        case 1: add_case(&students, &num_students, &root, &discipline_root);  break;
        case 2: delete_case(&students, &num_students, type_task2, &root, &discipline_root);  break;
        case 3: display_students(students, num_students); break;
        case 4: case_print_disciplines(discipline_root); break;
        case 5: remove_students_with_low_marks(&students, &num_students, &root); break;
        case 6: find_student(root); break;
        case 7: case_print_tree(root);  break;
        case 8: case_print_disciplines(discipline_root); break;
        }
        save_students(students, num_students, FILE_NAME);
    }
    free_memory(&students, num_students, &root);
    free_tree(discipline_root);
    return 0;
}
