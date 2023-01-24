#pragma once
#ifndef QUESTIONS_HPP
#define QUESTIONS_HPP

#include <array>
#include <string>

class Questions
{
private:
    enum
    {
        NUMBER_OF_ANSWERS = 4
    };

public:
    Questions(const std::string &question, const std::array<std::string, NUMBER_OF_ANSWERS> &answers);
    std::array<std::string, NUMBER_OF_ANSWERS> getAnswers() const;
    std::string getCorrectAnswer() const;
    std::string getQuestionBody() const;
    void setCorrectAnswerIndex(const int index);
    bool isCorrectAnswer(const std::string &answer);

private:
    std::array<std::string, NUMBER_OF_ANSWERS> mAnswers;
    int mCorrectAnswerIndex;
    std::string mQuestion;
};

#endif