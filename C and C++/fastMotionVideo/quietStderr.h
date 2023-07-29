#include <stdio.h>  

/*
 * Class: CQuietStderr
 *      
 *      Certain libaries will output 'normative text' to stderr, which can
 *      be very distracting for console based applications.  In particualar,
 *      the video compression libraries have this property.  Declaring an
 *      object of this type, will temprorarily suppress stderr output.

 *
 *      Use carefully.
 */

class CQuietStderr
{
public:
    CQuietStderr()
    {
        FILE* newStream;
        freopen_s (&newStream,"/dev/null", "a", stderr);
    }

    ~CQuietStderr()
    {
        FILE* newStream;
        freopen_s (&newStream,"/dev/stdout", "a", stderr);
    }
};