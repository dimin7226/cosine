#include "fun.h"

void input_marks(Student* student) {
    char buffer[256];
    (*student).marks = malloc((*student).num_marks * sizeof(Mark));
    for (int i = 0; i < (*student).num_marks; i++) {
        printf("Введите название дисциплины (за оценку %d): ", i + 1);
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        (*student).marks[i].discipline = malloc(strlen(buffer) + 1);
        strcpy((*student).marks[i].discipline, buffer);
        printf("Оценка за %s: ", buffer);
        scanf("%d", &(*student).marks[i].mark);
        while (getchar() != '\n');
    }
}

void input_events(Student* student) {
    char buffer[256];
    (*student).events = malloc((*student).num_events * sizeof(Event));
    for (int i = 0; i < (*student).num_events; i++) {
        printf("Введите тип события (0 - зачисление, 1 - отчисление, 2 - востановление): ");
        scanf("%d", (int*)&(*student).events[i].type);
        while (getchar() != '\n');
        printf("Введите дату события (YYYY.MM.DD): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        strcpy((*student).events[i].date, buffer);
    }
}

Student input_student() {
    Student student = { 0 };
    char buffer[256];
    printf("Номер зачётки: ");
    scanf("%lu", &student.credit_card_number);
    while (getchar() != '\n');
    printf("ФИО: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    student.full_name = malloc(strlen(buffer) + 1);
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
void save_students(Student* students, int num_students, char* file_name) {
    FILE* file;
    if ((file = fopen(file_name, "w")) == NULL) {
        printf("Файл \"%s\" не может быть открыт\n", file_name);
    }
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
    *students = realloc(*students, (*num_students + 1) * sizeof(Student));
    (*students)[*num_students] = new_student;
    (*num_students)++;
}

void display_students(Student* students, int num_students) {
    for (int i = 0; i < num_students; ++i) {
        printf("Номер зачётки: %lu, ФИО: %s\n", students[i].credit_card_number, students[i].full_name);
        printf("Оценки:\n");
        for (int j = 0; j < students[i].num_marks; ++j) {
            printf("\tДисциплина: %s, Оценка: %d\n", students[i].marks[j].discipline, students[i].marks[j].mark);
        }
        printf("События:\n");
        for (int k = 0; k < students[i].num_events; ++k) {
            char* event_type_str;
            switch (students[i].events[k].type) {
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
            printf("\tТип события: %s, Дата: %s\n", event_type_str, students[i].events[k].date);
        }
        printf("\n");
    }
}
void delete_student(Student** students, int* num_students, int index) {
    if (index < 0 || index >= *num_students) {
        printf("Неизвестный индекс\n");
        exit(1);
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
    *students = realloc(*students, (*num_students) * sizeof(Student));
    if (*students == NULL && *num_students > 0) {
        printf("Ошибка перераспределения памяти");
        exit(1);
    }
}

int compare_discipline_average(const void* a, const void* b) {
    const DisciplineAverage* da = (const DisciplineAverage*)a;
    const DisciplineAverage* db = (const DisciplineAverage*)b;
    if ((*da).average != (*db).average) {
        return ((*db).average - (*da).average) > 0 ? 1 : -1;
    }
    return strcmp((*da).discipline, (*db).discipline);
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
                    DisciplineAverage* temp = realloc(*averages, (*capacity) * sizeof(DisciplineAverage));
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
    printf("Дисциплины, сортрованные по среднему баллу:\n");
    for (int i = 0; i < num_averages; i++) {
        printf("%s: %.2f\n", averages[i].discipline, averages[i].average);
    }
}

void sort_disciplines_by_average(Student* students, int num_students) {
    int capacity = 10;
    DisciplineAverage* averages = malloc(capacity * sizeof(DisciplineAverage));
    if (!averages) {
        perror("Ошибка выделения памяти");
        return;
    }
    int num_averages = 0;
    calculate_sums_and_counts(&averages, &num_averages, &capacity, students, num_students);
    calculate_averages(averages, num_averages);
    qsort(averages, num_averages, sizeof(DisciplineAverage), compare_discipline_average);
    display_sorted_disciplines(averages, num_averages);
    free(averages);
}

void read_marks(FILE* file, Student* student) {
    char discipline[256];
    int mark;
    student->marks = malloc(student->num_marks * sizeof(Mark));
    for (int i = 0; i < student->num_marks; ++i) {
        if (fscanf(file, "%255[,],%d\n", discipline, &mark) == 2) {
            student->marks[i].discipline = malloc(strlen(discipline) + 1);
            strcpy(student->marks[i].discipline, discipline);
            student->marks[i].mark = mark;
        }
        else {
            fprintf(stderr, "Ошибка чтения файла.\n");
            exit(1);
        }
    }
}

void read_events(FILE* file, Student* student) {
    char date[11];
    int eventType;
    student->events = malloc(student->num_events * sizeof(Event));
    for (int i = 0; i < student->num_events; ++i) {
        if (fscanf(file, "%d,%10[\n]\n", &eventType, date) == 2) {
            student->events[i].type = (EventType)eventType;
            strcpy(student->events[i].date, date);
        }
        else {
            fprintf(stderr, "Ошибка чтения файла\n");
            exit(EXIT_FAILURE);
        }
    }
}

void read_student_details(FILE* file, Student** students, int* num_students) {
    char buffer[256];
    unsigned long credit_card_number;
    int num_marks, num_events;
    for (int i = 0; i < *num_students; i++) {
        Student student;
        fscanf(file, "%lu\n", &credit_card_number);
        student.credit_card_number = credit_card_number; 
        fscanf(file, "%s\n", buffer);
        student.full_name = malloc(strlen(buffer) + 1);
        strcpy(student.full_name, buffer);
        fscanf(file, "%d\n", &num_marks);
        student.num_marks = num_marks;;
        read_marks(file, &student);
        scanf(file, "%d\n", &num_events);
        student.num_events = num_events;
        read_events(file, &student);
        add_student(students, num_students, student);
    }
}

void load_students_from_file(Student** students, int* num_students, char* file_name) {
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        printf("Файл \"%s\" не может быть открыт\n", file_name);
        exit(1);
    }
    fseek(file, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(file);
    if (len == 0) {
        fclose(file);
        return;
    }
    fseek(file, 0, SEEK_SET);
    read_student_details(file, students, num_students);
    fclose(file);
}
void remove_students_with_low_marks(Student** students, int* num_students) {
    for (int i = 0; i < *num_students; i++) {
        int is_currently_enrolled = 0, has_low_mark = 0, last_index = 0;
        char* temp = '\0';
        if ((*students)[i].num_events > 0) temp = (*students)[i].events[0].date;
        for (int j = 1; j < (*students)[i].num_events; j++) {
            if (strcmp(temp, (*students)[i].events[j].date) <= 0) {
                temp = (*students)[i].events[j].date;
                last_index = j;
            }
        }
        if ((*students)[i].events[last_index].type != DROPOUT) is_currently_enrolled = 1;
        if (is_currently_enrolled) {
            for (int j = 0; j < (*students)[i].num_marks; ++j) {
                if ((*students)[i].marks[j].mark < 4) {
                    delete_student(students, num_students, i);
                    break;
                }
            }
        }
    }
}

void task2() {
    Student* students = NULL;
    int num_students = 0, type_task2;
    load_students_from_file(&students, &num_students, "students.txt");
    while (1) {
        printf("0 - выйти\n1 - добавить студента\n2 - удалить студента\n3 - показать студентов\n4 - сортировать дисциплины\n5 - удалить студентов с плохими оценками\nВыберите действие: ");
        scanf_s("%d", &type_task2);
        if (type_task2 == 0) break;
        switch (type_task2) {
        case 1: add_student(&students, &num_students, input_student()); break;
        case 2: printf("Индекс: "); scanf("%d", &type_task2); delete_student(&students, &num_students, type_task2); break;
        case 3: display_students(students, num_students); break;
        case 4: sort_disciplines_by_average(students, num_students); break;
        case 5: remove_students_with_low_marks(&students, &num_students); break;
        }
        //save_students(students, num_students, "students.txt");
    }
}

