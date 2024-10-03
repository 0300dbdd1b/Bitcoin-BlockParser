#include "utils.h"
#include "platform.h"
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>

int WildcardMatch(const char *str, const char *pattern);

int IsDirectory(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
}


void SanitizeDirString(char *dir)
{
    size_t len = strlen(dir);
    if (len > 0 && dir[len - 1] != (char)PATH_SEPARATOR)
	{
        dir[len] = PATH_SEPARATOR;
        dir[len + 1] = '\0';
    }
}



FileList ListFiles(const char *directory, const char *pattern, uint16_t allocationSize)
{
    FileList fileList;
    fileList.files = (FileInfo *)malloc(sizeof(FileInfo) * allocationSize);
    fileList.count = 0;
    fileList.totalSize = allocationSize;

    struct dirent *entry;
    DIR *dp = opendir(directory);

    if (dp == NULL)
    {
        perror("opendir");
        return (FileList){NULL, 0, 0};
    }

    while ((entry = readdir(dp)) != NULL)
    {
        if (WildcardMatch(entry->d_name, pattern))
        {
            if (fileList.count >= fileList.totalSize)
            {
                fileList.files = (FileInfo *)realloc(fileList.files, sizeof(FileInfo) * (fileList.totalSize * 2));
                fileList.totalSize *= 2; // Efficient memory resizing
            }

            // Allocate memory for the filepath and build the full path
            fileList.files[fileList.count].filepath = (char *)malloc(sizeof(char) * (strlen(directory) + strlen(entry->d_name) + 2)); // +1 for '/' and +1 for '\0'
            strcpy(fileList.files[fileList.count].filepath, directory);
            strcat(fileList.files[fileList.count].filepath, "/");
            strcat(fileList.files[fileList.count].filepath, entry->d_name);

            // Mark the file as not opened
            fileList.files[fileList.count].isOpen = 0;
            fileList.files[fileList.count].file = NULL; // File pointer remains NULL until the file is opened later

            fileList.count++;
        }
    }

    closedir(dp);
    return fileList;
}

