#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <windows.h>
#include <direct.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>
#include <libgen.h>
#include <unistd.h>

#define basement "C:\\tig base"
#define config "config"
#define init "init"
#define add "add"
#define reset "reset"
#define status "status"
#define commit "commit"
#define set "set"
#define replace "replace"
#define log "log"
#define branch "branch"
#define checkout "checkout"
#define revert "revert"
#define tag "tag"
#define grep "grep"
#define diff "diff"
#define precommit "pre-commit"
#define merge "merge"

char confname[100], confmail[100];

int check_file_existance(const char *file_name);
void reach_tig();
void add_to_staging_area(const char *file_name);
void remove_from_staging_area(const char *file_name);
int dir_existance();
void init_();
void config_(int tedad, const char *voroodi[]);
void add_(int tedad, const char *voroodi[]);
void find_files_with_wildcard(const char *wildcard, void (*callback)(const char *file_name));
void add_with_wildcard(const char *wildcard);
void add_directory_to_staging_area(int depht, int level);
void remove_directory_from_staging_area(int depht, int level);
int is_directory_or_file(const char *file_name);
void reset_(int tedad, const char *voroodi[]);
void status_();
void add_to_staging_directory(const char *full_address, const char *file_name);
void remove_from_staging_directory(const char *full_address);
char* get_relative_path(const char* full_path, const char* base_path);
void update_staged_file(const char* full_address);
void undo_();
void commit_(int tedad, const char *voroodi[]);
int is_directory_empty();
void set_(int tedad, const char *voroodi[]);
void replace_(int tedad, const char *voroodi[]);
void remove_(int tedad, const char *voroodi[]);
void append_to_start(const char *file_name, const char *data);
void log_(int tedad, const char *voroodi[]);
void branch_(int tedad, const char *voroodi[]);
void checkout_(int tedad, const char *voroodi[]);
void clear_cwd();
void checkout_with_ID(char *commit_ID);
int does_folder_exist(const char *folder_path);
void status_all();
int check_uncommited_change();
// void revert_(int tedad, const char *voroodi[]);
void tag_(int tedad, const char *voroodi[]);
void tag_sort_and_print();
int compare_strings(const void *a, const void *b);
void show_tag(const char *tag_name);
void grep_(int tedad, const char *voroodi[]);
void find_and_highlight(FILE *file, const char *word, int flag);
void diff_(int tedad, const char *voroodi[]);
void linear_diff(char *address1, char *address2, int start1, int start2, int end1, int end2);
void full_diff(char *address1, char *address2);
void precommit_(int tedad, const char *voroodi[]);
void pre_commit_given_order(const char *order);

int check_balance(const char *file_name) {
    FILE *file = fopen(file_name, "r");

    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int ch;
    int parentheses = 0, curly_brackets = 0, square_brackets = 0;

    while ((ch = fgetc(file)) != EOF) {
        switch (ch) {
            case '(':
                parentheses++;
                break;
            case ')':
                parentheses--;
                break;
            case '{':
                curly_brackets++;
                break;
            case '}':
                curly_brackets--;
                break;
            case '[':
                square_brackets++;
                break;
            case ']':
                square_brackets--;
                break;
        }
    }

    fclose(file);

    return (parentheses == 0) && (curly_brackets == 0) && (square_brackets == 0);
}

int has_trailing_whitespace(const char *file_name) {
    FILE *file = fopen(file_name, "r");

    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int ch;
    int last_char = EOF;
    while ((ch = fgetc(file)) != EOF) {
        if (!isspace(ch)) {
            last_char = ch;
        }
    }

    fclose(file);

    return !isspace(last_char);
}

int file_size_check(const char *filename) {
    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open the file\n");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);

    return size > 5 * 1024 * 1024;
}

int check_file_character_count(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open the file\n");
        return -1;
    }

    int character_count = 0;
    int ch;

    while ((ch = fgetc(file)) != EOF) {
        character_count++;
    }
    fclose(file);

    return character_count > 20000;
}

int check_file_duration(char *filename, int thresholdMinutes) {
    char command[512];
    snprintf(command, sizeof(command), "ffmpeg -i \"%s\" 2>&1 | grep Duration", filename);

    FILE *pipe = popen(command, "r");
    if (pipe == NULL) {
        // perror("Error opening pipe");
        return -1;
    }

    char durationLine[256];
    if (fgets(durationLine, sizeof(durationLine), pipe) == NULL) {
        // perror("Error reading from pipe");
        pclose(pipe);
        return -1;
    }

    pclose(pipe);

    char *durationStart = strstr(durationLine, "Duration: ");
    if (durationStart == NULL) {
        fprintf(stderr, "Error: Duration not found in the output.\n");
        return -1;
    }

    int hours, minutes, seconds;
    if (sscanf(durationStart, "Duration: %d:%d:%d", &hours, &minutes, &seconds) != 3) {
        fprintf(stderr, "Error: Failed to parse duration.\n");
        return -1;
    }

    int totalMinutes = hours * 60 + minutes;

    return totalMinutes > thresholdMinutes ? 1 : 0;
}

void pre_commit_given_order(const char *order)
{
    FILE *ptr = fopen("datas.txt", "r");
    char garbage[100], file_name[50];
    fscanf(ptr, "%s\n", garbage);
    fgets(garbage, sizeof(garbage), ptr);
    fgets(file_name, sizeof(file_name), ptr);
    if(!strcmp(order, "time-limit"))
    {
        int check = check_file_duration(file_name, 10);
        if(check == 1)
        {
            printf("\x1b[32mtime-limit..........passed\x1b[0m");
            return;
        }
        else if(check == 0)
        {
            printf("\x1b[31mtime-limit..........failed\x1b[0m");
            return;
        }
        else if(check == -1)
        {
            printf("\x1b[34mtime-limit..........skipped\x1b[0m");
            return;
        }
    }

    else if(!strcmp(order, "character-limit"))
    {
        if(strcmp(strstr(file_name, ".txt"), ".txt") && strcmp(strstr(file_name, ".c"), ".c") && strcmp(strstr(file_name, ".cpp"), ".cpp"))
        {
            printf("\x1b[34mcharacter-limit..........skipped\x1b[0m");
            return;
        }
        int check = check_file_character_count(file_name);
        if(check == 1)
        {
            printf("\x1b[32mcharacter-limit..........passed\x1b[0m");
            return;
        }
        else if(check == 0)
        {
            printf("\x1b[31mcharacter-limit..........failed\x1b[0m");
            return;
        }
    }

    else if(!strcmp(order, "file-size-check"))
    {
        if(file_size_check(file_name))
        {
            printf("\x1b[32mfile-size-check..........passed\x1b[0m");
            return;
        }
        else
        {
            printf("\x1b[31mfile-size-check..........failed\x1b[0m");
            return;
        }
    }

    else if(!strcmp(order, "eof-blank-space"))
    {
        if(strcmp(strstr(file_name, ".txt"), ".txt") && strcmp(strstr(file_name, ".c"), ".c") && strcmp(strstr(file_name, ".cpp"), ".cpp"))
        {
            printf("\x1b[34meof-blank-space..........skipped\x1b[0m");
            return;
        }
        int check = has_trailing_whitespace(file_name);
        if(check == 1)
        {
            printf("\x1b[32meof-blank-space..........passed\x1b[0m");
            return;
        }
        else if(check == 0)
        {
            printf("\x1b[31meof-blank-space..........failed\x1b[0m");
            return;
        }
    }

    else if(!strcmp(order, "balance-braces"))
    {
        if(strcmp(strstr(file_name, ".txt"), ".txt") && strcmp(strstr(file_name, ".c"), ".c") && strcmp(strstr(file_name, ".cpp"), ".cpp"))
        {
            printf("\x1b[34mbalance-braces..........skipped\x1b[0m");
            return;
        }
        int check = has_trailing_whitespace(file_name);
        if(check == 1)
        {
            printf("\x1b[32mbalance-braces..........passed\x1b[0m");
            return;
        }
        else if(check == 0)
        {
            printf("\x1b[31mbalance-braces..........failed\x1b[0m");
            return;
        }
    }

    else if(!strcmp(order, "balance-braces"))
    {
        if(!strcmp(strstr(file_name, ".txt"), ".txt") || !strcmp(strstr(file_name, ".c"), ".c") || !strcmp(strstr(file_name, ".cpp"), ".cpp") || !strcmp(strstr(file_name, ".mp4"), ".mp4") || !strcmp(strstr(file_name, ".mp3"), ".mp3") || !strcmp(strstr(file_name, ".wav"), ".wav"))
        {
            printf("\x1b[32mbalance-braces..........passed\x1b[0m");
            return;
        }
        else
        {
            printf("\x1b[31mbalance-braces..........failed\x1b[0m");
            return;
        }
    }
}

void full_diff(char *address1, char *address2)
{
    char full_address1_cp[MAX_PATH], full_address2_cp[MAX_PATH];
    strcpy(full_address1_cp, address1);
    strcpy(full_address2_cp, address2);
    char *file_name1 = basename(full_address1_cp);
    char *file_name2 = basename(full_address2_cp);
    FILE *ptr1 = fopen(address1, "r");
    FILE *ptr2 = fopen(address2, "r");
    char line1[200], line2[200];
    int index1 = 0, index2 = 0;
    void *a, *b;

    a = fgets(line1, sizeof(line1), ptr1);
    index1++;
    while(1)
    {
        int white_space = 0;
        for(int j = 0; j < strlen(line1); j++)
        {
            if(line1[j] != ' ' && line1[j] != '\n' && line1[j] != '\t')
            {
                white_space = 1;
                break;
            }
        }
        if(white_space)
        {
            break;
        }
        else
        {
            a = fgets(line1, sizeof(line1), ptr1);
            index1++;
        }
    }

    b = fgets(line2, sizeof(line2), ptr2);
    index2++;
    while(1)
    {
        int white_space = 0;
        for(int j = 0; j < strlen(line2); j++)
        {
            if(line2[j] != ' ' && line2[j] != '\n' && line2[j] != '\t')
            {
                white_space = 1;
                break;
            }
        }
        if(white_space)
        {
            break;
        }
        else
        {
            b = fgets(line2, sizeof(line2), ptr2);
            index2++;
        }
    }
    
    while (a != NULL || b != NULL)
    {
        if(strcmp(line1, line2))
        {
            printf("<<<<<\n\x1b[34m%s-%d\n%s\x1b[36m%s-%d\n%s\x1b[0m>>>>>", file_name1, index1, line1, file_name2, index2, line2);
        }

        a = fgets(line1, sizeof(line1), ptr1);
        index1++;
        while(1)
        {
            int white_space = 0;
            for(int j = 0; j < strlen(line1); j++)
            {
                if(line1[j] != ' ' && line1[j] != '\n' && line1[j] != '\t')
                {
                    white_space = 1;
                    break;
                }
            }
            if(white_space)
            {
                break;
            }
            else
            {
                a = fgets(line1, sizeof(line1), ptr1);
                index1++;
            }
        }

        b = fgets(line2, sizeof(line2), ptr2);
        index2++;
        while(1)
        {
            int white_space = 0;
            for(int j = 0; j < strlen(line2); j++)
            {
                if(line2[j] != ' ' && line2[j] != '\n' && line2[j] != '\t')
                {
                    white_space = 1;
                    break;
                }
            }
            if(white_space)
            {
                break;
            }
            else
            {
                b = fgets(line2, sizeof(line2), ptr2);
                index2++;
            }
        }
    }
}

