#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define FILE_NAME "students.txt"
#define TABLE_SIZE 100
#define DATE_SIZE 11
#define BUFFER_SIZE 256

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

typedef struct MarkNode {
    char* key;
    Mark value;
    struct MarkNode* next;
} MarkNode;

typedef struct EventNode {
    char* key;
    Event value;
    struct EventNode* next;
} EventNode;

typedef struct Student {
    unsigned long credit_card_number;
    char* full_name;
    MarkNode* marks;
    EventNode* events;
    struct Student* next;
} Student;

Student* hash_table[TABLE_SIZE] = { NULL };

unsigned long long hashcode(unsigned long credit_card_number) {
    unsigned long long hash = 5381;
    unsigned char* str = (unsigned char*)&credit_card_number;
    for (size_t i = 0; i < sizeof(credit_card_number); ++i) {
        hash = ((hash << 5) + hash) ^ str[i];
    }
    return hash;
}

char* my_strdup(const char* str) {
    size_t len = strlen(str);
    char* copy = (char*)malloc(len + 1);
    if (copy) {
        strcpy(copy, str);
    }
    return copy;
}

Student* create_student(unsigned long credit_card_number, const char* full_name) {
    Student* student = (Student*)malloc(sizeof(Student));
    if (!student) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    student->credit_card_number = credit_card_number;
    student->full_name = my_strdup(full_name);
    student->marks = NULL;
    student->events = NULL;
    student->next = NULL;
    return student;
}

void add_mark(Student* student, const char* discipline, int mark) {
    MarkNode* new_node = (MarkNode*)malloc(sizeof(MarkNode));
    if (!new_node) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    new_node->key = my_strdup(discipline);
    new_node->value.mark = mark;
    new_node->value.discipline = new_node->key;
    new_node->next = student->marks;
    student->marks = new_node;
}

void add_event(Student* student, const char* date, EventType type) {
    EventNode* new_node = (EventNode*)malloc(sizeof(EventNode));
    if (!new_node) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    new_node->key = my_strdup(date);
    new_node->value.type = type;
    strncpy(new_node->value.date, date, DATE_SIZE - 1);
    new_node->value.date[DATE_SIZE - 1] = '\0';  // Ensure null termination
    new_node->next = NULL; // Initially, the new node does not point to any other node

    // If the student has no events yet, add the new node as the first event
    if (student->events == NULL) {
        student->events = new_node;
    }
    else {
        // Otherwise, add the new node to the end of the event list
        EventNode* current = student->events;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

void free_student_memory(Student* student) {
    free(student->full_name);

    MarkNode* mark = student->marks;
    while (mark) {
        MarkNode* temp = mark;
        mark = mark->next;
        free(temp->key);
        free(temp);
    }

    EventNode* event = student->events;
    while (event) {
        EventNode* temp = event;
        event = event->next;
        free(temp->key);
        free(temp);
    }
}

void free_hash_table() {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        Student* student = hash_table[i];
        while (student) {
            Student* temp = student;
            student = student->next;
            free_student_memory(temp);
            free(temp);
        }
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

void put_student(Student* student) {
    unsigned long long index = hashcode(student->credit_card_number) % TABLE_SIZE;
    student->next = hash_table[index];
    hash_table[index] = student;
}

Student* get_student(unsigned long credit_card_number) {
    unsigned long long index = hashcode(credit_card_number) % TABLE_SIZE;
    Student* student = hash_table[index];
    while (student) {
        if (student->credit_card_number == credit_card_number) {
            return student;
        }
        student = student->next;
    }
    return NULL;
}

void delete_student(unsigned long credit_card_number) {
    unsigned long long index = hashcode(credit_card_number) % TABLE_SIZE;
    Student* student = hash_table[index];
    Student* prev = NULL;
    while (student) {
        if (student->credit_card_number == credit_card_number) {
            if (prev) {
                prev->next = student->next;
            }
            else {
                hash_table[index] = student->next;
            }
            free_student_memory(student);
            free(student);
            return;
        }
        prev = student;
        student = student->next;
    }
}

void display_student(Student* student) {
    printf("Номер зачётки: %lu, ФИО: %s\n", student->credit_card_number, student->full_name);
    printf("Оценки:\n");
    MarkNode* mark = student->marks;
    while (mark) {
        printf("\tДисциплина: %s, Оценка: %d\n", mark->value.discipline, mark->value.mark);
        mark = mark->next;
    }
    printf("События:\n");
    EventNode* event = student->events;
    while (event) {
        char* event_type_str;
        switch (event->value.type) {
        case ENROLLMENT:
            event_type_str = "Зачисление";
            break;
        case DROPOUT:
            event_type_str = "Отчисление";
            break;
        case REINSTATEMENT:
            event_type_str = "Восстановление";
            break;
        default:
            event_type_str = "Неизвестное событие";
            break;
        }
        printf("\tТип события: %s, Дата: %s\n", event_type_str, event->value.date);
        event = event->next;
    }
    printf("\n");
}

void display_all_students() {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        Student* student = hash_table[i];
        while (student) {
            display_student(student);
            student = student->next;
        }
    }
}

void save_students(const char* file_name) {
    FILE* file = fopen(file_name, "w");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < TABLE_SIZE; ++i) {
        Student* student = hash_table[i];
        while (student) {
            fprintf(file, "%lu,%s\n", student->credit_card_number, student->full_name);
            MarkNode* mark = student->marks;
            while (mark) {
                fprintf(file, "%s,%d\n", mark->value.discipline, mark->value.mark);
                mark = mark->next;
            }
            EventNode* event = student->events;
            while (event) {
                fprintf(file, "%d,%s\n", event->value.type, event->value.date);
                event = event->next;
            }
            student = student->next;
        }
    }
    fclose(file);
}

void load_students_from_file(const char* file_name) {
    FILE* file = fopen(file_name, "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    unsigned long credit_card_number;
    char full_name[BUFFER_SIZE];
    char discipline[BUFFER_SIZE];
    int mark, event_type;
    char date[DATE_SIZE];

    while (fscanf(file, "%lu,%255[^\n]\n", &credit_card_number, full_name) == 2) {
        Student* student = create_student(credit_card_number, full_name);

        while (fscanf(file, "%255[^,],%d\n", discipline, &mark) == 2) {
            add_mark(student, discipline, mark);
        }

        while (fscanf(file, "%d,%10s\n", &event_type, date) == 2) {
            add_event(student, date, event_type);
        }

        put_student(student);
    }

    fclose(file);
}

void remove_students_with_low_marks() {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        Student* student = hash_table[i];
        Student* prev = NULL;
        while (student) {
            int has_low_marks = 0;
            MarkNode* mark = student->marks;
            while (mark) {
                if (mark->value.mark < 4) {
                    has_low_marks = 1;
                    break;
                }
                mark = mark->next;
            }
            if (has_low_marks) {
                if (prev) {
                    prev->next = student->next;
                }
                else {
                    hash_table[i] = student->next;
                }
                Student* temp = student;
                student = student->next;
                free_student_memory(temp);
                free(temp);
            }
            else {
                prev = student;
                student = student->next;
            }
        }
    }
}

void input_marks(Student* student) {
    char buffer[BUFFER_SIZE];
    printf("Количество оценок: ");
    int num_marks;
    if (scanf("%d", &num_marks) != 1) {
        fprintf(stderr, "Ошибка ввода числа оценок\n");
        return;
    }
    while (getchar() != '\n');
    for (int i = 0; i < num_marks; i++) {
        printf("Введите название дисциплины (за оценку %d): ", i + 1);
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            fprintf(stderr, "Ошибка ввода названия дисциплины\n");
            return;
        }
        buffer[strcspn(buffer, "\n")] = 0;
        printf("Оценка за %s: ", buffer);
        int mark;
        if (scanf("%d", &mark) != 1) {
            fprintf(stderr, "Ошибка ввода оценки\n");
            return;
        }
        while (getchar() != '\n');
        add_mark(student, buffer, mark);
    }
}

