#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define FILE_NAME "students.txt"
#define DATE_SIZE 11
#define BUFFER_SIZE 256
#define HASH_NUMB 5381
#define INITIAL_SIZE 100
#define LOAD_FACTOR 0.72f
#define MULTIPLIER 2.0f

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

typedef struct Student {
    unsigned long credit_card_number;
    char* full_name;
    Mark* marks;
    int num_marks;
    Event* events;
    int num_events;
    struct Student* next;
} Student;

typedef struct {
    char* discipline;
    float average;
    int count;
} DisciplineAverage;

typedef char* K;
typedef Student* V;

typedef struct Entry {
    K key;
    V value;
} Entry;

typedef struct Node {
    Entry* value;
    struct Node* next;
} Node;

typedef struct Hashmap {
    Node** data;
    size_t size;
    size_t arr_size;
    size_t limit;
    float loadFactor;
    float multiplier;
} Hashmap;

unsigned int hash_func(const char* str) {
    unsigned long hash = HASH_NUMB;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % INITIAL_SIZE;
}

void freeEntry(Entry** e) {
    free(*e);
}

Hashmap* createHashmap(size_t limit, float loadFactor, float multiplier) {
    Hashmap* tmp = (Hashmap*)malloc(sizeof(Hashmap));
    tmp->arr_size = (limit >= INITIAL_SIZE) ? limit : INITIAL_SIZE;
    tmp->loadFactor = (loadFactor >= LOAD_FACTOR && loadFactor <= 1.0f) ? loadFactor : LOAD_FACTOR;
    tmp->limit = (size_t)(tmp->loadFactor * tmp->arr_size);
    tmp->size = 0;
    tmp->multiplier = (multiplier >= MULTIPLIER) ? multiplier : MULTIPLIER;
    tmp->data = (Node**)calloc(tmp->arr_size, sizeof(Node*));
    return tmp;
}

Hashmap* rehashUp(Hashmap** _map_, Entry* e);

void raw_put(Hashmap** _map_, Entry* e) {
    Hashmap* map = *_map_;
    unsigned int hash = hash_func(e->key);
    size_t index = (hash % map->arr_size);

    if (map->size < map->limit) {
        if (map->data[index] == NULL) {
            Node* newNode = (Node*)malloc(sizeof(Node));
            newNode->next = NULL;
            newNode->value = e;
            map->data[index] = newNode;
        }
        else {
            Node* anchor = map->data[index];
            Node* newNode = NULL;
            while (anchor->next) {
                if (strcmp(anchor->value->key, e->key) == 0) {
                    exit(-5);
                }
                anchor = anchor->next;
            }
            newNode = (Node*)malloc(sizeof(Node));
            newNode->next = NULL;
            newNode->value = e;
            anchor->next = newNode;
        }
    }
    else {
        *_map_ = rehashUp(_map_, e);
    }
    (*_map_)->size++;
}

void put(Hashmap** _map_, K key, V value) {
    Entry* e = (Entry*)malloc(sizeof(Entry));
    e->key = key;
    e->value = value;
    raw_put(_map_, e);
}

Hashmap* rehashUp(Hashmap** _map_, Entry* e) {
    Hashmap* newMap = createHashmap((size_t)((*_map_)->arr_size * (*_map_)->multiplier), (*_map_)->loadFactor, (*_map_)->multiplier);
    size_t i, size;
    Hashmap* map = (*_map_);
    Node* anchor = NULL;
    Node* target = NULL;

    size = (*_map_)->arr_size;
    for (i = 0; i < size; i++) {
        anchor = map->data[i];
        while (anchor) {
            target = anchor;
            anchor = anchor->next;
            raw_put(&newMap, target->value);
            free(target);
        }
        free(anchor);
    }
    free(map->data);
    free(*_map_);
    *_map_ = newMap;
    raw_put(&newMap, e);
    return newMap;
}

V get(Hashmap* map, K key) {
    unsigned int hash = hash_func(key);
    size_t index = (hash % map->arr_size);
    V retVal = NULL;
    if (map->data[index] != NULL) {
        if (strcmp(map->data[index]->value->key, key) == 0) {
            return map->data[index]->value->value;
        }
        else {
            Node* anchor = map->data[index]->next;
            while (anchor) {
                if (strcmp(anchor->value->key, key) == 0) {
                    retVal = anchor->value->value;
                    break;
                }
                anchor = anchor->next;
            }
        }
    }
    return retVal;
}

