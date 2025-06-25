#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define N_DATA 2000000
#define N_SHARED 2000

/**
 * Erzeugt eine P-Operation (wait) für eine Semaphore.
 * @param semnum Index der Semaphore im Set, auf die gewartet werden soll.
 * @return struct sembuf Struktur für semop(), die eine -1 Operation auf semnum beschreibt.
 */
struct sembuf P(int semnum) {
    struct sembuf op = {semnum, -1, 0};
    return op;
}

/**
 * Erzeugt eine V-Operation (signal) für eine Semaphore.
 * @param semnum Index der Semaphore im Set, die freigegeben werden soll.
 * @return struct sembuf Struktur für semop(), die eine +1 Operation auf semnum beschreibt.
        struct sembuf {
            int sem_num; // Index der Semaphore im Set
            short sem_op; // Operation: -1 für P (wait), +1 für V (signal)
            short sem_flg; // Flags, hier 0 für Standardverhalten
        };
 */
struct sembuf V(int semnum) {
    struct sembuf op = {semnum, +1, 0};
    return op;
}

int main() {
    int shm_id;         // ID des Shared-Memory-Segments
    int sem_id;         // ID des Semaphor-Sets
    int *shared_data;   // Zeiger auf den Shared-Memory-Bereich, shmat gibt zeiger auf den Anfang des Shared-Memory-Segments zurück

    // Schlüssel für Shared Memory und Semaphoren generieren
    key_t shm_key = ftok("shmfile", 1); // Shared-Memory-Key // ftok erzeugt einen eindeutigen Schlüssel basierend auf einer Datei und einem Projekt-ID
    key_t sem_key = ftok("semfile", 2); // Semaphoren-Key
    key_t shm_key2 = ftok("shfile", 3); // ungültiger schlüssel, um zu zeigen, dass ftok auch fehlschlagen kann

    if (shm_key == -1 ) {
        perror("shm_key error");
        exit(1);
    } else if (sem_key == -1) {
        perror("sem_key error");
        exit(1);
    } else {
        printf("Schlüssel für Shared Memory: %d, Schlüssel für Semaphoren: %d\n", shm_key, sem_key);

    }

    // Shared Memory Segment anlegen
    /* int shmget(key_t key, size_t size, int shmflg); key = Schlüssel, size = Größe in Bytes,
       shmflg = Flags, hier zugriffsrechte=0666 (lesen und schreiben) | IPC_CREAT (erzeugt Segment, falls nicht vorhanden;
        falls kein key vorhanden, wird hier trotzdem ein Segment erzeugt, ohne IPC_Creat würde es dann fehlschlagen)
     */
    shm_id = shmget(shm_key, N_SHARED * sizeof(int), 0666 | IPC_CREAT);
    if (shm_id < 0) {
        perror("shmget");
        exit(1);
    } else {
        printf("Shared Memory Segment ID: %d\n", shm_id); // der Kernel gibt eine eindeutige ID für das Shared Memory Segment zurück, diese inkrementiert immer +1, nicht zurücksetzbar

    }

    /* Zwei Semaphoren anlegen: S1 (Lesen), S2 (Schreiben)
         int semget(key_t key, int nsems, int semflg); key = Schlüssel, nsems = Anzahl der Semaphoren im Set,
         semflg = Flags, hier zugriffsrechte=0666 (lesen und schreiben) | IPC_CREAT (erzeugt Set, falls nicht vorhanden)
     */
    sem_id = semget(sem_key, 2, 0666 | IPC_CREAT);
    if (sem_id < 0) {
        perror("semget");
        exit(1);
    } else {
        printf("Semaphoren Set ID: %d\n", sem_id); // der Kernel gibt eine eindeutige ID für das Semaphoren Set zurück

    }

    // Initialwerte der Semaphoren setzen
    /* int semctl(int semid, int semnum, int cmd, ...); semid = ID des Semaphoren-Sets, semnum = Index der Semaphore im Set,
       cmd = Kommando, hier SETVAL (setzt den Wert der Semaphore), ... = optionaler Wert (hier 0 für S1 und 1 für S2;
        0 bedeutet, dass S1 initial blockiert ist, S2 initial nicht blockiert ist, 1 freier platz)
     */
    semctl(sem_id, 0, SETVAL, 0);
    semctl(sem_id, 1, SETVAL, 1);

    // Zufallsdaten erzeugen
    srand48(time(NULL));
    int data[N_DATA];
    for (int i = 0; i < N_DATA; i++) {
        data[i] = lrand48();
    }
    printf("Erzeugte Zufallsdaten(ersten 10):\n");
    for (int i = 0; i < 10; i++) {
        printf("%d ", data[i]); // Ausgabe der ersten 10 Zufallszahlen
    }
    printf("\n");


    pid_t pid = fork(); // Kindprozess erzeugen;

    // pid < 0 bedeutet, dass der Fork fehlgeschlagen ist
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    // pid > 0 bedeutet, dass wir im Elternprozess sind (Erzeugerprozess)
    if (pid > 0) {
        /**
         * Überträgt die Daten in Blöcken von N_SHARED in den Shared Memory.
         * Synchronisiert mit dem Verbraucherprozess über Semaphoren.
         * (int *)shmat(int shmid, const void *shmaddr, int shmflg);
            * shmid = ID des Shared-Memory-Segments, shmaddr = Adresse, an die das Segment angehängt werden soll (NULL = beliebige Adresse),
            * shmflg = Flags (0 = Standardverhalten, d.h. Lese- und Schreibzugriff)
         */
        shared_data = (int *)shmat(shm_id, NULL, 0);
        if (shared_data == (void *)-1) {
            perror("shmat producer");
            exit(1);
        }

        int written = 0; // Anzahl der bereits geschriebenen Daten
        while (written < N_DATA) {
            // Warte auf Freigabe zum Schreiben (S2)
            struct sembuf p2 = P(1);
            /* int semop(int semid, struct sembuf *sops, size_t nsops);
                * semid = ID des Semaphoren-Sets, sops = zeiger auf Array von semop-Operationen, nsops = Anzahl der Operationen
                * Diese Funktion blockiert, bis die Semaphore verfügbar ist (d.h. der Wert > 0 ist)
             */
            semop(sem_id, &p2, 1);

            int count = (N_DATA - written > N_SHARED) ? N_SHARED : (N_DATA - written);
            for (int i = 0; i < count; i++) {
                shared_data[i] = data[written + i];
            }
            written += count;

            // Lesefreigabe setzen (S1)
            /* P-Operation: verringert den Wert der Semaphore um 1, blockiert, wenn der Wert <= 0 ist
               V-Operation: erhöht den Wert der Semaphore um 1, weckt wartende Prozesse auf
            */
            struct sembuf v1 = V(0);
            semop(sem_id, &v1, 1); // semop erwartet einen Zeiger auf ein array, mit &v1 zeigt man sozusagen auf ein array mit einem element, eigentlich au die speicheradresse
        }
        printf("Anzahl der geschriebenen Daten: %d\n", written);


        printf("Ersten 10 Daten im Shared Memory:\n");

        for (int i = 0; i < 10 ; i++) {
            printf("%d ", shared_data[i]);

        }

        // Warten auf Kindprozess
        wait(NULL);

        // Ressourcen freigeben
        // shmdt: trennt den Shared Memory vom Adressraum des Prozesses
        shmdt(shared_data);
        // shmctl: löscht das Shared Memory Segment und gibt die Ressourcen frei
        shmctl(shm_id, IPC_RMID, NULL);
        // semctl: löscht das Semaphoren-Set
        semctl(sem_id, 0, IPC_RMID);
        printf("\n");

        printf("Erzeuger: Fertig.\n");

    }

    // pid == 0 bedeutet, dass wir im Kindprozess sind (Verbraucherprozess)
    else if (pid == 0) {
        /**
         * Liest die Daten blockweise aus dem Shared Memory.
         * Synchronisiert mit dem Erzeugerprozess über Semaphoren.
         */
        shared_data = (int *)shmat(shm_id, NULL, 0);
        if (shared_data == (void *)-1) {
            perror("shmat consumer");
            exit(1);
        }

        int received = 0; // Anzahl der bereits gelesenen Daten
        while (received < N_DATA) {
            // Warte auf Lesefreigabe (S1)
            struct sembuf p1 = P(0);
            semop(sem_id, &p1, 1);

            int count = (N_DATA - received > N_SHARED) ? N_SHARED : (N_DATA - received);
            for (int i = 0; i < count; i++) {
                int val = shared_data[i];

            }
            received += count;

            // Schreibfreigabe setzen (S2)
            struct sembuf v2 = V(1);
            semop(sem_id, &v2, 1);
        }
        printf("Anzahl der gelesenen Daten: %d\n", received);

        // Optional: Ausgabe der ersten 10 gelesenen Daten
        printf("Ersten 10 Daten gelesen vom Shared Memory:\n");

        for (int i = 0; i < 10; i++) {
            printf("%d ", shared_data[i]);

        }

        shmdt(shared_data);
        printf("\n");

        printf("Verbraucher: Fertig.\n");

    }

    return 0;
}