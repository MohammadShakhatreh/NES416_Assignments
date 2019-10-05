#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define COURSES_LEN 4

struct Course {
    char name[200];
    char code[20];
    int credit;
    float grade;

} courses[COURSES_LEN]; // Declare an array of courses

void print_course(struct Course course) {
    printf("%s %s %.2f %d\n", course.name, course.code, course.grade, course.credit);
}

/*
* display the commands the program understands.
*/
int menu() {
    puts("1 - Diplay all courses information.");
    puts("2 - Compute GPA of all courses.");
    puts("3 - Display highest grade and the corresponding course name.");
    puts("4 - Display lowest grade and the corresponding course name");
    puts("5 - Display course(s) code(s) whose credit hours are based on your input.");
    puts("6 - Display course information for given course code.");
    puts("7 - Display the grade as letter format for given course code(ex: 85 is B).");
    puts("8 - Exit.");
    printf("> ");

    int choice = 0;
    scanf("%d", &choice);

    return choice;
}

/*
* loads courses from a file and puts them in the global array courses.
* file format is name, code, credit, grade
*/
int load_courses(char *filename) {
    FILE *fp = fopen(filename, "r");

    // File failed to open
    if (fp == NULL)
        return -1;

    for (int i = 0; i < COURSES_LEN; ++i){
        // [^,\n] means read a string until a comma or newline
        fscanf(fp, "%[^,\n], %[^,\n], %d, %f\n",
            courses[i].name,
            courses[i].code,
            &courses[i].credit,
            &courses[i].grade
        );
    }

    fclose(fp);
    return 0;
}

int main(int argc, char *argv[]){
    // The program accepts a filename argument to load the courses from.
    if (argc != 2) {
        printf("usage: %s filename\n", argv[0]);
        return -1;
    }

    // Trying to load the courses.
    char *filename = argv[1];
    if (load_courses(filename) != 0){
        printf("failed to open the file\n");
        return -1;
    }

    bool exit = false;
    while(!exit) {
        int choice = menu();

        switch(choice) {
            case 1:
                display_courses_info();
                break;
            case 2:
                compute_gpa();
                break;
        }
    }

    return 0;
}