Entry* xremove(Hashmap* map, K key) {
    unsigned int hash = hash_func(key);
    size_t index = (hash % map->arr_size);
    Node* retVal = NULL;
    Entry* content = NULL;

    if (map->data[index] != NULL) {
        if (strcmp(map->data[index]->value->key, key) == 0) {
            retVal = map->data[index];
            map->data[index] = map->data[index]->next;
        }
        else {
            Node* back = map->data[index];
            Node* anchor = back->next;
            while (anchor) {
                if (strcmp(anchor->value->key, key) == 0) {
                    retVal = anchor;
                    back->next = anchor->next;
                }
                back = anchor;
                anchor = anchor->next;
            }
        }
    }

    if (retVal != NULL) {
        content = retVal->value;
    }
    free(retVal);
    map->size--;
    return content;
}

void mapIterate(Hashmap* map, void(*f)(Entry*, void*), void* data) {
    size_t size, i;
    size = map->arr_size;
    for (i = 0; i < size; i++) {
        Node* anchor = map->data[i];
        while (anchor) {
            f(anchor->value, data);
            anchor = anchor->next;
        }
    }
}


void destroyHashmap(Hashmap** _map_) {
    Hashmap* map = *_map_;
    size_t i, size;
    Node* anchor = NULL;
    Node* target = NULL;

    size = map->arr_size;
    for (i = 0; i < size; i++) {
        anchor = map->data[i];
        while (anchor) {
            target = anchor;
            anchor = anchor->next;
            freeEntry(&(target->value));
            free(target);
        }
        free(anchor);
    }
    free(map->data);
    free(*_map_);
    *_map_ = NULL;
}

void free_memory(Hashmap** _map_) {
    destroyHashmap(_map_);
}

void input_marks(Student* student) {
    char buffer[BUFFER_SIZE];
    student->marks = (Mark*)malloc(student->num_marks * sizeof(Mark));
    for (int i = 0; i < student->num_marks; i++) {
        printf("Введите название дисциплины (за оценку %d): ", i + 1);
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        student->marks[i].discipline = (char*)malloc(strlen(buffer) + 1);
        strcpy(student->marks[i].discipline, buffer);
        printf("Оценка за %s: ", buffer);
        scanf("%d", &student->marks[i].mark);
        while (getchar() != '\n');
    }
}