void linear_diff(char *address1, char *address2, int start1, int start2, int end1, int end2)
{
    char full_address1_cp[MAX_PATH], full_address2_cp[MAX_PATH];
    strcpy(full_address1_cp, address1);
    strcpy(full_address2_cp, address2);
    char *file_name1 = basename(full_address1_cp);
    char *file_name2 = basename(full_address2_cp);
    FILE *ptr1 = fopen(address1, "r");
    FILE *ptr2 = fopen(address2, "r");
    char line1[200], line2[200];
    int index1 = 0, index2 = 0;
    void *a, *b;
    
    for(int i = 1; i <= start1; i++)
    {
        a = fgets(line1, sizeof(line1), ptr1);
        index1++;
        while(1)
        {
            int white_space = 0;
            for(int j = 0; j < strlen(line1); j++)
            {
                if(line1[j] != ' ' && line1[j] != '\n' && line1[j] != '\t')
                {
                    white_space = 1;
                    break;
                }
            }
            if(white_space)
            {
                break;
            }
            else
            {
                a = fgets(line1, sizeof(line1), ptr1);
                index1++;
            }
        }
    }

    
    
    for(int i = 1; i <= start2; i++)
    {
        b = fgets(line2, sizeof(line2), ptr2);
        index2++;
        while(1)
        {
            int white_space = 0;
            for(int j = 0; j < strlen(line2); j++)
            {
                if(line2[j] != ' ' && line2[j] != '\n' && line2[j] != '\t')
                {
                    white_space = 1;
                    break;
                }
            }
            if(white_space)
            {
                break;
            }
            else
            {
                b = fgets(line2, sizeof(line2), ptr2);
                index2++;
            }
        }
    }
    
    while (index1 <= end1 || index2 <= end2 || a != NULL || b != NULL)
    {
        if(strcmp(line1, line2))
        {
            printf("<<<<<\n\x1b[34m%s-%d\n%s\x1b[36m%s-%d\n%s\x1b[0m>>>>>", file_name1, index1, line1, file_name2, index2, line2);
        }

        a = fgets(line1, sizeof(line1), ptr1);
        index1++;
        while(1)
        {
            int white_space = 0;
            for(int j = 0; j < strlen(line1); j++)
            {
                if(line1[j] != ' ' && line1[j] != '\n' && line1[j] != '\t')
                {
                    white_space = 1;
                    break;
                }
            }
            if(white_space)
            {
                break;
            }
            else
            {
                a = fgets(line1, sizeof(line1), ptr1);
                index1++;
            }
        }

        b = fgets(line2, sizeof(line2), ptr2);
        index2++;
        while(1)
        {
            int white_space = 0;
            for(int j = 0; j < strlen(line2); j++)
            {
                if(line2[j] != ' ' && line2[j] != '\n' && line2[j] != '\t')
                {
                    white_space = 1;
                    break;
                }
            }
            if(white_space)
            {
                break;
            }
            else
            {
                b = fgets(line2, sizeof(line2), ptr2);
                index2++;
            }
        }
    }
}

void find_and_highlight(FILE *file, const char *word, int flag)
{
    int index = 0;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        index++;
        char *position = strstr(buffer, word);
        if (position != NULL)
        {
            size_t prefix_length = position - buffer;
            size_t suffix_length = strlen(buffer) - prefix_length - strlen(word);
            if(flag)
            {
                printf("line %d: ", index);
            }
            char prefix[100];
            strncpy(prefix, buffer, prefix_length);
            prefix[prefix_length] = '\0';
            printf("%s", prefix);
            printf("\x1b[31m");
            printf("%s", word);
            printf("\x1b[0m");
            printf("%s\n", position + strlen(word));
        }
    }
    return;
}

void show_tag(const char *tag_name)
{
    char t_name[100], t_massage[100], c_ID[10], c_name[100], date[100], c_mail[100];
    FILE *ptr = fopen("tags.txt", "r");
    while (fgets(t_name, sizeof(t_name), ptr))
    {
        fgets(t_massage, sizeof(t_massage), ptr);
        fgets(c_ID, sizeof(c_ID), ptr);
        fgets(c_name, sizeof(c_name), ptr);
        fgets(c_mail, sizeof(c_mail), ptr);
        fgets(date, sizeof(date), ptr);
        t_name[strlen(t_name) - 1] = '\0';
        c_name[strlen(c_name) - 1] = '\0';
        c_mail[strlen(c_mail) - 1] = '\0';
        if(!strcmp(t_name, tag_name))
        {
            printf("tag: %s\n", t_name);
            printf("commit: %s", c_ID);
            printf("Author: %s<%s>\n", c_name, c_mail);
            printf("date: %s", date);
            if(strlen(t_massage) > 1)
            {
                printf("message: %s", t_massage);
            }
            else
            {
                printf("there is no message for this tag\n");
            }
        }
    }
    
}

