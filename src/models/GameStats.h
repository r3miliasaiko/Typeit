#pragma once

struct GameStats {
    int correctWords = 0;
    int missedWords = 0;
    int wrongAttempts = 0;
    int currentCombo = 0;
    int maxCombo = 0;
    float health = 100.0f;
    
    void onCorrect() {
        correctWords++;
        currentCombo++;
        if (currentCombo > maxCombo) {
            maxCombo = currentCombo;
        }
    }
    
    void onMiss() {
        missedWords++;
        resetCombo();
    }
    
    void onWrong() {
        wrongAttempts++;
        resetCombo();
    }
    
    void resetCombo() {
        currentCombo = 0;
    }
    
    double getAccuracy() const {
        int total = correctWords + missedWords + wrongAttempts;
        if (total == 0) return 100.0;
        return (correctWords * 100.0) / total;
    }
};

