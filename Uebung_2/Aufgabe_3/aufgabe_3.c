//
// Created by erwin on 14.05.2025.
//
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>

// Gibt ein lesbares Zeitformat aus
void printTime(time_t t) {
    char* timeStr = ctime(&t);
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

    if (argc <  2) {
        printf("Benutzung: %s <Dateipfad> [Dateipfad2] [..] \n", argv[0]);
        return 1;
    }

  for (int i = 1; i < argc; i++) {
    const char* filePath = argv[i];
    struct stat fileStat;

      printf("\n==== Datei: %s ====\n", filePath);
      printf("\n");

    // Versuche, Informationen zur Datei zu lesen
    if (stat(filePath, &fileStat) == -1) {
        perror("Fehler beim Lesen der Datei");
        continue;
    } else {
        printf("Datei wurde erfolgreich gefunden!\n\n");
    }

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

      printf("----------------------------------------------");
      printf("\n");

  }



    return 0;
}