void input_events(Student* student) {
    char buffer[BUFFER_SIZE];
    printf("Количество событий: ");
    int num_events;
    if (scanf("%d", &num_events) != 1) {
        fprintf(stderr, "Ошибка ввода числа событий\n");
        return;
    }
    while (getchar() != '\n');
    for (int i = 0; i < num_events; i++) {
        printf("Введите тип события (0 - зачисление, 1 - отчисление, 2 - восстановление): ");
        int type;
        if (scanf("%d", &type) != 1) {
            fprintf(stderr, "Ошибка ввода типа события\n");
            return;
        }
        while (getchar() != '\n');
        printf("Введите дату события (YYYY.MM.DD): ");
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            fprintf(stderr, "Ошибка ввода даты события\n");
            return;
        }
        buffer[strcspn(buffer, "\n")] = 0;
        add_event(student, buffer, type);
    }
}

Student* input_student() {
    Student* student = NULL;
    char buffer[BUFFER_SIZE];
    unsigned long credit_card_number;
    printf("Номер зачётки: ");
    if (scanf("%lu", &credit_card_number) != 1) {
        fprintf(stderr, "Ошибка ввода номера зачётки\n");
        return NULL;
    }
    while (getchar() != '\n');
    printf("ФИО: ");
    if (!fgets(buffer, sizeof(buffer), stdin)) {
        fprintf(stderr, "Ошибка ввода ФИО\n");
        return NULL;
    }
    buffer[strcspn(buffer, "\n")] = 0;
    student = create_student(credit_card_number, buffer);
    input_marks(student);
    input_events(student);
    put_student(student);
    return student;
}

void add_student(Student* new_student) {
    if (new_student) {
        put_student(new_student);
    }
}

void sort_disciplines_by_average(Student* students, int num_students) {
    // Реализация сортировки дисциплин по среднему баллу
}

void case_add_student() {
    Student* new_student = input_student();
    if (new_student) {
        add_student(new_student);
    }
}

int main() {
    setlocale(LC_ALL, "Ru");
    load_students_from_file(FILE_NAME);
    while (1) {
        printf("0 - выйти\n1 - добавить студента\n2 - удалить студента\n3 - показать студентов\n4 - сортировать дисциплины\n5 - удалить студентов с плохими оценками\nВыберите действие: ");
        int type_task2 = check_input();
        if (type_task2 == 0) break;
        switch (type_task2) {
        case 1: {
            case_add_student(); break;
        }
        case 2: {
            printf("Номер зачётки: ");
            unsigned long credit_card_number;
            if (scanf("%lu", &credit_card_number) != 1) {
                fprintf(stderr, "Ошибка ввода номера зачётки\n");
                break;
            }
            delete_student(credit_card_number);
            break;
        }
        case 3: display_all_students(); break;
        case 4: /*sort_disciplines_by_average(students, num_students);*/ break; // Реализуйте эту функцию при необходимости
        case 5: remove_students_with_low_marks(); break;
        default: fprintf(stderr, "Неверный ввод\n");
        }
        save_students(FILE_NAME);
    }
    free_hash_table();
    return 0;
}
