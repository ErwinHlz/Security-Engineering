//
// Created by hanan on 14.05.25.
//
#include <stdio.h>      //standard input ouput
#include <sys/stat.h>   //for getting file information using stat()
#include <time.h>       //for getting time fuctions and structure
#include <fcntl.h>      //For file control options,
#include <pwd.h>        //For user info functions like getpwuid()


void printTime(time_t t) {                //Takes time_t value and prints it in human readable form
    char* timeStr = ctime(&t);            //
    printf("%s", timeStr);
}

// Bestimmt den Dateityp aus dem Modusfeld (fileStat.st_mode)
const char* getFileType(struct stat fileStat) {
    switch (fileStat.st_mode & S_IFMT) {
        case S_IFREG: return "Reguläre Datei";       // Diese sind alle in der Datei /usr/include/bits/stat.h definiert
        case S_IFDIR: return "Verzeichnis";
        case S_IFLNK: return "Symbolischer Link";
        case S_IFCHR: return "Character Device";
        case S_IFBLK: return "Block Device";
        case S_IFIFO: return "Pipe/FIFO";
        case S_IFSOCK: return "Socket";
        default: return "Unbekannt";
    }
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("Benutzung: %s <Dateipfad>\n", argv[0]);
        return 1;
    }

    const char* filePath = argv[1];
    struct stat fileStat;

    // Versuche, Informationen zur Datei zu lesen
    if (stat(filePath, &fileStat) == -1) {
        perror("Fehler beim Lesen der Datei");
        return 1;
    }
    printf("Datei wurde erfolgreich gefunden!\n");

    // Benutzername des Besitzers herausfinden
    struct passwd* pw = getpwuid(fileStat.st_uid);
    if (pw) {
        printf("Benutzername: %s\n", pw->pw_name);
    } else {
        printf("Benutzername konnte nicht gefunden werden.\n");
    }

    // Weitere Dateiinformationen ausgeben

    printf("Dateigröße: %lld Bytes\n", (long long)fileStat.st_size);
    printf("User-ID: %d\n", fileStat.st_uid);
    printf("Gruppen-ID: %d\n", fileStat.st_gid);
    printf("Dateityp: %s\n", getFileType(fileStat));
    printf("Zugriffsrechte (Oktal): %04o\n", fileStat.st_mode & 07777); // Rechte im Oktal-Format

    printf("Letzter Zugriff: ");
    printTime(fileStat.st_atime);
    printf("Letzte Änderung (Inhalt): ");
    printTime(fileStat.st_mtime);
    printf("Letzte Änderung (Metadaten/Inode): ");
    printTime(fileStat.st_ctime);

    return 0;
}