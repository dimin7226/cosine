#include "fun.h"

int main() {
	setlocale(LC_ALL, "");
	int task;
	while (1) {
		printf("Введите номер задания?\n");
		scanf_s("%d", &task);
		if (task == 1) {
			task1();
		}
		else if (task == 2) {
			task2();
		}
		printf("Для выхода введите\"0\"\n");
		scanf_s("%d", &task);
		if (task == 0) return 0;
		else printf(DELIMITER"\n");
	}
	return 0;
}
