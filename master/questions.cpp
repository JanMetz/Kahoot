#include "questions.hpp"

#include <array>
#include <string>


Questions::Questions(const std::string &question, const std::array<std::string, NUMBER_OF_QUESTIONS> &answers) : mAnswers(answers)
{

}

std::array<std::string, NUMBER_OF_QUESTIONS> Questions::getAnswers() const
{
    return mAnswers;
}

std::string Questions::getCorrectAnswer() const
{
    return mAnswers[mCorrectAnswerIndex];
}

void Questions::setCorrectAnswerIndex(const int index)
{
    if (mCorrectAnswerIndex >= NUMBER_OF_QUESTIONS)
        return;

    mCorrectAnswerIndex = index;
}

bool Questions::isCorrectAnswer(const std::string &answer)
{
    return answer == getCorrectAnswer();
}
