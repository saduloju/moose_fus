s/^#undef  *\([ABCDEFGHIJKLMNOPQRSTUVWXYZ_]\)/#undef MOOSE_\1/
s/^#undef  *\([abcdefghijklmnopqrstuvwxyz]\)/#undef _moose_\1/
s/^#define  *\([ABCDEFGHIJKLMNOPQRSTUVWXYZ_][abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_]*\)\(.*\)/#ifndef MOOSE_\1\
#define MOOSE_\1\2\
#endif/
s/^#define  *\([abcdefghijklmnopqrstuvwxyz][abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_]*\)\(.*\)/#ifndef _moose_\1\
#define _moose_\1\2\
#endif/
