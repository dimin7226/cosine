#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define FILE_NAME "students.txt"
#define TABLE_SIZE 100
#define DATE_SIZE 11
#define BUFFER_SIZE 256
#define HASH_NUMB 5381

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

Student* hash_table[TABLE_SIZE] = { NULL };

unsigned int hash(const char* str) {
    unsigned long hash = HASH_NUMB;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % TABLE_SIZE;
}

void add_student_to_hash_table(Student* student) {
    unsigned int index = hash(student->full_name);
    student->next = hash_table[index];
    hash_table[index] = student;
}

void find_students(const char* full_name, Student** found_students, int* count) {
    unsigned int index = hash(full_name);
    Student* student = hash_table[index];
    *count = 0;
    while (student != NULL) {
        if (strcmp(student->full_name, full_name) == 0) {
            found_students[*count] = student;
            (*count)++;
        }
        student = student->next;
    }
}

void delete_student(Student** students, int* num_students, int index) {
    if (index < 0 || index >= *num_students) {
        printf("Неизвестный индекс\n");
        return;
    }
    free((*students)[index].full_name);
    for (int i = 0; i < (*students)[index].num_marks; i++) {
        free((*students)[index].marks[i].discipline);
    }
    free((*students)[index].marks);
    free((*students)[index].events);
    for (int i = index; i < *num_students - 1; ++i) {
        (*students)[i] = (*students)[i + 1];
    }
    (*num_students)--;
    *students = (Student*)realloc(*students, (*num_students) * sizeof(Student));
    if (*students == NULL && *num_students > 0) {
        printf("Ошибка перераспределения памяти");
        exit(1);
    }
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

Student input_student() {
    Student student = { 0 };
    char buffer[BUFFER_SIZE];
    printf("Номер зачётки: ");
    scanf("%lu", &student.credit_card_number);
    while (getchar() != '\n');
    printf("ФИО: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    student.full_name = (char*)malloc(strlen(buffer) + 1);
    strcpy(student.full_name, buffer);
    printf("Количество оценок: ");
    scanf("%d", &student.num_marks);
    while (getchar() != '\n');
    input_marks(&student);
    printf("Введите количество событий: ");
    scanf("%d", &student.num_events);
    while (getchar() != '\n');
    input_events(&student);
    return student;
}

void display_students(Student* students, int num_students) {
    for (int i = 0; i < num_students; ++i) {
        printf("Номер зачётки: %lu, ФИО: %s\n", students[i].credit_card_number, students[i].full_name);
        printf("Оценки:\n");
        for (int j = 0; j < students[i].num_marks; ++j)
            printf("\tДисциплина: %s, Оценка: %d\n", students[i].marks[j].discipline, students[i].marks[j].mark);
        printf("События:\n");
        for (int k = 0; k < students[i].num_events; ++k) {
            char* event_type_str;
            switch (students[i].events[k].type) {
            case ENROLLMENT:
                event_type_str = "Зачисление";  break;
            case DROPOUT:
                event_type_str = "Отчисление";  break;
            case REINSTATEMENT:
                event_type_str = "Восстановление";  break;
            default:
                event_type_str = "Неизвестное событие"; break;
            }
            printf("\tТип события: %s, Дата: %s\n", event_type_str, students[i].events[k].date);
        }
        printf("\n");
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
                    *capacity++;
                    DisciplineAverage* temp = (DisciplineAverage*)realloc(*averages, (*capacity) * sizeof(DisciplineAverage));
                    if (!temp) {
                        printf("Ошибка перераспределения памяти");
                        free(*averages);
                        exit(1);
                    }
                    *averages = temp;
                }
                (*averages)[*num_averages].discipline = students[i].marks[j].discipline;
                (*averages)[*num_averages].average = students[i].marks[j].mark;
                (*averages)[*num_averages].count = 1;
                (*num_averages)++;
            }
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
    display_sorted_disciplines(averages, num_averages);
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

void remove_students_with_low_marks(Student** students, int* num_students) {
    for (int i = 0; i < *num_students; i++) {
        int has_low_mark = 0, last_index = 0;
        if ((*students)[i].num_events > 0) {
            char* temp = (*students)[i].events[0].date;
            for (int j = 1; j < (*students)[i].num_events; j++) {
                if (strcmp(temp, (*students)[i].events[j].date) <= 0) {
                    temp = (*students)[i].events[j].date;
                    last_index = j;
                }
            }
            if ((*students)[i].events[last_index].type != DROPOUT) {
                for (int j = 0; j < (*students)[i].num_marks; ++j) {
                    if ((*students)[i].marks[j].mark < 4) {
                        delete_student(students, num_students, i);
                        i--;
                        break;
                    }
                }
            }
        }
    }
}

void free_memory() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Student* student = hash_table[i];
        while (student) {
            Student* temp = student;
            student = student->next;
            free(temp->full_name);
            for (int j = 0; j < temp->num_marks; j++) {
                free(temp->marks[j].discipline);
            }
            free(temp->marks);
            free(temp->events);
            free(temp);
        }
    }
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

void print_student(Student* found_students) {
    printf("Найден студент: %s\nНомер зачётки: %lu\n", found_students->full_name, found_students->credit_card_number);
    printf("Оценки:\n");
    for (int j = 0; j < found_students->num_marks; ++j) {
        printf("\tДисциплина: %s, Оценка: %d\n", found_students->marks[j].discipline, found_students->marks[j].mark);
    }
    printf("События:\n");
    for (int k = 0; k < found_students->num_events; ++k) {
        char* event_type_str;
        switch (found_students->events[k].type) {
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
        printf("\tТип события: %s, Дата: %s\n", event_type_str, found_students->events[k].date);
    }
    printf("\n");
}

void case_add(Student** students, int* num_students) {
    Student new_student = input_student();
    *students = (Student*)realloc(*students, (*num_students + 1) * sizeof(Student));
    (*students)[*num_students] = new_student;
    add_student_to_hash_table(&(*students)[*num_students]);
    (*num_students)++;
}

void case_delete(Student* students, int* num_students, int type_task2) {
    printf("Индекс: ");
    scanf("%d", &type_task2);
    delete_student(&students, &num_students, type_task2);
}

void case_find_students() {
    char full_name[BUFFER_SIZE];
    printf("ФИО: ");
    while (getchar() != '\n');
    fgets(full_name, sizeof(full_name), stdin);
    full_name[strcspn(full_name, "\n")] = 0;
    Student* found_students[TABLE_SIZE];
    int count = 0;
    find_students(full_name, found_students, &count);
    if (count > 0) {
        for (int i = 0; i < count; i++) {
            print_student(found_students[i]);
        }
    }
    else {
        printf("Студент не найден\n");
    }
}

int main() {
    setlocale(LC_ALL, "");
    Student* students = NULL;
    int num_students = 0, type_task2;
    load_students_from_file(&students, &num_students, FILE_NAME);
    for (int i = 0; i < num_students; i++) {
        add_student_to_hash_table(&students[i]);
    }
    while (1) {
        printf("0 - выйти\n1 - добавить студента\n2 - удалить студента\n3 - показать студентов\n4 - сортировать дисциплины\n5 - удалить студентов с плохими оценками\n6 - найти студента по ФИО\nВыберите действие: ");
        scanf("%d", &type_task2);
        if (type_task2 == 0) break;
        switch (type_task2) {
        case 1: case_add(&students, &num_students);  break;
        case 2: case_delete(students, &num_students, type_task2);
        case 3: display_students(students, num_students); break;
        case 4: sort_disciplines_by_average(students, num_students); break;
        case 5: remove_students_with_low_marks(&students, &num_students); break;
        case 6: case_find_students();  break;
        }
        save_students(students, num_students, FILE_NAME);
    }
    free_memory();
    return 0;
}

