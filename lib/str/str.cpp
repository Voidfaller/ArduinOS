#include <str.h>
//helper function to get the length of a string
int strlength(const char *str)
{
    int length = 0;
    while (str[length] != '\0')
    {
        length++;
    }
    return length;
}

bool strcompare(const char *a, const char *b)
{
    while (*a && *b)
    {
        if (*a != *b)
            return false;
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

char *strcopy(char *dest, const char *src)
{
    char *originalDest = dest; // Store the original destination pointer
    while (*src)
    {
        *dest++ = *src++;
    }
    *dest = '\0'; // Null-terminate the destination string
    return originalDest; // Return the original destination pointer
}