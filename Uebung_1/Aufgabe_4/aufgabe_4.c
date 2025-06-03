//
// Created by Hanan on 4/23/2025.
//
#include <stdio.h>
#include <math.h>

#define WORDS_GERMAN 84000
#define WORDS_ENGLISH 100000

void calculateEntropy(void){
    float resultGerman = log2(WORDS_GERMAN);
    float resultEnglish = log2(WORDS_ENGLISH);

    printf("Entropy for the german words: %.2f \n", resultGerman);
    printf("Entropy for the english words: %.2f \n", resultEnglish);
}

int main(){
    calculateEntropy();
    return 0;
}