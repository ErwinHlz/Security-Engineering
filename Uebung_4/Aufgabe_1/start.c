#include <unistd.h> // Library, die fork() und execvp() enthält
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h> // enthält setpriority Methode
#include <sys/wait.h> // enthält waitpid()
#include <signal.h> // enthält psignal()



// argc steht für argument count, also Anzahl der Argumente
// char** agrgv[] ist ein Pointer zu einem Pointer zu einem String, also ein Pointer zu einer Array von Strings
// Wenn wir in Command Line mehrere Arguments haben, können wir jede einzelne Buchstaben mithilfe von diesem Array zugreifen
// z.B, "ls -l /home". ptr[0][1] gibt uns "s von ls" zurück.
    

int main(int argc, char** argv){  
    
    if (argc < 2) {
        //fprintf wird benutzt, um standard-errors auszugeben. 
        fprintf(stderr, "Bitte mindestens ein Argument eingeben\n");
        return EXIT_FAILURE;
    }


    //pid_t ist eine Variable, die die PID eines Programms speichert. 
    //fork() dupliziert der aktuelle Prozess. 
    pid_t pid = fork();

    //Code, kein Kindprozess gestartet wurde
    if (pid < 0){
        perror("Kein Kindprozess wurde gestartet");
        return EXIT_FAILURE;
    }

    //Code, der nur von Kindprozess ausgeführt wird
    if (pid == 0){
        //lowers the child process priority and starts a new programm
        // setpriority lowers the prioroty to 19(which is the lowest, -20 is the highest and need root rights)
        // PRIO_Process tells to set prioroty for a process
        // 0,19 --> means set priority 19 for the pid 0. 
        // If the priority fails, then exit with and error
        if (setpriority(PRIO_PROCESS,0,19) < 0){
            perror("setpriority");
            exit(EXIT_FAILURE);
        }
        //Replace the child process with the new programm(also was wir in command line eingeben)
        execvp(argv[1], argv +1);
        //Falls execvp nicht funktioniert, mit Exit Code 1 beenden.
        perror("execvp");
        exit(EXIT_FAILURE);
        
    // Code, der nur von Elternprozess ausgeführt wird
    } else {
        int status;
        printf("Gestarteter Prozess hat PID: %d\n", pid);

        //wartet, bist der Kindprozess fertig ist. waitpid("id von übergegebenen Programm", "Statuscode wird gespeichert", 0 bedeutet blockierend warten bis der Prozess fertig ist)

        if (waitpid(pid, &status, 0) == -1){
            perror("waitpid");
            return EXIT_FAILURE;
        }

        //gucken ob der Prozess normal beendet ist
        if (WIFEXITED(status)){
            printf("Prozess endete Normal mit Exit-Code: %d\n", WEXITSTATUS(status));

        //Falls der Prozess durch eine Signal beendet wird(zb kill, Sigkill usw)    
        } else if (WIFSIGNALED(status)){
            //holt die Signal und sagt uns welchen Signal das ist
            int sig = WTERMSIG(status);
            printf("Prozess wurde durch Signal %d beendet: ", sig);
            //Das gibt uns zusätzlich noch die Name von dem Signal, z.B Segment Fault usw.
            psignal(sig, "");
        } else {
            printf("Prozess endet auf ungewöhnliche Weise");
        }
        
    }
    return EXIT_SUCCESS;
    
}