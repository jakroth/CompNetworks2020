#include "ctap.h"

TESTS {
const char *t = "Tonsley";
char *r = reverse(t);

//test 1
ok(string_length(t) == string_length(r), "Strings are the same size");

//test 2
is("yelsnoT",r,"Strings match");

free(r);
}