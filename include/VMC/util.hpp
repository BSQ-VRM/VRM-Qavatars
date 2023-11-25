#pragma once

class IntUtil
{
public:
    //Safe int conversion function, returns -1 on empty, skips non numerical chars
    static int to_int(char const *s)
    {
        if ( s == NULL || *s == '\0' )
            return -1;

        bool negate = (s[0] == '-');
        if ( *s == '+' || *s == '-' )
            ++s;

        if ( *s == '\0')
            return -1;

        if ( *s == ' ')
            return -1;

        int result = 0;
        while(*s)
        {
            if ( *s < '0' || *s > '9' )
            {
                ++s;
                continue;
            }
            result = result * 10  - (*s - '0');  //assume negative number
            ++s;
        }
        return negate ? result : -result; //-result is positive!
    }
};