int compare_strings(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

void tag_sort_and_print()
{
    int i = 0;
    char *tags[100], garbage[100], line[100];
    FILE *ptr = fopen("tags.txt", "r");
    while (fgets(line, sizeof(line), ptr) != NULL && strcmp(line, "\n"))
    {
        fgets(garbage, sizeof(garbage), ptr);
        fgets(garbage, sizeof(garbage), ptr);
        fgets(garbage, sizeof(garbage), ptr);
        fgets(garbage, sizeof(garbage), ptr);
        fgets(garbage, sizeof(garbage), ptr);
        strcpy(tags[i], line);
        i++;
    }
    fclose(ptr);
    qsort(tags, i, sizeof(tags[0]), compare_strings);
    for(int j = 0; j < i; j++)
    {
        printf("%s", tags[j]);
        // printf("^\n");
    }
}

int check_uncommited_change()
{
    struct dirent *entry;
    DIR *dir = opendir(".");

    char X, Y;
    char *current_dir = _getcwd(NULL, MAX_PATH);
    char branch_name[80], branch_name_2[80], garbage[100], index[5];
    reach_tig();
    FILE *commit_ptr = fopen("commit history.txt", "r");
    FILE *branch_ptr = fopen("branch.txt", "r");
    fgets(branch_name, sizeof(branch_name), branch_ptr);
    while(fgets(garbage, sizeof(garbage), commit_ptr) != NULL)
    {
        fgets(index, sizeof(index), commit_ptr);
        fgets(garbage, sizeof(garbage), commit_ptr);
        fgets(garbage, sizeof(garbage), commit_ptr);
        fgets(branch_name_2, sizeof(branch_name_2), commit_ptr);
        fgets(garbage, sizeof(garbage), commit_ptr);
        fgets(garbage, sizeof(garbage), commit_ptr);
        branch_name_2[strlen(branch_name_2) - 1] = '\0';
        index[strlen(index) - 1] = '\0';
        if(!strcmp(branch_name, branch_name_2))
        {
            break;
        }
    }
    fclose(commit_ptr);
    fclose(branch_ptr);
    SetCurrentDirectory(current_dir);
    // printf("%s\n\n", current_dir);
    while((entry = readdir(dir)) != NULL)
    {
        if(strcmp(entry -> d_name, ".") == 0 || strcmp(entry -> d_name, "..") == 0 || strcmp(entry -> d_name, "tig") == 0)
        {
            continue;
        }

        else if((entry -> d_type != DT_DIR))
        {
            FILE *main_file = fopen(entry -> d_name, "r");
            reach_tig();
            char full_address[MAX_PATH], full_address_copy[MAX_PATH];
            sprintf(full_address, "%s\\%s", current_dir, entry -> d_name);
            strcpy(full_address_copy, full_address);
            SetCurrentDirectory("commits");
            char dir_name[260];
            dir_name[0] = '\0';
            char *token = strtok(full_address_copy, "\\:.");
            while(token != NULL)
            {
                strcat(dir_name, token);
                token = strtok(NULL, "\\:.");
            }
            SetCurrentDirectory(index);
            struct dirent *en;
            DIR *d = opendir(".");
            int ex = 0;
            while((en = readdir(d)) != NULL)
            {
                if(!strcmp(en -> d_name, dir_name))
                {
                    ex = 1;
                    break;
                }
            }
            closedir(d);
            if(!ex)
            {
                return 1;
                Y = 'A';
                goto lable1;
            }
            SetCurrentDirectory(dir_name);
            FILE *commit_file = fopen(entry -> d_name, "r");
            int same = 1;
            char line1[200], line2[200];
            while(fgets(line1, sizeof(line1), commit_file) != NULL)
            {
                if(fgets(line2, sizeof(line2), main_file) == NULL)
                {
                    same = 0;
                    break;
                }
                if(strcmp(line1, line2))
                {
                    same = 0;
                    break;
                }
            }
            if(fgets(line2, sizeof(line2), main_file) != NULL)
            {
                same = 0;
            }
            fclose(main_file);
            fclose(commit_file);
            if(same)
            {
                SetCurrentDirectory(current_dir);
                continue;
            }
            else
            {
                return 1;
                Y = 'M';
            }
            SetCurrentDirectory("..");
            lable1:
            SetCurrentDirectory("..");
            SetCurrentDirectory("..");
            SetCurrentDirectory("staging");
            if(GetFileAttributes(dir_name) == INVALID_FILE_ATTRIBUTES)
            {
                X = '-';
            }
            else
            {
                X = '+';
            }
            // printf("%s %c%c\n", entry -> d_name, X, Y);
            SetCurrentDirectory(current_dir);
            continue;
        }
        else if(entry -> d_type == DT_DIR);
        {
            // printf("11%s\n", entry -> d_name);
            SetCurrentDirectory(entry -> d_name);
            check_uncommited_change();
            SetCurrentDirectory("..");
        }
    }
    closedir(dir);
    return 0;
    // reach_tig();
    // SetCurrentDirectory("commits");
    // SetCurrentDirectory(index);
    // char *temp_address = _getcwd(NULL, MAX_PATH);
    // dir = opendir(".");
    // while ((entry = readdir(dir)) != NULL)
    // {
    //     if((entry -> d_type == DT_DIR) && strcmp(entry -> d_name, ".") && strcmp(entry -> d_name, ".."))
    //     {
    //         SetCurrentDirectory(entry -> d_name);
    //         FILE *dt = fopen("datas.txt", "r");
    //         char address[100], garbage[100], file_name[100], address_cp[MAX_PATH], address_cp_cp[MAX_PATH];
    //         fscanf(dt, "%s\n", address);
    //         fgets(garbage, sizeof(garbage), dt);
    //         fscanf(dt, "%s\n", file_name);
    //         fclose(dt);
    //         strcpy(address_cp_cp, address);
    //         if(strcmp(current_dir, dirname(address_cp_cp)))
    //         {
    //             continue;
    //         }
    //         if(GetFileAttributes(address) == INVALID_FILE_ATTRIBUTES)
    //         {
    //             strcpy(address_cp, address);
    //             char dir_name[MAX_PATH];
    //             dir_name[0] = '\0';
    //             char *token = strtok(address_cp, "\\:.");
    //             while(token != NULL)
    //             {
    //                 strcat(dir_name, token);
    //                 token = strtok(NULL, "\\:.");
    //             }
    //             reach_tig();
    //             SetCurrentDirectory("staging");
    //             if(GetFileAttributes(dir_name) != INVALID_FILE_ATTRIBUTES)
    //             {
    //                 printf("%s +D\n", file_name);
    //             }
    //             else
    //             {
    //                 printf("%s -D\n", file_name);
    //             }
    //         }
    //         fclose(dt);
    //         SetCurrentDirectory(temp_address);
    //     }
    // }
    // closedir(dir);
    // SetCurrentDirectory(current_dir);
}

int does_folder_exist(const char *folder_path){
    DIR *dir = opendir(folder_path);
    if (dir) {
        closedir(dir);
        return 1;
    } else {
        return 0;
    }
}

void checkout_with_ID(char *commit_ID)
{
    SetCurrentDirectory("..");
    clear_cwd();
    SetCurrentDirectory("tig");
    FILE *ptr = fopen("cc_ID.txt", "w");
    fprintf(ptr, "%s", commit_ID);
    fclose(ptr);
    SetCurrentDirectory("commits");
    SetCurrentDirectory(commit_ID);
    
    struct dirent *entry;
    DIR *dir = opendir(".");
    while((entry = readdir(dir)) != NULL)
    {
        if(entry -> d_type == DT_DIR && strcmp(entry -> d_name, "..") && strcmp(entry -> d_name, "."))
        {
            SetCurrentDirectory(entry -> d_name);
            char full_address[MAX_PATH], file_name[100], garbage[100], command[MAX_PATH];
            ptr = fopen("datas.txt", "r");
            fscanf(ptr, "%s\n", full_address);
            fgets(garbage, sizeof(garbage), ptr);
            fscanf(ptr, "%s", file_name);
            fclose(ptr);
            dirname(full_address);
            if(!does_folder_exist(full_address))
            {
                sprintf(command, "mkdir \"%s\"", full_address);
                system(command);
            }
            sprintf(command, "copy \"%s\\%s\" \"%s\"", _getcwd(NULL, MAX_PATH), file_name, full_address);
            system(command);
            SetCurrentDirectory("..");
        }
    }
    system("xcopy branch.txt ..\\..");
    system("xcopy cc_ID.txt ..\\..");
    system("xcopy \"commit massage.txt\" ..\\..");
    system("xcopy configmail.txt ..\\..");
    system("xcopy configname.txt ..\\..");
    system("xcopy date.txt ..\\..");
    system("xcopy lc_ID.txt ..\\..");
    system("xcopy \"staging area.txt\" ..\\..");
}

void clear_cwd()
{
    struct dirent *entry;
    DIR *dir=opendir(".");
    struct stat info;
    char command[1000];
    while((entry = readdir(dir)) != NULL)
    {
        stat(entry -> d_name, &info);
        if(!strcmp(entry->d_name,".") || !strcmp(entry->d_name,"..") || !strcmp(entry->d_name,"tig") || !strcmp(entry -> d_name, "tig.c") || !strcmp(entry -> d_name, "tig.exe"))
        {
            continue;
        }
        
        if(S_ISDIR(info.st_mode))
        {
            sprintf(command,"rmdir /s \"%s\"", entry -> d_name);
            system(command);
        }
        else{
           sprintf(command,"del \"%s\"", entry -> d_name);
            system(command); 
        }
    }
}

void append_to_start(const char *file_name, const char *data) {
    FILE *file = fopen(file_name, "r");
    FILE *temp_file = fopen("temp.txt", "w");

    if (file == NULL || temp_file == NULL) {
        perror("Error opening file");
        return;
    }

    fprintf(temp_file, "%s", data);

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        fprintf(temp_file, "%s", line);
    }

    fclose(file);
    fclose(temp_file);

    remove(file_name);
    rename("temp.txt", file_name);
}

int is_directory_empty()
{
    struct dirent *entry;
    DIR *dir = opendir(".");
    int is_empty = 1;
    while((entry = readdir(dir)) != NULL)
    {
        if(strcmp(entry -> d_name, ".") && strcmp(entry -> d_name, ".."))
        {
            is_empty = 0;
            break;
        }
    }
    return is_empty;
}

// char* get_relative_path(const char* full_path, const char* base_path)
// {
//     size_t len = strlen(base_path);
//     if (strncmp(full_path, base_path, len) == 0)
//     {
//         const char* relative_path = full_path + len;
//         if (relative_path[0] == '\\')
//         {
//             relative_path++;
//         }
//         return strdup(relative_path);
//     } 
// }