void input_events(Student* student) {
    char buffer[BUFFER_SIZE];
    student->events = (Event*)malloc(student->num_events * sizeof(Event));
    for (int i = 0; i < student->num_events; i++) {
        printf("Введите тип события (0 - зачисление, 1 - отчисление, 2 - восстановление): ");
        scanf("%d", (int*)&student->events[i].type);
        while (getchar() != '\n');
        printf("Введите дату события (ГГГГ.ММ.ДД): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        strcpy(student->events[i].date, buffer);
    }
}

Student* input_student() {
    Student* student = (Student*)malloc(sizeof(Student));
    char buffer[BUFFER_SIZE];
    printf("Номер зачётки: ");
    scanf("%lu", &student->credit_card_number);
    while (getchar() != '\n');
    printf("ФИО: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    student->full_name = (char*)malloc(strlen(buffer) + 1);
    strcpy(student->full_name, buffer);
    printf("Количество оценок: ");
    scanf("%d", &student->num_marks);
    while (getchar() != '\n');
    input_marks(student);
    printf("Введите количество событий: ");
    scanf("%d", &student->num_events);
    while (getchar() != '\n');
    input_events(student);
    return student;
}

void display_student(Entry* entry, void* data) {
    Student* student = entry->value;
    printf("Номер зачётки: %lu, ФИО: %s\n", student->credit_card_number, student->full_name);
    printf("Оценки:\n");
    for (int j = 0; j < student->num_marks; ++j)
        printf("\tДисциплина: %s, Оценка: %d\n", student->marks[j].discipline, student->marks[j].mark);
    printf("События:\n");
    for (int k = 0; k < student->num_events; ++k) {
        char* event_type_str;
        switch (student->events[k].type) {
        case ENROLLMENT:
            event_type_str = "Зачисление";  break;
        case DROPOUT:
            event_type_str = "Отчисление";  break;
        case REINSTATEMENT:
            event_type_str = "Восстановление";  break;
        default:
            event_type_str = "Неизвестное событие"; break;
        }
        printf("\tТип события: %s, Дата: %s\n", event_type_str, student->events[k].date);
    }
    printf("\n");
}

void calculate_sums_and_counts(DisciplineAverage** averages, int* num_averages, int* capacity, Student* student) {
    for (int j = 0; j < student->num_marks; j++) {
        int found = 0;
        for (int k = 0; k < *num_averages; k++) {
            if (strcmp((*averages)[k].discipline, student->marks[j].discipline) == 0) {
                (*averages)[k].average += student->marks[j].mark;
                (*averages)[k].count++;
                found = 1;
                break;
            }
        }
        if (!found) {
            if (*num_averages >= *capacity) {
                (*capacity)++;
                DisciplineAverage* temp = (DisciplineAverage*)realloc(*averages, (*capacity) * sizeof(DisciplineAverage));
                if (!temp) {
                    printf("Ошибка перераспределения памяти");
                    free(*averages);
                    exit(1);
                }
                *averages = temp;
            }
            (*averages)[*num_averages].discipline = student->marks[j].discipline;
            (*averages)[*num_averages].average = student->marks[j].mark;
            (*averages)[*num_averages].count = 1;
            (*num_averages)++;
        }
    }
}

void calculate_averages(DisciplineAverage* averages, int num_averages) {
    for (int i = 0; i < num_averages; i++) {
        averages[i].average /= averages[i].count;
    }
}

void display_sorted_disciplines(DisciplineAverage* averages, int num_averages) {
    printf("Дисциплины, сортированные по среднему баллу:\n");
    for (int i = 0; i < num_averages; i++) {
        printf("%s: %.2f\n", averages[i].discipline, averages[i].average);
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

void calculate_sums_and_counts_callback(Entry* entry, void* data) {
    struct { DisciplineAverage** averages; int* num_averages; int* capacity; } *context = data;
    calculate_sums_and_counts(context->averages, context->num_averages, context->capacity, entry->value);
}

void sort_disciplines_by_average(Hashmap* students_map) {
    int capacity = 10;
    DisciplineAverage* averages = (DisciplineAverage*)malloc(capacity * sizeof(DisciplineAverage));
    if (!averages) {
        printf("Ошибка выделения памяти");
        return;
    }
    int num_averages = 0;

    struct { DisciplineAverage** averages; int* num_averages; int* capacity; } context = { &averages, &num_averages, &capacity };
    mapIterate(students_map, calculate_sums_and_counts_callback, &context);

    calculate_averages(averages, num_averages);
    bubble_sort_disciplines(averages, num_averages);
    display_sorted_disciplines(averages, num_averages);
    free(averages);
}

void load_students_from_file(Hashmap** students_map, const char* file_name) {
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        printf("Ошибка открытия файла\n");
        return;
    }
    int num_students;
    fscanf(file, "%d\n", &num_students);

    for (int i = 0; i < num_students; i++) {
        Student* student = (Student*)malloc(sizeof(Student));
        char buffer[256];
        fscanf(file, "%lu,%255[^\n]\n", &(student->credit_card_number), buffer);
        student->full_name = (char*)malloc(strlen(buffer) + 1);
        strcpy(student->full_name, buffer);
        fscanf(file, "%d\n", &(student->num_marks));
        student->marks = (Mark*)malloc(student->num_marks * sizeof(Mark));
        for (int j = 0; j < student->num_marks; j++) {
            fscanf(file, "%255[^,],%d\n", buffer, &(student->marks[j].mark));
            student->marks[j].discipline = (char*)malloc(strlen(buffer) + 1);
            strcpy(student->marks[j].discipline, buffer);
        }
        fscanf(file, "%d\n", &(student->num_events));
        student->events = (Event*)malloc(student->num_events * sizeof(Event));
        for (int k = 0; k < student->num_events; k++) {
            fscanf(file, "%d,%s\n", (int*)&(student->events[k].type), student->events[k].date);
        }
        put(students_map, student->full_name, student);
    }
    fclose(file);
}

void remove_students_with_low_marks_callback(Entry* entry, void* data) {
    Hashmap* students_map = data;
    Student* student = entry->value;
    int has_low_mark = 0, last_index = 0;
    if (student->num_events > 0) {
        char* temp = student->events[0].date;
        for (int j = 1; j < student->num_events; j++) {
            if (strcmp(temp, student->events[j].date) <= 0) {
                temp = student->events[j].date;
                last_index = j;
            }
        }
        if (student->events[last_index].type != DROPOUT) {
            for (int j = 0; j < student->num_marks; ++j) {
                if (student->marks[j].mark < 4) {
                    xremove(students_map, student->full_name);
                    break;
                }
            }
        }
    }
}

void remove_students_with_low_marks(Hashmap* students_map) {
    mapIterate(students_map, remove_students_with_low_marks_callback, students_map);
}

void save_students_callback(Entry* entry, void* data) {
    Student* student = entry->value;
    FILE* file = data;
    fprintf(file, "%lu,%s\n", student->credit_card_number, student->full_name);
    fprintf(file, "%d\n", student->num_marks);
    for (int j = 0; j < student->num_marks; ++j) {
        fprintf(file, "%s,%d\n", student->marks[j].discipline, student->marks[j].mark);
    }
    fprintf(file, "%d\n", student->num_events);
    for (int k = 0; k < student->num_events; ++k) {
        fprintf(file, "%d,%s\n", student->events[k].type, student->events[k].date);
    }
}

void save_students(Hashmap* students_map, const char* file_name) {
    FILE* file;
    if ((file = fopen(file_name, "w")) == NULL) {
        printf("Файл \"%s\" не может быть открыт\n", file_name);
        return;
    }
    fprintf(file, "%zu\n", students_map->size);
    mapIterate(students_map, save_students_callback, file);
    fclose(file);
}

void print_student(Student* student) {
    printf("Найден студент: %s\nНомер зачётки: %lu\n", student->full_name, student->credit_card_number);
    printf("Оценки:\n");
    for (int j = 0; j < student->num_marks; ++j) {
        printf("\tДисциплина: %s, Оценка: %d\n", student->marks[j].discipline, student->marks[j].mark);
    }
    printf("События:\n");
    for (int k = 0; k < student->num_events; ++k) {
        char* event_type_str;
        switch (student->events[k].type) {
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
        printf("\tТип события: %s, Дата: %s\n", event_type_str, student->events[k].date);
    }
    printf("\n");
}

void case_add(Hashmap** students_map) {
    Student* new_student = input_student();
    put(students_map, new_student->full_name, new_student);
}

void case_delete(Hashmap* students_map) {
    char buffer[BUFFER_SIZE];
    printf("ФИО: ");
    while (getchar() != '\n');
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    xremove(students_map, buffer);
}

void case_find_students(Hashmap* students_map) {
    char buffer[BUFFER_SIZE];
    printf("ФИО: ");
    while (getchar() != '\n');
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    Student* student = get(students_map, buffer);
    if (student != NULL) {
        print_student(student);
    }
    else {
        printf("Студент не найден\n");
    }
}

int main() {
    setlocale(LC_ALL, "");
    Hashmap* students_map = createHashmap(INITIAL_SIZE, LOAD_FACTOR, MULTIPLIER);
    load_students_from_file(&students_map, FILE_NAME);
    int type_task2;
    while (1) {
        printf("0 - выйти\n1 - добавить студента\n2 - удалить студента\n3 - показать студентов\n4 - сортировать дисциплины\n5 - удалить студентов с плохими оценками\n6 - найти студента по ФИО\nВыберите действие: ");
        scanf("%d", &type_task2);
        if (type_task2 == 0) break;
        switch (type_task2) {
        case 1: case_add(&students_map);  break;
        case 2: case_delete(students_map); break;
        case 3: mapIterate(students_map, display_student, NULL); break;
        case 4: sort_disciplines_by_average(students_map); break;
        case 5: remove_students_with_low_marks(students_map); break;
        case 6: case_find_students(students_map);  break;
        }
        save_students(students_map, FILE_NAME);
    }
    free_memory(&students_map);
    return 0;
}
