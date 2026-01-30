#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

typedef struct
{
    char *id;
    char *name;
    char gender;
    int age;
    char *pos;
} patient;

patient *patients = NULL;

int len = 0;

void print_truncated(const char *str, int width)
{
    int len = strlen(str);
    if (len > width)
    {
        for (int i = 0; i < width; i++)
            putchar(str[i]);
        printf("..."); // truncated indicator
        // pad to keep column aligned
        for (int i = 0; i < (width - 3); i++)
            putchar(' ');
    }
    else
    {
        printf("%-*s", width, str); // left-align with padding
    }
}
char *generateId(char gender, int is_under_18)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    int day = t->tm_mday;
    int month = t->tm_mon + 1;
    int year = t->tm_year % 100;
    char month_char = (month <= 9) ? ('0' + month) : ('0' + (month - 10));

    char age_char = is_under_18 ? 'U' : 'A';

    char *id = malloc(9 * sizeof(char));

    if (!id)
        return NULL;

    snprintf(id, 9, "%02d%c%02dP%c%c", day, month_char, year, gender, age_char);

    return id;
}

void freeBuffer()
{
    while (getchar() != '\n')
        ;
}

void addPatient(const char *name, int age, char gender, const char *positon)
{
    patient *temp = (patient *)realloc(patients, (len + 1) * sizeof(patient));

    if (temp == NULL)
    {
        printf("Memory allocation failed !");
    }
    else
    {
        patients = temp;
    }

    patients[len].name = (char *)malloc((strlen(name)) + 1);
    strcpy(patients[len].name, name);

    patients[len].age = age;

    patients[len].gender = gender;

    patients[len].pos = (char *)malloc(strlen(positon) + 1);
    strcpy(patients[len].pos, positon);

    patients[len].id = generateId(gender, age < 18);

    len++;
}
void showAll()
{
    printf("=====================================================================================================\n");
    printf("| %-4s | %-20s | %-4s | %-10s | %-30s | %-9s |\n",
           "Sln", "Name", "Age", "Gender", "Disease/Problem", "ID");
    printf("-----------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < len; i++)
    {
        printf("| %-4d | ", i + 1);
        print_truncated(patients[i].name, 20);
        printf(" | %-4d | %-10c | ", patients[i].age, patients[i].gender);
        print_truncated(patients[i].pos, 30);
        printf(" | %-9s |\n", patients[i].id);
        printf("-----------------------------------------------------------------------------------------------------\n");
    }

    printf("printed succesfully all patients!");
}
void removePatient();

int main()
{
    int running = 1;
    char inputName[1024], inputPos[1024], inputGender;
    int inputAge;
    int chs;
    while (running)
    {
        printf("\n\n\n");
        printf("------------Hospitl Database Management-------------");
        printf("\n1.Add Patient\n2.Remove Patient\n3.Show all patients\n4.Show reports\n5.Exit\n");
        scanf(" %d", &chs);
        switch (chs)
        {
        case 1:
            freeBuffer();
            printf("Enter Patient's name :");
            fgets(inputName, sizeof(inputName), stdin);
            inputName[strcspn(inputName, "\n")] = 0;

            printf("Enter Age(1-110) :");
            scanf(" %d", &inputAge);
            if (inputAge < 1 || inputAge > 110)
                printf("Invalid age Input !!");
            freeBuffer();
            printf("Enter Gender(M/F/O) : ");
            scanf(" %c", &inputGender);
            if (inputGender != 'M' && inputGender != 'F' && inputGender != 'O')
                printf("Invalid choice ");
            freeBuffer();
            printf("Enter the problem/disease : ");
            fgets(inputPos, sizeof(inputPos), stdin);
            inputPos[strcspn(inputPos, "\n")] = 0;
            addPatient(inputName, inputAge, inputGender, inputPos);
            break;
        case 3:
            showAll();
            break;
        case 5:
            printf("EXITING.....\n");
            running = 0;
            break;
        default:
            printf("Error - wrong input\n");
            break;
        }
    }
}
