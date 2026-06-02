#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 0;
    }

    int val = atoi(argv[1] + 4);
    if (val == 423) { // 0x1a7
        char *path = strdup("/bin/sh"); // 0x80c5348
        
        int zero = 0;
        gid_t egid = getegid();
        uid_t euid = geteuid();

        setresgid(egid, egid, egid);
        setresuid(euid, euid, euid);

        char *args[] = {path, NULL};
        execv(path, args);
    } else {
        // This part is not necessary to solve since we just need to bypass the atoi check
    }

    return 0;
}