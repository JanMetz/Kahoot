#include "questions.hpp"

#include <array>
#include <string>


Questions::Questions(const std::string &question, const std::array<std::string, NUMBER_OF_QUESTIONS> &answers)
{

}

std::array<std::string, NUMBER_OF_QUESTIONS> Questions::getAnswers() const
{
    return std::array<std::string, NUMBER_OF_QUESTIONS>();
}

std::string Questions::getCorrectAnswer() const
{
    return "";
}

void Questions::setCorrectAnswerIndex(const int index)
{

}

bool Questions::isCorrectAnswer(const std::string &answer)
{
    return false;
}
