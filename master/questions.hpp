#pragma once
#ifndef QUESTIONS_HPP
#define QUESTIONS_HPP

#include <vector>
#include <string>

class Questions
{
private:
    enum
    {
        NUMBER_OF_QUESTIONS = 4
    };

public:
    Questions(const std::string &question, const std::array<std::string, NUMBER_OF_QUESTIONS> &answers);
    std::array<std::string, NUMBER_OF_QUESTIONS> getAnswers() const;
    std::string getCorrectAnswer() const;
    std::string getQuestionBody() const;
    void setCorrectAnswerIndex(const int index);
    bool isCorrectAnswer(const std::string &answer);

private:
    std::array<std::string, NUMBER_OF_QUESTIONS> mAnswers;
    int mCorrectAnswerIndex;
    std::string mQuestion;
};

#endif