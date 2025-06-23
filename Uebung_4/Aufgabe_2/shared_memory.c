#include <stdio.h>      // printf, perror
#include <stdlib.h>     // exit
#include <sys/ipc.h>    // IPC_PRIVATE, IPC_CREAT (Shared Memory, Interprozesskommunikation)
#include <sys/shm.h>    // shmget, shmat, shmdt, shmctl (Shared Memory)
#include <unistd.h>     // fork, getpid
#include <time.h>       // time
#include <sys/wait.h>   // wait
#include <semaphore.h>  // sem_t, sem_init, sem_wait, sem_post, sem_destroy (POSIX-Semaphore)
#include <sys/mman.h>   // mmap, munmap, PROT_READ, PROT_WRITE, MAP_SHARED, MAP_ANONYMOUS, MAP_FAILED

#define N_SHARED 2000

int main() {
    int shmid;
    int *shared_data;

    // mmap: Legt einen Bereich im Shared Memory an, um Semaphore zwischen Prozessen zu teilen.
    // Parameter: Adresse (NULL = vom System gewählt), Größe, Zugriffsrechte, Flags, Dateideskriptor, Offset
    // Bibliothek: sys/mman.h
    sem_t *sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sem == MAP_FAILED) {
        // perror: Gibt eine Fehlermeldung für die letzte Systemfunktion aus.
        // Bibliothek: stdio.h
        perror("mmap");
        // exit: Beendet das Programm mit Fehlercode.
        // Bibliothek: stdlib.h
        exit(1);
    }
    // sem_init: Initialisiert einen POSIX-Semaphor.
    // Parameter: Zeiger auf sem_t, pshared (1 = zwischen Prozessen), Startwert
    // Bibliothek: semaphore.h
    if (sem_init(sem, 1, 0) == -1) {
        perror("sem_init");
        exit(1);
    }

    // shmget: Erstellt oder öffnet ein Shared Memory Segment.
    // Parameter: Schlüssel, Größe, Flags (z.B. Erstellen, Rechte)
    // Bibliothek: sys/shm.h
    shmid = shmget(IPC_PRIVATE, N_SHARED * sizeof(int), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    // fork: Erzeugt einen Kindprozess (Kopie des aktuellen Prozesses).
    // Rückgabewert: 0 im Kindprozess, PID des Kindes im Elternprozess, -1 bei Fehler
    // Bibliothek: unistd.h
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // shmat: Bindet das Shared Memory Segment in den Adressraum ein.
        // Parameter: shmid, Adresse (NULL = vom System gewählt), Flags
        // Bibliothek: sys/shm.h
        shared_data = (int *)shmat(shmid, NULL, 0);
        if (shared_data == (void *)-1) {
            perror("shmat (child)");
            exit(1);
        }
        printf("P2 wartet auf die Daten vom Vaterprozess...\n");
        // sem_wait: Wartet, bis der Semaphor > 0 ist, dekrementiert ihn dann.
        // Parameter: Zeiger auf sem_t
        // Bibliothek: semaphore.h
        sem_wait(sem);
        printf("P2 liest die ersten 10 Werte:\n");
        for (int i = 0; i < 10; ++i) {
            printf("%d ", shared_data[i]);
        }
        printf("\n");
        // shmdt: Löst das Shared Memory Segment vom Adressraum.
        // Parameter: Zeiger auf das Segment
        // Bibliothek: sys/shm.h
        shmdt(shared_data);
        exit(0);
    } else {
        shared_data = (int *)shmat(shmid, NULL, 0);
        if (shared_data == (void *)-1) {
            perror("shmat (parent)");
            exit(1);
        }
        // srand48: Initialisiert den Zufallszahlengenerator.
        // Parameter: Startwert (z.B. time(NULL))
        // Bibliothek: stdlib.h
        srand48(time(NULL));
        for (int i = 0; i < N_SHARED; ++i) {
            // lrand48: Gibt eine nicht-negative Zufallszahl zurück.
            // Bibliothek: stdlib.h
            shared_data[i] = lrand48();
        }
        shmdt(shared_data);
        // sem_post: Erhöht den Semaphor um 1 (Signal an wartende Prozesse).
        // Parameter: Zeiger auf sem_t
        // Bibliothek: semaphore.h
        sem_post(sem);
        // wait: Wartet auf die Beendigung eines Kindprozesses.
        // Parameter: Zeiger auf Status (NULL = ignorieren)
        // Bibliothek: sys/wait.h
        wait(NULL);
        // shmctl: Führt Steueroperationen auf Shared Memory Segmenten aus (z.B. löschen).
        // Parameter: shmid, Befehl (z.B. IPC_RMID), optionales Argument
        // Bibliothek: sys/shm.h
        shmctl(shmid, IPC_RMID, NULL);
        // sem_destroy: Zerstört einen POSIX-Semaphor.
        // Parameter: Zeiger auf sem_t
        // Bibliothek: semaphore.h
        sem_destroy(sem);
        // munmap: Gibt einen mit mmap belegten Speicherbereich frei.
        // Parameter: Adresse, Größe
        // Bibliothek: sys/mman.h
        munmap(sem, sizeof(sem_t));
    }
    return 0;
}