int is_directory_or_file(const char *file_name)
{
    struct stat path_stat;
    stat(file_name, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

int check_file_existance(const char *file_name)
{
    struct stat buffer;
    return (stat(file_name, &buffer));
}

void reach_tig()
{
    while(1)
    {
        char *current_dir = _getcwd(NULL, MAX_PATH);
        char tig_path[MAX_PATH];
        sprintf(tig_path, "%s\\tig", current_dir);

        if(GetFileAttributes(tig_path) != INVALID_FILE_ATTRIBUTES)
        {
            SetCurrentDirectory(tig_path);
            return;
        }

        SetCurrentDirectory("..");
    }
}

void add_to_staging_area(const char *file_name)
{
    char *current_dir = _getcwd(NULL, MAX_PATH);
    char full_address[MAX_PATH], full_address_cp[MAX_PATH];
    sprintf(full_address, "%s\\%s", current_dir, file_name);
    reach_tig();
    FILE *ptr = fopen("staging area.txt", "r");
    char line[MAX_PATH];
    while(fgets(line, sizeof(line), ptr) != NULL)
    {
        int len = strlen(line);

        if(len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }

        if(!strcmp(line, full_address))
        {
            update_staged_file(full_address);
            SetCurrentDirectory(current_dir);
            fclose(ptr);
            return;
        }
    }
    fclose(ptr);
    char data[MAX_PATH];
    sprintf(data, "%s\n", full_address);
    append_to_start("add history.txt", data);

    ptr = fopen("staging area.txt", "a");
    fprintf(ptr, "%s\n", full_address);
    fclose(ptr);
    add_to_staging_directory(full_address, file_name);
    printf("you succesfully added %s to staging area\n", file_name);
    SetCurrentDirectory(current_dir);
    return;
}

int dir_existance()
{
    char *main_dir = _getcwd(NULL, MAX_PATH);
    while(1)
    {
        char *current_dir = _getcwd(NULL, MAX_PATH);
        char tig_path[MAX_PATH];
        sprintf(tig_path, "%s\\tig", current_dir);

        if(GetFileAttributes(tig_path) != INVALID_FILE_ATTRIBUTES)
        {
            SetCurrentDirectory(main_dir);
            return 1;
        }

        if(strlen(current_dir) == 3)
        {
            break;
        }

        if(!SetCurrentDirectory(".."))
        {
            SetCurrentDirectory(main_dir);
            return 1;
        }
    }

    
    SetCurrentDirectory(basement);
    FILE *ptr;
    ptr = fopen("configname.txt", "r");
    fgets(confname, sizeof(confname), ptr);
    fclose(ptr);

    ptr = fopen("configmail.txt", "r");
    fgets(confmail, sizeof(confmail), ptr);
    fclose(ptr);

    SetCurrentDirectory(main_dir);
    return 0;
}

void init_()
{
    if (dir_existance())
    {
        printf("Error: tig repo is already exist\n");
        exit(EXIT_FAILURE);
    }

    if(mkdir("tig"))
    {
        printf("Error creating repo directory\n");
        exit(EXIT_FAILURE);
    }

    {
        SetCurrentDirectory("tig");
        FILE *ptr;
        ptr = fopen("configname.txt", "w");
        fprintf(ptr, "%s", confname);
        fclose(ptr);

        ptr = fopen("configmail.txt", "w");
        fprintf(ptr, "%s", confmail); 
        fclose(ptr);
        
        ptr = fopen("staging area.txt", "w");
        fclose(ptr);

        ptr = fopen("tracked files.txt", "w");
        fclose(ptr);

        ptr = fopen("lc_ID.txt", "w");
        fprintf(ptr, "%d", 0);
        fclose(ptr);

        ptr = fopen("cc_ID.txt", "w");
        fprintf(ptr, "%d", 0);
        fclose(ptr);

        ptr = fopen("index.txt", "w");
        fprintf(ptr, "%d", 0);
        fclose(ptr);

        ptr = fopen("branch.txt", "w");
        fprintf(ptr, "%s", "master");
        fclose(ptr);

        ptr = fopen("add history.txt", "w");
        fprintf(ptr, "$$$");
        fclose(ptr);

        ptr = fopen("commit history.txt", "w");
        fclose(ptr);

        ptr = fopen("tags.txt", "w");
        fclose(ptr);

        ptr = fopen("hooks.txt", "w");
        fclose(ptr);

        if(mkdir("branches"))
        {
            printf("Error creating branches directory\n");
            exit(EXIT_FAILURE);
        }

        if(mkdir("commits"))
        {
            printf("Eror creating commits directory\n");
            exit(EXIT_FAILURE);
        }

        if(mkdir("staging"))
        {
            printf("Eror creating staging directory\n");
            exit(EXIT_FAILURE);
        }

        SetCurrentDirectory("branches");
        ptr = fopen("master.txt", "w");
        fprintf(ptr, "0");
        fclose(ptr);
        SetCurrentDirectory("..\\..");
    }

    if (SetFileAttributes("tig", FILE_ATTRIBUTE_HIDDEN) == 0)
    {
        printf("Error hiding directory.\n");
        exit(EXIT_FAILURE);
    }

    printf("Initialized Tig repo succesfully\n");
    return;
}

void config_(int tedad, const char *voroodi[])
{
    if(!dir_existance())
    {
        printf("not a tig dir\n");
        exit(EXIT_FAILURE);
    }

    char *main_path = _getcwd(NULL, MAX_PATH);
    if(!strcmp(voroodi[2], "-global"))
    {
        if(tedad > 5)
        {
            printf("too much arguments\n");
            exit(EXIT_FAILURE);
        }

        if(tedad < 5)
        {
            printf("you need more arguments\n");
            exit(EXIT_FAILURE);
        }

        SetCurrentDirectory(basement);
        FILE *ptr;
        if(!strcmp(voroodi[3], "user.name"))
        {
            ptr = fopen("configname.txt", "w");
            fprintf(ptr, "%s", voroodi[4]);
            fclose(ptr);
        }

        else if(!strcmp(voroodi[3], "user.email"))
        {
            ptr = fopen("configmail.txt", "w");
            fprintf(ptr, "%s", voroodi[4]);
            fclose(ptr);
        }

        else
        {
            printf("please enter name or email you're talking about\n");
            exit(EXIT_FAILURE);
        }
        SetCurrentDirectory(main_path);
        reach_tig();

        if(!strcmp(voroodi[3], "user.name"))
        {
            ptr = fopen("configname.txt", "w");
            fprintf(ptr, "%s", voroodi[4]);
            fclose(ptr);
            return;
        }

        else if(!strcmp(voroodi[3], "user.email"))
        {
            ptr = fopen("configmail.txt", "w");
            fprintf(ptr, "%s", voroodi[4]);
            fclose(ptr);
            return;
        }
    }

    else
    {
        if(tedad > 4)
        {
            printf("too much arguments\n");
            exit(EXIT_FAILURE);
        }

        if(tedad < 4)
        {
            printf("you need more arguments\n");
            exit(EXIT_FAILURE);
        }

        reach_tig();
        FILE *ptr;

        if(!strcmp(voroodi[2], "user.name"))
        {
            ptr = fopen("configname.txt", "w");
            fprintf(ptr, "%s", voroodi[3]);
            fclose(ptr);
            return;
        }

        else if(!strcmp(voroodi[2], "user.email"))
        {
            ptr = fopen("configmail.txt", "w");
            fprintf(ptr, "%s", voroodi[3]);
            fclose(ptr);
            return;
        }

        else
        {
            printf("please enter name or email you're talking about\n");
            exit(EXIT_FAILURE);
        }
    }
}

void add_(int tedad,const char * voroodi[])
{
    char *current_dir = _getcwd(NULL, MAX_PATH);

    if(!dir_existance())
    {
        printf("please first initialize tig repo\n");
        exit(EXIT_FAILURE);
    }

    if(tedad < 3)
    {
        printf("please specify a file\n");
        exit(EXIT_FAILURE);
    }

    if(strcmp(voroodi[2], "-f") && strcmp(voroodi[2], "-n"))
    {
        for(int i = 2; i < tedad; i++)
        {
            if(!check_file_existance(voroodi[i]))
            {
                if(is_directory_or_file(voroodi[i]))
                {
                    SetCurrentDirectory(voroodi[i]);
                    add_directory_to_staging_area(100, 1);
                    SetCurrentDirectory("..");
                }

                else
                {
                    add_to_staging_area(voroodi[i]);
                }
            }

            else
            {
                printf("there is no such file or directory: %s\n", voroodi[2]);
            }
        }
    }

    else if(!strcmp(voroodi[2], "-f"))
    {
        for(int i = 3; i < tedad; i++)
        {
            if(!check_file_existance(voroodi[i]))
            {
                if(is_directory_or_file(voroodi[i]))
                {
                    SetCurrentDirectory(voroodi[i]);
                    add_directory_to_staging_area(100, 1);
                    SetCurrentDirectory("..");
                }

                else
                {
                    add_to_staging_area(voroodi[i]);
                }
            }

            else
            {
                printf("there is no such file or directory: %s\n", voroodi[i]);
            }
        }
    }

    else if(!strcmp(voroodi[2], "-n"))
    {
        char *endptr;
        int depht = strtol(voroodi[3], &endptr, 10);
        add_directory_to_staging_area(depht, 1);
    }

    reach_tig();
    append_to_start("add history.txt", "$$$\n");
    SetCurrentDirectory(current_dir);

}

void find_files_with_wildcard(const char *wildcard, void (*callback)(const char *file_name))
{
    struct _finddata_t file_info;
    intptr_t handle = _findfirst(wildcard, &file_info);
    printf("%s\n", file_info.name);

    if(handle != -1)
    {
        do
        {
            callback(file_info.name);
        } while(_findnext(handle, &file_info) == 0);
    }
    _findclose(handle);
}

void add_with_wildcard(const char *wildcard)
{
    find_files_with_wildcard(wildcard, add_to_staging_area);
    return;
}

void add_directory_to_staging_area(int depht, int level)
{
    if(depht < level)
    {
        return;
    }

    struct dirent *entry;
    DIR *dir = opendir(".");
    while((entry = readdir(dir)) != NULL)
    {
        if(entry -> d_type == DT_DIR)
        {
            if(strcmp(entry -> d_name, ".") && strcmp(entry -> d_name, "..") && strcmp(entry -> d_name, "tig"))
            {
                SetCurrentDirectory(entry -> d_name);
                add_directory_to_staging_area(depht, level + 1);
                SetCurrentDirectory("..");
            }
        }
        else
        {
            add_to_staging_area(entry -> d_name);
        }
    }
    closedir(dir);
    return;
}

void add_to_staging_directory(const char *full_address, const char *file_name)
{
    reach_tig();
    char full_address_copy[MAX_PATH];
    strcpy(full_address_copy, full_address);
    char *dir_name, branch_name[100];
    dir_name[0] = '\0';
    char *token = strtok(full_address_copy, "\\:.");
    while(token != NULL)
    {
        strcat(dir_name, token);
        token = strtok(NULL, "\\:.");
    }
    FILE *ptr = fopen("branch.txt", "r");
    fgets(branch_name, sizeof(branch_name), ptr);
    fclose(ptr);

    SetCurrentDirectory("staging");
    mkdir(dir_name);
    SetCurrentDirectory(dir_name);
    FILE *file = fopen("datas.txt", "w");
    fprintf(file, "%s\n%s\n%s\n", full_address, branch_name, file_name);
    fclose(file);

    char command[MAX_PATH];
    sprintf(command, "copy \"%s\"", full_address);
    system(command);
    SetCurrentDirectory("..");
    SetCurrentDirectory("..");
    return;
}

void update_staged_file(const char* full_address)
{
    reach_tig();
    char full_address_copy[MAX_PATH];
    strcpy(full_address_copy, full_address);
    char *dir_name, branch_name[100];
    dir_name[0] = '\0';
    char *token = strtok(full_address_copy, "\\:.");
    while(token != NULL)
    {
        strcat(dir_name, token);
        token = strtok(NULL, "\\:.");
    }
    FILE *ptr = fopen("branch.txt", "r");
    fgets(branch_name, sizeof(branch_name), ptr);
    fclose(ptr);
    SetCurrentDirectory("staging");
    if(GetFileAttributes(dir_name) != INVALID_FILE_ATTRIBUTES)
    {
        SetCurrentDirectory(dir_name);
    }
    else
    {
        mkdir(dir_name);
        SetCurrentDirectory(dir_name);
    }
    FILE *file = fopen("datas.txt", "w");
    fprintf(file, "%s\n%s\n", full_address, branch_name);
    fclose(file);

    char command[MAX_PATH];
    sprintf(command, "copy \"%s\"", full_address);
    system(command);
    SetCurrentDirectory("..");
    SetCurrentDirectory("..");
    return;
}

void reset_(int tedad, const char *voroodi[])
{
    if(!dir_existance())
    {
        printf("please first initialize tig repo\n");
        exit(EXIT_FAILURE);
    }

    if(tedad < 3)
    {
        printf("please specify a file\n");
        exit(EXIT_FAILURE);
    }

    if(!strcmp(voroodi[2], "-f"))
    {
        for(int i = 3; i < tedad; i++)
        {
            if(is_directory_or_file(voroodi[i]))
            {
                SetCurrentDirectory(voroodi[i]);
                remove_directory_from_staging_area(1, 1);
                SetCurrentDirectory("..");
            }

            else
            {
                remove_from_staging_area(voroodi[2]);
            }
        }
    }

    else if(strcmp(voroodi[2], "-undo"))
    {
        for(int i = 2; i < tedad; i++)
        {
            if(is_directory_or_file(voroodi[i]))
            {
                SetCurrentDirectory(voroodi[i]);
                remove_directory_from_staging_area(1, 1);
                SetCurrentDirectory("..");
            }

            else
            {
                remove_from_staging_area(voroodi[2]);
            }
        }
    }

    else if(!strcmp(voroodi[2], "-undo"))
    {
        undo_();
    }
}

void remove_directory_from_staging_area(int depht, int level)
{
    struct dirent *entry;
    DIR *dir = opendir(".");
    while((entry = readdir(dir)) != NULL)
    {
        if(entry -> d_type == DT_DIR)
        {
            if(strcmp(entry -> d_name, ".") && strcmp(entry -> d_name, "..") && strcmp(entry -> d_name, "tig"))
            {
                SetCurrentDirectory(entry -> d_name);
                remove_directory_from_staging_area(depht, level + 1);
                SetCurrentDirectory("..");
            }
        }
        else
        {
            remove_from_staging_area(entry -> d_name);
        }
    }
    return;
}

void remove_from_staging_area(const char *file_name)
{
    char *current_dir = _getcwd(NULL, MAX_PATH);
    char full_address[MAX_PATH];
    sprintf(full_address, "%s\\%s", current_dir, file_name);
    reach_tig();

    FILE *ptr = fopen("staging area.txt", "r");
    FILE *t = fopen("temp.txt", "w");
    char line[MAX_PATH];
    while(fgets(line, sizeof(line), ptr) != NULL)
    {
        int len = strlen(line);

        if(len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }

        if(!strcmp(line, full_address))
        {
            continue;
        }

        else
        {
            fprintf(t, "%s\n", line);
        }
    }
    fclose(ptr);
    fclose(t);
    remove("staging area.txt");
    rename("temp.txt", "staging area.txt");
    remove_from_staging_directory(full_address);
    printf("you succesfully removed %s from staging area\n", file_name);
    SetCurrentDirectory(current_dir);
    return;
}

void remove_from_staging_directory(const char *full_address)
{
    reach_tig();
    char full_address_copy[MAX_PATH];
    strcpy(full_address_copy, full_address);
    char dir_name[MAX_PATH];
    dir_name[0] = '\0';
    char *token = strtok(full_address_copy, "\\:.");
    while(token != NULL)
    {
        strcat(dir_name, token);
        token = strtok(NULL, "\\:.");
    }
    char command[MAX_PATH];
    SetCurrentDirectory("staging");
    sprintf(command, "rmdir /s /q \"%s\"", dir_name);
    system(command);
    SetCurrentDirectory("..");
    return;
}

void undo_()
{
    reach_tig();
    FILE *ptr = fopen("add history.txt", "r");
    char line[MAX_PATH];
    char line_cp[MAX_PATH];
    fscanf(ptr, "%s\n", line);
    fscanf(ptr, "%s\n", line);
    do
    {
        strcpy(line_cp,line);
        remove_from_staging_directory(line_cp);
        FILE *st = fopen("staging area.txt", "r");
        FILE *tp = fopen("temp.txt", "w");
        char l[MAX_PATH];
        while(fgets(l, sizeof(l), st) != NULL)
        {
            l[strlen(l) - 1] = '\0';
            if(!strcmp(l, line))
            {
                continue;
            }
            else
            {
                fprintf(tp, "%s\n", l);
            }
        }
        fclose(st);
        fclose(tp);
        remove("staging area.txt");
        rename("temp.txt", "staging area.txt");
        fscanf(ptr, "%s\n", line);
    } while(strcmp(line, "$$$"));
    fclose(ptr);

    ptr = fopen("add history.txt", "r");
    fscanf(ptr, "%s\n", line);
    do
    {
        fscanf(ptr, "%s\n", line);
    } while (strcmp(line, "$$$"));
    
    FILE *tp = fopen("temp.txt", "w");
    do
    {
        fprintf(tp, "%s\n", line);
    } while (fscanf(ptr, "%s\n", line) != EOF);
    fclose(tp);
    fclose(ptr);
    
    remove("add history.txt");
    rename("temp.txt", "add history.txt");
    printf("succefully undoed the last add\n");
    return;
}

void commit_(int tedad, const char *voroodi[])
{
    reach_tig();
    if(!dir_existance())
    {
        printf("please first initialize tig repo\n");
        exit(EXIT_FAILURE);
    }

    if(tedad < 4)
    {
        printf("please use the correct format");
        exit(EXIT_FAILURE);
    }

    if(tedad == 4)
    {
        if(!strcmp(voroodi[2], "-m"))
        {
            SetCurrentDirectory("staging");
            if(is_directory_empty())
            {
                printf("staging area is empty\n");
                exit(EXIT_FAILURE);
            }
            SetCurrentDirectory("..");

            if(strlen(voroodi[3]) > 72)
            {
                printf("max size of your commit's maasage is 72");
                exit(EXIT_FAILURE);
            }

            char date[100];
            time_t current_time;
            time(&current_time);
            sprintf(date, "%s", ctime(&current_time));

            int totat_commits, files_commited = 0;
            char commit_ID[MAX_PATH], command[MAX_PATH], confn[MAX_PATH], confe[MAX_PATH], branch_name[MAX_PATH], buffer[MAX_PATH], data[1000];
            char *current_dir = _getcwd(NULL, MAX_PATH);
            FILE *ptr = fopen("index.txt", "r");
            fscanf(ptr, "%d", &totat_commits);
            fclose(ptr);
            ptr = fopen("index.txt", "w");
            fprintf(ptr, "%d", totat_commits + 1);
            fclose(ptr);
            ptr = fopen("cc_ID.txt", "r");
            fscanf(ptr, "%s", commit_ID);
            fclose(ptr);
            ptr = fopen("configname.txt", "r");
            fgets(confn, sizeof(confn), ptr);
            fclose(ptr);
            ptr = fopen("configmail.txt", "r");
            fgets(confe, sizeof(confe), ptr);
            fclose(ptr);
            ptr = fopen("branch.txt", "r");
            fscanf(ptr, "%s", branch_name);
            fclose(ptr);
            ptr = fopen("staging area.txt", "r");
            while(fgets(buffer, strlen(buffer), ptr) != NULL)
            {
                if(buffer[0] == 'C')
                {
                    files_commited++;
                }
            }
            fclose(ptr);
            SetCurrentDirectory("commits");
            mkdir(commit_ID);
            SetCurrentDirectory(commit_ID);
            system("xcopy ..\\..\\staging /E");
            SetCurrentDirectory("..");
            SetCurrentDirectory("..");
            system("del \"add history.txt\"");
            ptr = fopen("add history.txt", "w");
            fclose(ptr);
            sprintf(data, "%s%s\n%s\n%s\n%s\n%d\n%s\n", date, commit_ID, confn, confe, branch_name, files_commited, voroodi[3]);
            append_to_start("commit history.txt", data);
            system("rmdir /s /q staging");
            mkdir("staging");
            SetCurrentDirectory("commits");
            SetCurrentDirectory(commit_ID);
            system("copy ..\\..\\cc_ID.txt");
            system("copy \"..\\..\\staging area.txt\"");
            system("copy ..\\..\\lc_ID.txt");
            system("copy ..\\..\\configname.txt");
            system("copy ..\\..\\configmail.txt");
            system("copy ..\\..\\branch.txt");
            ptr = fopen("commit massage.txt", "w");
            fprintf(ptr, "%s\n", voroodi[3]);
            fclose(ptr);
            ptr = fopen("date.txt", "w");
            fprintf(ptr, "%s", date);
            fclose(ptr);
            SetCurrentDirectory("..");
            SetCurrentDirectory("..");
            system("del \"staging area.txt\"");
            ptr = fopen("staging area.txt", "w");
            fclose(ptr);
            ptr = fopen("cc_ID.txt", "w");
            fprintf(ptr, "%d", totat_commits + 1);
            fclose(ptr);
            ptr = fopen("lc_ID.txt", "w");
            fprintf(ptr, "%s", commit_ID);
            printf("\x1b[35m");
            printf("date of commit: %s", date);
            printf("\x1b[36m");
            printf("commit ID: %s\ncommit massage: %s\n",commit_ID, voroodi[3]);
            printf("\x1b[0m");
            
        }

        else if(!strcmp(voroodi[2], "-s"))
        {
            char *current_dir = _getcwd(NULL, MAX_PATH);
            SetCurrentDirectory(basement);
            FILE *ptr =fopen("short-cuts.txt", "r");
            char line[80];
            char massage[80];
            int exist = 0;
            while(fgets(line, sizeof(line), ptr) != NULL)
            {
                fgets(massage, sizeof(massage), ptr);
                line[strlen(line) - 1] = '\0';
                if(!strcmp(voroodi[3], line))
                {
                    massage[strlen(massage) - 1] = '\0';
                    exist = 1;
                    break;
                }
            }
            SetCurrentDirectory(current_dir);
            if(exist)
            {
                printf("%s\n", massage);
                const char *v[] = {"tig", "commit", "-m", massage};
                commit_(4, v);
                return;
            }
            else
            {
                printf("there is no such a shortcut\n");
            }
        }
    }

    else
    {
        printf("wrong format of commit command! please write your massage between two \" if it have whitespace\n");
    }
}

void set_(int tedad, const char *voroodi[])
{
    if(!dir_existance())
    {
        printf("please first initialize tig repo\n");
        exit(EXIT_FAILURE);
    }

    if(tedad < 6 || tedad > 6)
    {
        printf("please use the correct format");
        exit(EXIT_FAILURE);
    }

    if(tedad == 6)
    {
        if(strcmp(voroodi[2], "-m") || strcmp(voroodi[4], "-s"))
        {
            printf("please use the correct format");
            exit(EXIT_FAILURE);
        }

        else
        {
            if(strlen(voroodi[3]) > 72)
            {
                printf("max size of your commit's massage is 72");
                exit(EXIT_FAILURE);
            }

            char *current_dir = _getcwd(NULL, MAX_PATH);
            SetCurrentDirectory(basement);
            FILE *ptr = fopen("short-cuts.txt", "a");
            fprintf(ptr, "%s\n%s\n", voroodi[5], voroodi[3]);
            fclose(ptr);
            SetCurrentDirectory(current_dir);
            printf("%s setted as the shortcut of the following massage: %s", voroodi[5], voroodi[3]);
            return;
        }
    }
}

void replace_(int tedad, const char *voroodi[])
{
    if(!dir_existance())
    {
        printf("please first initialize tig repo\n");
        exit(EXIT_FAILURE);
    }

    if(tedad < 6 || tedad > 6)
    {
        printf("please use the correct format");
        exit(EXIT_FAILURE);
    }

    if(tedad == 6)
    {
        if(strcmp(voroodi[2], "-m") || strcmp(voroodi[4], "-s"))
        {
            printf("please use the correct format");
            exit(EXIT_FAILURE);
        }

        else
        {
            if(strlen(voroodi[3]) > 72)
            {
                printf("max size of your commit's massage is 72");
                exit(EXIT_FAILURE);
            }

            char *current_dir = _getcwd(NULL, MAX_PATH);
            SetCurrentDirectory(basement);
            FILE *t = fopen("temp.txt", "w");
            FILE *ptr = fopen("short-cuts.txt", "r");
            char line[80];
            char massage[80];
            int exist = 0;
            while(fgets(line, sizeof(line), ptr) != NULL)
            {
                fgets(massage, sizeof(massage), ptr);
                line[strlen(line) - 1] = '\0';
                if(strcmp(voroodi[5], line))
                {
                    massage[strlen(massage) - 1] = '\0';
                    fprintf(t, "%s\n%s\n", line, massage);
                }
                else
                {
                    fprintf(t, "%s\n%s\n", line, voroodi[3]);
                    exist = 1;
                }
            }
            fclose(t);
            fclose(ptr);
            remove("short-cuts.txt");
            rename("temp.txt", "short-cuts.txt");
            if(!exist)
            {
                printf("there is no such shortcut :(");
                exit(EXIT_FAILURE);
            }
            SetCurrentDirectory(current_dir);
            printf("%s updated as the shortcut of the following massage: %s", voroodi[5], voroodi[3]);
            return;
        }
    }
}

void remove_(int tedad, const char *voroodi[])
{
    if(!dir_existance())
    {
        printf("please first initialize tig repo\n");
        exit(EXIT_FAILURE);
    }

    if(tedad < 4 || tedad > 4)
    {
        printf("please use the correct format");
        exit(EXIT_FAILURE);
    }

    if(tedad == 4)
    {
        if(strcmp(voroodi[2], "-s"))
        {
            printf("please use the correct format");
            exit(EXIT_FAILURE);
        }

        else
        {
            if(strlen(voroodi[3]) > 72)
            {
                printf("max size of your commit's massage is 72");
                exit(EXIT_FAILURE);
            }

            char *current_dir = _getcwd(NULL, MAX_PATH);
            SetCurrentDirectory(basement);
            FILE *t = fopen("temp.txt", "w");
            FILE *ptr = fopen("short-cuts.txt", "r");
            char line[80];
            char massage[80];
            int exist = 0;
            while(fgets(line, sizeof(line), ptr) != NULL)
            {
                fgets(massage, sizeof(massage), ptr);
                line[strlen(line) - 1] = '\0';
                if(strcmp(voroodi[3], line))
                {
                    massage[strlen(massage) - 1] = '\0';
                    fprintf(t, "%s\n%s\n", line, massage);
                }
                else
                {
                    exist = 1;
                }
            }
            fclose(t);
            fclose(ptr);
            remove("short-cuts.txt");
            rename("temp.txt", "short-cuts.txt");
            if(!exist)
            {
                printf("there is no such shortcut :(");
                exit(EXIT_FAILURE);
            }
            SetCurrentDirectory(current_dir);
            printf("%s removed from the shortcuts", voroodi[3]);
            return;
        }
    }
}

void log_(int tedad, const char *voroodi[])
{
    if(!dir_existance())
    {
        printf("please first initialize tig repo\n");
        exit(EXIT_FAILURE);
    }
    reach_tig();

    if(tedad == 2)
    {
        char line[100];
        FILE *ptr = fopen("commit history.txt", "r");
        while(fgets(line, sizeof(line), ptr) != NULL)
        {
            printf("%s", line);
        }
        return;
    }

    else if(tedad == 4)
    {
        if(!strcmp(voroodi[2], "-n"))
        {
            int i = 0;
            char *endptr;
            int num = strtol(voroodi[3], &endptr, 10);
            char line[100];
            FILE *ptr = fopen("commit history.txt", "r");
            while(fgets(line, sizeof(line), ptr) != NULL && i < 7 * num)
            {
                printf("%s", line);
                i++;
            }
            return;
        }

        else if(!strcmp(voroodi[2], "-branch"))
        {
            char files_commited[100], commit_massage[80];
            char commit_ID[MAX_PATH], command[MAX_PATH], confn[MAX_PATH], confe[MAX_PATH], branch_name[MAX_PATH], buffer[MAX_PATH], date[100];
            FILE *ptr = fopen("commit history.txt", "r");
            while(fgets(date, sizeof(date), ptr) != NULL)
            {
                fgets(commit_ID, sizeof(commit_ID), ptr);
                fgets(confn, sizeof(confn), ptr);
                fgets(confe, sizeof(confe), ptr);
                fgets(branch_name, sizeof(branch_name), ptr);
                fgets(files_commited, sizeof(files_commited), ptr);
                fgets(commit_massage, sizeof(commit_massage), ptr);
                branch_name[strlen(branch_name) -1] = '\0';
                if(!strcmp(branch_name, voroodi[3]))
                {
                    printf("%s%s%s%s%s\n%s%s",date, commit_ID, confn, confe, branch_name, files_commited, commit_massage);
                }
            }
            return;
        }

        else if(!strcmp(voroodi[2], "-author"))
        {
            char files_commited[100], commit_massage[80];
            char commit_ID[MAX_PATH], confn[MAX_PATH], confe[MAX_PATH], branch_name[MAX_PATH], date[100];
            FILE *ptr = fopen("commit history.txt", "r");
            while(fgets(date, sizeof(date), ptr) != NULL)
            {
                fgets(commit_ID, sizeof(commit_ID), ptr);
                fgets(confn, sizeof(confn), ptr);
                fgets(confe, sizeof(confe), ptr);
                fgets(branch_name, sizeof(branch_name), ptr);
                fgets(files_commited, sizeof(files_commited), ptr);
                fgets(commit_massage, sizeof(commit_massage), ptr);
                confn[strlen(confn) -1] = '\0';
                if(!strcmp(confn, voroodi[3]))
                {
                    printf("%s%s%s\n%s%s%s%s",date, commit_ID, confn, confe, branch_name, files_commited, commit_massage);
                }
            }
            return;
        }
    }

    else if(!strcmp(voroodi[2], "-search"))
        {
            char files_commited[100], commit_massage[80];
            char commit_ID[MAX_PATH], confn[MAX_PATH], confe[MAX_PATH], branch_name[MAX_PATH], date[100];
            FILE *ptr = fopen("commit history.txt", "r");
            while(fgets(date, sizeof(date), ptr) != NULL)
            {
                fgets(commit_ID, sizeof(commit_ID), ptr);
                fgets(confn, sizeof(confn), ptr);
                fgets(confe, sizeof(confe), ptr);
                fgets(branch_name, sizeof(branch_name), ptr);
                fgets(files_commited, sizeof(files_commited), ptr);
                fgets(commit_massage, sizeof(commit_massage), ptr);
                for(int i = 3; i < tedad; i++)
                {
                    if(strstr(commit_massage, voroodi[i]) != NULL)
                    {
                        printf("%s%s%s%s%s%s%s",date, commit_ID, confn, confe, branch_name, files_commited, commit_massage);
                        break;
                    }
                }
            }
            return;
        }
}

void status_()
{
    if(!dir_existance())
    {
        printf("please first initialize tig repo\n");
        exit(EXIT_FAILURE);
    }
    struct dirent *entry;
    DIR *dir = opendir(".");

    char X, Y;
    char *current_dir = _getcwd(NULL, MAX_PATH);
    char branch_name[80], branch_name_2[80], garbage[100], index[5];
    reach_tig();
    FILE *commit_ptr = fopen("commit history.txt", "r");
    FILE *branch_ptr = fopen("branch.txt", "r");
    fgets(branch_name, sizeof(branch_name), branch_ptr);
    while(fgets(garbage, sizeof(garbage), commit_ptr) != NULL)
    {
        fgets(index, sizeof(index), commit_ptr);
        fgets(garbage, sizeof(garbage), commit_ptr);
        fgets(garbage, sizeof(garbage), commit_ptr);
        fgets(branch_name_2, sizeof(branch_name_2), commit_ptr);
        fgets(garbage, sizeof(garbage), commit_ptr);
        fgets(garbage, sizeof(garbage), commit_ptr);
        branch_name_2[strlen(branch_name_2) - 1] = '\0';
        index[strlen(index) - 1] = '\0';
        if(!strcmp(branch_name, branch_name_2))
        {
            break;
        }
    }
    fclose(commit_ptr);
    fclose(branch_ptr);
    SetCurrentDirectory(current_dir);
    // printf("%s\n\n", current_dir);
    while((entry = readdir(dir)) != NULL)
    {
        if(strcmp(entry -> d_name, ".") == 0 || strcmp(entry -> d_name, "..") == 0 || strcmp(entry -> d_name, "tig") == 0)
        {
            continue;
        }

        else if((entry -> d_type != DT_DIR))
        {
            FILE *main_file = fopen(entry -> d_name, "r");
            reach_tig();
            char full_address[MAX_PATH], full_address_copy[MAX_PATH];
            sprintf(full_address, "%s\\%s", current_dir, entry -> d_name);
            strcpy(full_address_copy, full_address);
            SetCurrentDirectory("commits");
            char dir_name[260];
            dir_name[0] = '\0';
            char *token = strtok(full_address_copy, "\\:.");
            while(token != NULL)
            {
                strcat(dir_name, token);
                token = strtok(NULL, "\\:.");
            }
            SetCurrentDirectory(index);
            struct dirent *en;
            DIR *d = opendir(".");
            int ex = 0;
            while((en = readdir(d)) != NULL)
            {
                if(!strcmp(en -> d_name, dir_name))
                {
                    ex = 1;
                    break;
                }
            }
            closedir(d);
            if(!ex)
            {
                Y = 'A';
                goto lable1;
            }
            SetCurrentDirectory(dir_name);
            FILE *commit_file = fopen(entry -> d_name, "r");
            int same = 1;
            char line1[200], line2[200];
            while(fgets(line1, sizeof(line1), commit_file) != NULL)
            {
                if(fgets(line2, sizeof(line2), main_file) == NULL)
                {
                    same = 0;
                    break;
                }
                if(strcmp(line1, line2))
                {
                    same = 0;
                    break;
                }
            }
            if(fgets(line2, sizeof(line2), main_file) != NULL)
            {
                same = 0;
            }
            fclose(main_file);
            fclose(commit_file);
            if(same)
            {
                SetCurrentDirectory(current_dir);
                continue;
            }
            else
            {
                Y = 'M';
            }
            SetCurrentDirectory("..");
            lable1:
            SetCurrentDirectory("..");
            SetCurrentDirectory("..");
            SetCurrentDirectory("staging");
            if(GetFileAttributes(dir_name) == INVALID_FILE_ATTRIBUTES)
            {
                X = '-';
            }
            else
            {
                X = '+';
            }
            printf("%s %c%c\n", entry -> d_name, X, Y);
            SetCurrentDirectory(current_dir);
            continue;
        }
        else if(entry -> d_type == DT_DIR);
        {
            // printf("11%s\n", entry -> d_name);
            SetCurrentDirectory(entry -> d_name);
            status_();
            SetCurrentDirectory("..");
        }
    }
    closedir(dir);
    // return;
    reach_tig();
    SetCurrentDirectory("commits");
    SetCurrentDirectory(index);
    char *temp_address = _getcwd(NULL, MAX_PATH);
    dir = opendir(".");
    while ((entry = readdir(dir)) != NULL)
    {
        if((entry -> d_type == DT_DIR) && strcmp(entry -> d_name, ".") && strcmp(entry -> d_name, ".."))
        {
            SetCurrentDirectory(entry -> d_name);
            FILE *dt = fopen("datas.txt", "r");
            char address[100], garbage[100], file_name[100], address_cp[MAX_PATH], address_cp_cp[MAX_PATH];
            fscanf(dt, "%s\n", address);
            fgets(garbage, sizeof(garbage), dt);
            fscanf(dt, "%s\n", file_name);
            fclose(dt);
            strcpy(address_cp_cp, address);
            if(strcmp(current_dir, dirname(address_cp_cp)))
            {
                continue;
            }
            if(GetFileAttributes(address) == INVALID_FILE_ATTRIBUTES)
            {
                strcpy(address_cp, address);
                char dir_name[MAX_PATH];
                dir_name[0] = '\0';
                char *token = strtok(address_cp, "\\:.");
                while(token != NULL)
                {
                    strcat(dir_name, token);
                    token = strtok(NULL, "\\:.");
                }
                reach_tig();
                SetCurrentDirectory("staging");
                if(GetFileAttributes(dir_name) != INVALID_FILE_ATTRIBUTES)
                {
                    printf("%s +D\n", file_name);
                }
                else
                {
                    printf("%s -D\n", file_name);
                }
            }
            fclose(dt);
            SetCurrentDirectory(temp_address);
        }
    }
    closedir(dir);
    SetCurrentDirectory(current_dir);
}

void branch_(int tedad, const char *voroodi[])
{
    if(!dir_existance())
    {
        printf("please first initialize tig repo\n");
        exit(EXIT_FAILURE);
    }

    if(tedad > 3)
    {
        printf("please use the correct format\n");
        exit(EXIT_FAILURE);
    }

    reach_tig();
    if(tedad == 3)
    {
        FILE *ptr = fopen("commit history.txt", "r");
        char garbage[200], commit_ID[10];
        fgets(garbage, sizeof(garbage), ptr);
        fgets(commit_ID, sizeof(commit_ID), ptr);
        fclose(ptr);
        SetCurrentDirectory("branches");
        char branch_name[200];
        sprintf(branch_name, "%s.txt", voroodi[2]);
        if(GetFileAttributes(branch_name) != INVALID_FILE_ATTRIBUTES)
        {
            printf("thare is already a branch with the given name\n");
            exit(EXIT_FAILURE);
        }
        ptr = fopen(branch_name, "w");
        fprintf(ptr, "%s", commit_ID);
        fclose(ptr);
        printf("%s branch has been made succesfully", voroodi[2]);
        SetCurrentDirectory("..\\commits");
        SetCurrentDirectory(commit_ID);
        ptr = fopen("branch.txt", "a");
        fprintf(ptr, "\n%s", voroodi[2]);
        fclose(ptr);
    }
    else
    {
        SetCurrentDirectory("branches");
        struct dirent *entry;
        DIR *dir = opendir(".");

        while((entry = readdir(dir)) != NULL)
        {
            if(entry -> d_type != DT_DIR)
            {
                printf("%s\n", entry -> d_name);
            }
        }
        closedir(dir);
    }
}

void checkout_(int tedad, const char *voroodi[])
{
    if(!dir_existance())
    {
        printf("please first initialize tig repo\n");
        exit(EXIT_FAILURE);
    }
    reach_tig();
    SetCurrentDirectory("..");
    if(check_uncommited_change())
    {
        printf("please first commit every changes on your project\n");
        exit(EXIT_FAILURE);
    }
    reach_tig();
    SetCurrentDirectory("tig");

    if(!strcmp(voroodi[2], "HEAD"))
    {
        char branch_name[100];
        FILE *ptr_branch_name = fopen("branch.txt", "r");
        fgets(branch_name, sizeof(branch_name), ptr_branch_name);
        fclose(ptr_branch_name);
        int exist = 0;
        char files_commited[100], commit_massage[80];
        char commit_ID[MAX_PATH], confn[MAX_PATH], confe[MAX_PATH], branch_name_f[MAX_PATH], date[100];
        FILE *ptr_commit_history = fopen("commit history.txt", "r");
        while(fgets(date, sizeof(date), ptr_commit_history) != NULL)
        {
            fgets(commit_ID, sizeof(commit_ID), ptr_commit_history);
            fgets(confn, sizeof(confn), ptr_commit_history);
            fgets(confe, sizeof(confe), ptr_commit_history);
            fgets(branch_name_f, sizeof(branch_name_f), ptr_commit_history);
            fgets(files_commited, sizeof(files_commited), ptr_commit_history);
            fgets(commit_massage, sizeof(commit_massage), ptr_commit_history);
            branch_name_f[strlen(branch_name_f) -1] = '\0';
            commit_ID[strlen(commit_ID) -1] = '\0';
            if(!strcmp(branch_name_f, branch_name))
            {
                exist = 1;
                break;
            }
        }
        checkout_with_ID(commit_ID);
    }

    else if(!strcmp(voroodi[2], "-b"))
    {
        char branch_name[100];
        strcpy(branch_name, voroodi[3]);
        int exist = 0;
        char files_commited[100], commit_massage[80];
        char commit_ID[MAX_PATH], confn[MAX_PATH], confe[MAX_PATH], branch_name_f[MAX_PATH], date[100];
        FILE *ptr_commit_history = fopen("commit history.txt", "r");
        while(fgets(date, sizeof(date), ptr_commit_history) != NULL)
        {
            fgets(commit_ID, sizeof(commit_ID), ptr_commit_history);
            fgets(confn, sizeof(confn), ptr_commit_history);
            fgets(confe, sizeof(confe), ptr_commit_history);
            fgets(branch_name_f, sizeof(branch_name_f), ptr_commit_history);
            fgets(files_commited, sizeof(files_commited), ptr_commit_history);
            fgets(commit_massage, sizeof(commit_massage), ptr_commit_history);
            branch_name_f[strlen(branch_name_f) -1] = '\0';
            commit_ID[strlen(commit_ID) -1] = '\0';
            if(!strcmp(branch_name_f, branch_name))
            {
                exist = 1;
                break;
            }
        }

        if(!exist)
        {
            char line[100];
            sprintf(line, "branches\\%s.txt", branch_name);
            FILE *ptr = fopen(line, "r");
            fgets(commit_ID, sizeof(commit_ID), ptr);
            fclose(ptr);
            commit_ID[strlen(commit_ID) - 1] = '\0';
        }
        checkout_with_ID(commit_ID);
    }

    else if(!strcmp(voroodi[2], "-id"))
    {
        char commit_ID[10];
        strcpy(commit_ID, voroodi[3]);
        checkout_with_ID(commit_ID);
    }

    else if(!strncmp(voroodi[2], "HEAD-", 5))
    {
        char t[10];
        strcpy(t, voroodi[2]);
        char *token = strtok(t, "-");
        token = strtok(NULL, "-");
        char *endptr;
        int n = strtol(token, &endptr, 10);
        char branch_name[100];
        FILE *ptr_branch_name = fopen("branch.txt", "r");
        fgets(branch_name, sizeof(branch_name), ptr_branch_name);
        fclose(ptr_branch_name);
        char files_commited[100], commit_massage[80];
        char commit_ID[MAX_PATH], confn[MAX_PATH], confe[MAX_PATH], branch_name_f[MAX_PATH], date[100];
        FILE *ptr_commit_history = fopen("commit history.txt", "r");
        while (n)
        {
            while(fgets(date, sizeof(date), ptr_commit_history) != NULL)
            {
                fgets(commit_ID, sizeof(commit_ID), ptr_commit_history);
                fgets(confn, sizeof(confn), ptr_commit_history);
                fgets(confe, sizeof(confe), ptr_commit_history);
                fgets(branch_name_f, sizeof(branch_name_f), ptr_commit_history);
                fgets(files_commited, sizeof(files_commited), ptr_commit_history);
                fgets(commit_massage, sizeof(commit_massage), ptr_commit_history);
                branch_name_f[strlen(branch_name_f) -1] = '\0';
                commit_ID[strlen(commit_ID) -1] = '\0';
                if(!strcmp(branch_name_f, branch_name))
                {
                    n--;
                    break;
                }
            }
        }
        checkout_with_ID(commit_ID);
    }
}

void status_all()
{
    if(!dir_existance())
    {
        printf("please first initialize tig repo\n");
        exit(EXIT_FAILURE);
    }
    reach_tig();
    SetCurrentDirectory("..");
    status_();
}

void tag_(int tedad, const char *voroodi[])
{
    if(!dir_existance())
    {
        printf("please first initialize tig repo\n");
        exit(EXIT_FAILURE);
    }
    reach_tig();

    if(tedad == 2)
    {
        tag_sort_and_print();
        return;
    }

    if(!strcmp(voroodi[2], "show"))
    {
        show_tag(voroodi[3]);
        return;
    }

    char config_name[100], config_mail[100];
    FILE *ptr = fopen("configname.txt", "r");
    fgets(config_name, sizeof(config_name), ptr);
    fclose(ptr);

    ptr = fopen("configmail.txt", "r");
    fgets(config_mail, sizeof(config_mail), ptr);
    fclose(ptr);

    char tag_name[100];
    strcpy(tag_name, voroodi[3]);
    char tag_massage[100] = "";
    char commit_ID[100] = "";
    int flag = 0;
    char s[100];

    for (int i = 4; i < tedad; i++)
    {
        if(!strcmp(voroodi[i], "-m"))
        {
            strcpy(tag_massage, voroodi[i + 1]);
            i++;
        }
        else if(!strcmp(voroodi[i], "-c"))
        {
            strcpy(commit_ID, voroodi[ i + 1]);
            strcpy(commit_ID, voroodi[i + 1]);
            i++;
        }
        else if(!strcmp(voroodi[i], "-f"))
        {
            flag = 1;
        }
    }

    if(strlen(commit_ID) == 0)
    {
        FILE *ptr = fopen("cc_ID.txt", "r");
        fgets(s, sizeof(s), ptr);
        strcpy(commit_ID, s);
        fclose(ptr);
    }

    if(!flag)
    {
        char t_name[100], t_massage[100], c_ID[10], c_name[100], date[100], c_mail[100];
        FILE *ptr = fopen("tags.txt", "r");
        while (fgets(t_name, sizeof(t_name), ptr) != NULL)
        {
            fgets(t_massage, sizeof(t_massage), ptr);
            fgets(c_ID, sizeof(c_ID), ptr);
            fgets(c_name, sizeof(c_name), ptr);
            fgets(c_mail, sizeof(c_mail), ptr);
            fgets(date, sizeof(date), ptr);
            t_name[strlen(t_name) - 1] = '\0';
            if(!strcmp(t_name, tag_name))
            {
                printf("there is a tag with the given name before please use -f method to overwrite on it\n");
                exit(EXIT_FAILURE);
            }
        }
        fclose(ptr);
        char date_[100];
        time_t current_time;
        time(&current_time);
        sprintf(date_, "%s", ctime(&current_time));
        ptr = fopen("tags.txt", "a");
        fprintf(ptr, "%s\n%s\n%s\n%s\n%s\n%s", tag_name, tag_massage, commit_ID, config_name, config_mail, date_);
        printf("your tag added succesfully\n");
        return;
    }

    else
    {
        int exist = 0;
        char t_name[100], t_massage[100], c_ID[10], c_name[100], date[100], c_mail[100];
        FILE *ptr = fopen("tags.txt", "r");
        FILE *t = fopen("temp.txt", "w");
        while (fgets(t_name, sizeof(t_name), ptr) != NULL)
        {
            fgets(t_massage, sizeof(t_massage), ptr);
            fgets(c_ID, sizeof(c_ID), ptr);
            fgets(c_name, sizeof(c_name), ptr);
            fgets(c_mail, sizeof(c_mail), ptr);
            fgets(date, sizeof(date), ptr);
            t_name[strlen(t_name) - 1] = '\0';
            if(strcmp(t_name, tag_name))
            {
                fprintf(t, "%s\n%s%s%s%s%s", t_name, t_massage, c_ID, c_name, date);
            }
            else
            {
                exist = 1;
                char date_[100];
                time_t current_time;
                time(&current_time);
                sprintf(date_, "%s", ctime(&current_time));
                fprintf(t, "%s\n%s\n%s\n%s\n%s\n%s", tag_name, tag_massage, commit_ID, config_name, config_mail, date_);
            }
        }
        if(!exist)
        {
            char date_[100];
            time_t current_time;
            time(&current_time);
            sprintf(date_, "%s", ctime(&current_time));
            fprintf(t, "%s\n%s\n%s\n%s\n%s\n%s", tag_name, tag_massage, commit_ID, config_name, config_mail, date_);
        }
        fclose(ptr);
        fclose(t);
        remove("tags.txt");
        rename("temp.txt", "tags.txt");
        printf("your tag added succesfully\n");
        return;
    }
}

void grep_(int tedad, const char *voroodi[])
{
    if(!dir_existance())
    {
        printf("please first initialize tig repo\n");
        exit(EXIT_FAILURE);
    }

    char file_name[MAX_PATH], word[50], commit_ID[100] = "";
    int flag = 0;

    strcpy(file_name, voroodi[3]);
    strcpy(word, voroodi[5]);

    FILE *ptr;
    for(int i = 6; i < tedad; i++)
    {
        if(!strcmp(voroodi[i], "-c"))
        {
            strcpy(commit_ID, voroodi[i + 1]);
            i++;
        }
        else if(!strcmp(voroodi[i], "-n"))
        {
            flag = 1;
        }
    }
    if(strlen(commit_ID) == 0)
    {
        ptr = fopen(file_name, "r");
    }

    else
    {
        char *full_address;
        sprintf(full_address, "%s\\%s", _getcwd(NULL, MAX_PATH), file_name);
        char dir_name[260];
        dir_name[0] = '\0';
        char *token = strtok(full_address, "\\:.");
        while(token != NULL)
        {
            strcat(dir_name, token);
            token = strtok(NULL, "\\:.");
        }
        reach_tig();
        SetCurrentDirectory("commits");
        SetCurrentDirectory(commit_ID);
        SetCurrentDirectory(dir_name);
        ptr = fopen(file_name, "r");
    }
    find_and_highlight(ptr, word, flag);
    fclose(ptr);
}

void diff_(int tedad, const char *voroodi[])
{
    if(!dir_existance())
    {
        printf("please first initialize tig repo\n");
        exit(EXIT_FAILURE);
    }

    if(!strcmp(voroodi[2], "-f"))
    {
        char full_address1[MAX_PATH], full_address2[MAX_PATH];
        sprintf(full_address1, "%s\\%s", _getcwd(NULL, MAX_PATH), voroodi[3]);
        sprintf(full_address2, "%s\\%s", _getcwd(NULL, MAX_PATH), voroodi[4]);
        if(tedad > 5)
        {
            int start1, start2, end1, end2;
            char buffer[10];
            strcpy(buffer, voroodi[6]);
            char *token = strtok(buffer, "-");
            start1 = atoi(token);
            token = strtok(NULL, "-");
            end1 = atoi(token);
            strcpy(buffer, voroodi[8]);
            token = strtok(buffer, "-");
            start2 = atoi(token);
            token = strtok(NULL, "-");
            end2 = atoi(token);
            linear_diff(full_address1, full_address2, start1, start2, end1, end2);
        }
        else
        {
            full_diff(full_address1, full_address2);
            return;
        }
    }
}

// void revert_(int tedad, const char *voroodi[])
// {
//     if(!dir_existance())
//     {
//         printf("please first initialize tig repo\n");
//         exit(EXIT_FAILURE);
//     }
//     reach_tig();
//     int m_flag = 0, e_flag = 0;
//     for (int i = 0; i < tedad; i++)
//     {
//         if(!strcmp(voroodi[i], "-m"))
//         {
//             m_flag = 1;
//         }
//     }
// }

void precommit_(int tedad, const char *voroodi[])
{
    if(!dir_existance())
    {
        printf("please first initialize tig repo\n");
        exit(EXIT_FAILURE);
    }
    reach_tig();

    if(tedad == 2)
    {
        SetCurrentDirectory("staging");
        struct dirent *entry;
        DIR *dir = opendir(".");
        while ((entry = readdir(dir)) != NULL)
        {
            if(!strcmp(entry -> d_name, ".") || !strcmp(entry -> d_name, "..") || entry -> d_type != DT_DIR)
            {
                continue;
            }
            SetCurrentDirectory(entry -> d_name);
            FILE *ptr = fopen("..\\..\\hooks.txt", "r");
            char line[50];
            fgets(line, sizeof(line), ptr);
            while (fgets(line, sizeof(line), ptr))
            {
                line[strlen(line) - 1] = '\0';
                pre_commit_given_order(line);
            }
            fclose(ptr);
            SetCurrentDirectory("..");
        }
        return;
    }

    if(!strcmp(voroodi[2], add))
    {
        FILE *ptr = fopen("hooks.txt", "a");
        fprintf("\n%s", voroodi[4]);
        fclose(ptr);
        return;
    }

    else if(!strcmp(voroodi[2], "remove"))
    {
        FILE *ptr = fopen("hooks.txt", "r");
        FILE *t = fopen("temp.txt", "w");
        char line[50];
        while (fgets(line, sizeof(line), ptr) != NULL)
        {
            line[strlen(line) - 1] = '\0';
            if(strcmp(line, voroodi[4]))
            {
                fprintf(t, "\n%s", line);
            }
        }
        fclose(ptr);
        fclose(t);
        remove("hooks.txt");
        rename("temp.txt", "hooks.txt");
        return;
    }

    else if (!strcmp(voroodi[2], "applied"))
    {
        FILE *ptr = fopen("hooks.txt", "r");
        char line[50];
        while (fgets(line, sizeof(line), ptr) != NULL)
        {
            printf("%s", line);
        }
        fclose(ptr);
        return;
    }
    
    else if(!strcmp(voroodi[2], "hooks"))
    {
        SetCurrentDirectory(basement);
        FILE *ptr = fopen("hook-list.txt", "r");
        char line[50];
        fgets(line, sizeof(line), ptr);
        while (fgets(line, sizeof(line), ptr) != NULL)
        {
            printf("%s", line);
        }
        return;
    }
}

int main(int argc,const char **argv)
{
    if(!strcmp(argv[1], init))
    {
        if(argc > 2)
        {
            perror("init command need to only have 2 inputs\n");
            return 1;
        }
        init_();
        return 0;
    }

    else if(!strcmp(argv[1], config))
    {
        config_(argc, argv);
        return 0;
    }

    else if(!strcmp(argv[1], add))
    {
        add_(argc, argv);
        return 0;
    }

    else if(!strcmp(argv[1], reset))
    {
        reset_(argc, argv);
        return 0;
    }

    else if(!strcmp(argv[1], status))
    {
        if(argc == 3)
        {
            status_all();
            return 0;
        }
        status_();
        return 0;
    }

    else if(!strcmp(argv[1], commit))
    {
        commit_(argc, argv);
        return 0;
    }

    else if(!strcmp(argv[1], set))
    {
        set_(argc, argv);
        return 0;
    }

    else if(!strcmp(argv[1], replace))
    {
        replace_(argc, argv);
        return 0;
    }
    
    else if(!strcmp(argv[1], "remove"))
    {
        remove_(argc, argv);
        return 0;
    }

    else if(!strcmp(argv[1], log))
    {
        log_(argc, argv);
        return 0;
    }

    else if(!strcmp(argv[1], branch))
    {
        branch_(argc, argv);
        return 0;
    }

    else if(!strcmp(argv[1], checkout))
    {
        checkout_(argc, argv);
        return 0;
    }
    
    else if(!strcmp(argv[1], revert))
    {
        // revert_(argc, argv);
        return 0;
    }

    else if(!strcmp(argv[1], tag))
    {
        tag_(argc, argv);
        return 0;
    }

    else if(!strcmp(argv[1], grep))
    {
        grep_(argc, argv);
        return 0;
    }

    else if(!strcmp(argv[1], diff))
    {
        diff_(argc, argv);
        return 0;
    }

    else if(!strcmp(argv[1], precommit))
    {
        precommit_(argc, argv);
        return 0;
    }

    else if(!strcmp(argv[1], merge))
    {
    }

    printf("invalid command\n");
    return 1;
}


//i have to return and edit add_ a litle bit