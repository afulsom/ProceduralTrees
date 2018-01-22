#pragma once
#include <string>
#include <map>

struct Rule
{
	Rule(std::string replacementString, unsigned int weight)
		: m_stringToReplaceWith(replacementString), m_weight(weight)
	{
	}

	std::string m_stringToReplaceWith;
	unsigned int m_weight;
};

class LSystem
{
public:
	LSystem();
	~LSystem();

	std::string Run();
	std::string Run(unsigned int numIterations);

public:
	std::string m_axiom;
	std::multimap<char, Rule> m_rules;
	unsigned int m_numIterations;
};